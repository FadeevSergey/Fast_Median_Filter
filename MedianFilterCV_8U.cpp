#include <string>
#include <cmath>

#include "MedianFilterCV_8U.h"

MedianFilterCV_8U::MedianFilterCV_8U(const std::string &imagePath, uchar threshold) : threshold(threshold) {
    setSourceImage(imagePath);
}

MedianFilterCV_8U::MedianFilterCV_8U(const cv::Mat &image, uchar threshold) : threshold(threshold) {
    setSourceImage(image);
}

MedianFilterCV_8U::MedianFilterCV_8U(const uchar **matrix, int height, int width, uchar threshold) : threshold(
        threshold) {
    setSourceImage(matrix, height, width);
}

MedianFilterCV_8U::~MedianFilterCV_8U() {
    delete sourceImage;
    delete resultImage;
}

uchar MedianFilterCV_8U::getThreshold() const {
    return threshold;
}

const cv::Mat &MedianFilterCV_8U::getResultImage() const {
    return *resultImage;
}

void MedianFilterCV_8U::setSourceImage(const std::string &imagePath) {
    delete sourceImage;

    sourceImage = new cv::Mat(cv::imread(imagePath));
    convertSourceImageToGrayCV_8U();
}

void MedianFilterCV_8U::setSourceImage(const cv::Mat &image) {
    delete sourceImage;

    sourceImage = new cv::Mat(image);
    convertSourceImageToGrayCV_8U();
}

void MedianFilterCV_8U::setSourceImage(const uchar **matrix, int rows, int cols) {
    delete sourceImage;

    sourceImage = new cv::Mat(rows, cols, CV_8U);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            sourceImage->at<ushort>(i, j) = matrix[i][j];
        }
    }
}

void MedianFilterCV_8U::setThreshold(uchar newThreshold) {
    threshold = newThreshold;
}

//1 + 1 + 5 * 539 + 1 + 5 * 959 + 958 * 539 * 438 + 539 * 567 + operations of filterImageEdges
//225.479.662 + 1.085.306 = 226 564 958
void MedianFilterCV_8U::filter() {
    resultImage = new cv::Mat(sourceImage->rows, sourceImage->cols, CV_8U);

    std::pair<uchar, uchar> pairOfMedians;                                // 1 operation
    for (int y = 1; y < sourceImage->rows - 1; ++y) {                     // 1 + 5 operations at every step (539 steps if rows=1080)
        for (int x = 1; x < sourceImage->cols - 2; x += 2) {              // 1 + 5 operations at every step (959 steps if cols=1920)
            pairOfMedians = getPairMedian(*sourceImage, y, x, false);     // 1 operation + operations of getPairMedian(420/567)

            resultImage->at<uchar>(y, x) = getActualPixel(pairOfMedians.first, y, x);          // 2 operations + operations of getActual(9)
            resultImage->at<uchar>(y, x + 1) = getActualPixel(pairOfMedians.second, y, x + 1); // 4 operations + operations of getActual(9)
        }
    }

    // if the last two columns are not processed
    if (sourceImage->cols % 2 == 1) {                                     // 3 operations
        int x = sourceImage->cols - 2;                                    // 3 operations

        for (int y = 1; y < sourceImage->rows - 2; y += 2) {              // 1 + 5 operations at every step
            pairOfMedians = getPairMedian(*sourceImage, y, x, true);      // 2 operations + operations of getPairMedian (420/567)
            resultImage->at<uchar>(y, x) = getActualPixel(pairOfMedians.first, y, x);          // 3 operations + operations of getActual(9)
            resultImage->at<uchar>(y + 1, x) = getActualPixel(pairOfMedians.second, y + 1, x); // 5 operations + operations of getActual(9)
        }

        if (sourceImage->rows % 2 == 1) {
            int y = sourceImage->rows - 2;                                // 3 operations
            resultImage->at<uchar>(sourceImage->rows - 2, sourceImage->cols - 2) =
                    getActualPixel(getMedian(*sourceImage, y, x), y, x);  // 6 operations + operations of getActaulPixel
                                                                          // and getMedian
        }
    }

    filterImageEdges(*sourceImage);                                       // 1 operations + operations of filterImageEdges
}

// 9 operations
uchar MedianFilterCV_8U::getActualPixel(uchar newPixel, int y, int x) const {
    if (std::abs(sourceImage->at<uchar>(y, x) - newPixel) <= threshold) { // 3 operations + operations of std::abs()
        return sourceImage->at<uchar>(y, x);                              // 2 operation
    } else {
        return newPixel;                                                  // 1 operation
    }
}

void MedianFilterCV_8U::convertSourceImageToGrayCV_8U() {
    cv::Mat *intermediateImage = new cv::Mat;
    sourceImage->convertTo(*intermediateImage, CV_8U);

    cv::cvtColor(*intermediateImage, *sourceImage, cv::COLOR_BGR2GRAY);

    delete intermediateImage;
}

//420 if (x == 1 && !pairOnOneLine) || (y == 1 && pairOnOneLine) = false
//567 if (x == 1 && !pairOnOneLine) || (y == 1 && pairOnOneLine) = true

std::pair<uchar, uchar> MedianFilterCV_8U::getPairMedian(const cv::Mat &image, int y, int x, bool pairOnOneLine) {
    // PairOnOneLine - true, if we are looking for medians on one line,
    //                false, if on one column
    if ((x == 1 && !pairOnOneLine) || (y == 1 && pairOnOneLine)) {                         // 7 operations
        //if we are looking for median on second line or second column
        //need init must fill out and initialize third and fourth temp array
        if (pairOnOneLine) {                                                               // 1 operation
            initThreeAdjacentPixels(image, thirdTempArray, y - 1, x, pairOnOneLine);       // 14| 2 operations + operations of init(12)
            initThreeAdjacentPixels(image, fourthTempArray, y, x, pairOnOneLine);          // 13| 1 operations + operations of init(12)
        } else {
            initThreeAdjacentPixels(image, thirdTempArray, y, x - 1, pairOnOneLine);       // 13| 2 operations + operations of init(12)
            initThreeAdjacentPixels(image, fourthTempArray, y, x, pairOnOneLine);          // 13| 1 operations + operations of init(12)
        }

        sortThreeElements(thirdTempArray);                                                 // 43| 1 operations + operations of sort(42)
        sortThreeElements(fourthTempArray);                                                // 43| 1 operations + operations of sort(42)
    }

    // The values ​​sorted in the last step are written to first and second temp array
    std::swap(firstTempArray, thirdTempArray);                                             // 4 operations
    std::swap(secondTempArray, fourthTempArray);                                           // 4 operations

    if (pairOnOneLine) {                                                                   // 1 operation
        initThreeAdjacentPixels(image, thirdTempArray, y + 1, x, pairOnOneLine);           // 2 operations
        initThreeAdjacentPixels(image, fourthTempArray, y + 2, x, pairOnOneLine);          // 2 operations
    } else {
        initThreeAdjacentPixels(image, thirdTempArray, y, x + 1, pairOnOneLine);           // 2 operations
        initThreeAdjacentPixels(image, fourthTempArray, y, x + 2, pairOnOneLine);          // 2 operations
    }

    sortThreeElements(thirdTempArray);                                                     // 43| 1 operation + operations of sort(42)
    sortThreeElements(fourthTempArray);                                                    // 43| 1 operation + operations of sort(42)

    uchar mergeResult[6];                                                                  // 1 operation
    mergeTwoSortedArray(secondTempArray, 3, thirdTempArray, 3, mergeResult);               // 118| 1 operation + operations of merge(117)

    std::pair<uchar, uchar> pairOfMedians;                                                 // 1 operations

    pairOfMedians.first = getMedianOfTwoSortedArray(mergeResult, 6, firstTempArray, 3);    // 98| 3 operations + opeations of getMedian(95)
    pairOfMedians.second = getMedianOfTwoSortedArray(mergeResult, 6, fourthTempArray, 3);  // 98| 3 operations + opeations of getMedian(95)

    return pairOfMedians;                                                                  // 1 operation
}

// 3 + 4 * 6 + 15 * 6 = 117 operations if firstArraySize = secondArraySize = 3
// 3 + 4 * 9 + 15 * 9 = 174 operations firstArraySize = 6, secondArraySize = 3
void MedianFilterCV_8U::mergeTwoSortedArray(const uchar *firstArray, size_t firstArraySize,
                                            const uchar *secondArray, size_t secondArraySize, uchar *result) const {
    for (int i = 0, f = 0, s = 0; i < firstArraySize + secondArraySize; ++i) {                 // 3 operations + 4 at every step
        if (s == secondArraySize || (f != firstArraySize && firstArray[f] < secondArray[s])) { // 9 operations
            result[i] = firstArray[f++];                                                       // 6 operations
        } else {
            result[i] = secondArray[s++];                                                      // 6 operations
        }
    }
}

// 42 operations
void MedianFilterCV_8U::sortThreeElements(uchar *arrayOfThreeElements) const {
    for (std::pair<int, int> i : pairsForSortThreeElements) {                                 // 6 operations
        if (arrayOfThreeElements[i.first] > arrayOfThreeElements[i.second]) {                 // 5 operations
            std::swap(arrayOfThreeElements[i.first], arrayOfThreeElements[i.second]);         // 7 operations
        }
    }
}

//12 operations
void MedianFilterCV_8U::initThreeAdjacentPixels(const cv::Mat &image, uchar *threeAdjacentPixels, int y, int x,
                                                bool pixelsOnOneLine) const {

    if (pixelsOnOneLine) {                                                                   // 1 operation
        threeAdjacentPixels[0] = image.at<uchar>(y, x - 1);                                  // 4 operations
        threeAdjacentPixels[1] = image.at<uchar>(y, x);                                      // 3 operations
        threeAdjacentPixels[2] = image.at<uchar>(y, x + 1);                                  // 4 operations
    } else {
        threeAdjacentPixels[0] = image.at<uchar>(y - 1, x);                                  // 4 operations
        threeAdjacentPixels[1] = image.at<uchar>(y, x);                                      // 3 operations
        threeAdjacentPixels[2] = image.at<uchar>(y + 1, x);                                  // 4 operations
    }
}

// 1 + 28 + 5 * 13 + 1 = 95 operations if firstArraySize = 6, secondArraySize = 3
uchar MedianFilterCV_8U::getMedianOfTwoSortedArray(const uchar *firstArray, size_t firstArraySize,
                                                   const uchar *secondArray, size_t secondArraySize) const {
    uchar result = 0;                                                                         // 1 operation

    for (int i = 0, f = 0, s = 0; i <= (firstArraySize + secondArraySize) / 2; ++i) {         // 3 operations + 5 at every step
        if (s == secondArraySize || (f != firstArraySize && firstArray[f] < secondArray[s])) {// 7 operations
            result = firstArray[f++];                                                         // 6 operations
        } else {
            result = secondArray[s++];                                                        // 6 operations
        }
    }

    return result;                                                                            // 1 operation
}

//1085306 if 1920x1080
void MedianFilterCV_8U::filterImageEdges(const cv::Mat &image) {


    // Array of ranges to filter. Each range consists of two pairs.
    // The first and second is a range in y, third and fourth is a range in x.
    // For example {0, 0, 0, image.cols - 1} range specifying the first line
    int ranges[4][4] = {{0, 0, 0, image.cols - 1},
                        {1, image.rows - 1, 0, 0},
                        {image.rows - 1, image.rows - 1, 0, image.cols - 1},
                        {1, image.rows - 1, image.cols - 1, image.cols - 1}}; // 42 operations


    uchar tempMedian;                                                         // 1 operations
    // if first or last line (len = 1920)  (1 + 2 + 4 + 2 + 4 * 1920 + 1920 * (1 + 4 + 1 + 14 + 14 + ~80 + 63)) * 2 = 695052
    // if the first or last column (hei = 1080) (1 + 2 + 4 + 2 + 4 * 1078 + 1078 * (1 + 4 + 1 + 14 + 14 + ~80 + 63)) * 2 = 390254
    for (auto range : ranges) {                                               // 4 operations
        int position = 0;                                                     // 1 operation
        for (int y = range[0]; y <= range[1]; ++y) {                          // 2 operations + 4 at every step
            for (int x = range[2]; x <= range[3]; ++x) {                      // 2 operations + 4 at every step
                uchar array[6];                                               // 1 operation
                // Add 0 and 255 in case we are looking for a median in the corners.
                // Thus we process corner pixels as well as non-corner pixels
                array[4] = 0;                                                 // 2 operations
                array[5] = 255;                                               // 2 operations

                position = 0;                                                 // 1 operation
                for (int i = y - 1; i <= y + 1; ++i) {                        // 2 operations + 12
                    for (int j = x - 1; j <= x + 1; ++j) {                    // 2 operations + 12
                        if (i < 0 || j < 0 || i >= image.rows || j >= image.cols) { // 9 operations
                            continue;                                         // 1 operation
                        }

                        array[position++] = image.at<uchar>(i, j);            // 7 operations
                    }
                }

                // sort the first 3 and last 3 elements
                sortThreeElements(array);                                     // 1 operation + operations of sort
                sortThreeElements(array + 3);                                 // 2 operations + operations of sort

                tempMedian = getMedianOfTwoSortedArray(array, 3, array + 3, 3);// 3 operations
                resultImage->at<uchar>(y, x) = getActualPixel(tempMedian, y, x);// 3 operations
            }
        }
    }
}

uchar MedianFilterCV_8U::getMedian(const cv::Mat &, int y, int x) {
    initThreeAdjacentPixels(*sourceImage, firstTempArray, y - 1, x, true);
    initThreeAdjacentPixels(*sourceImage, secondTempArray, y, x, true);
    initThreeAdjacentPixels(*sourceImage, thirdTempArray, y + 1, x, true);

    sortThreeElements(firstTempArray);
    sortThreeElements(secondTempArray);
    sortThreeElements(thirdTempArray);

    uchar mergeResult[6];
    mergeTwoSortedArray(firstTempArray, 3, secondTempArray, 3, mergeResult);

    return getMedianOfTwoSortedArray(mergeResult, 6, thirdTempArray, 3);
}


