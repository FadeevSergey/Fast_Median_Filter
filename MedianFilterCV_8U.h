#pragma once

#include <utility>
#include <opencv2/opencv.hpp>

class MedianFilterCV_8U {

public:
    explicit MedianFilterCV_8U(const std::string &imagePath, uchar threshold = 0);
    explicit MedianFilterCV_8U(const cv::Mat &image, uchar threshold = 0);
    MedianFilterCV_8U(const uchar **matrix, int height, int width, uchar threshold = 0);
    MedianFilterCV_8U(MedianFilterCV_8U &&) = delete;
    MedianFilterCV_8U(const MedianFilterCV_8U &) = delete;
    MedianFilterCV_8U() = delete;

    ~MedianFilterCV_8U();

    const cv::Mat &getResultImage() const;
    uchar getThreshold() const;

    void setSourceImage(const std::string &imagePath);
    void setSourceImage(const cv::Mat &image);
    void setSourceImage(const uchar **matrix, int height, int width);
    void setThreshold(uchar threshold);

    void filter();

    cv::Mat &operator=(const MedianFilterCV_8U &) = delete;
    cv::Mat &operator=(MedianFilterCV_8U &&) = delete;

private:
//    int
    uchar threshold = 0;
    cv::Mat *sourceImage = nullptr;
    cv::Mat *resultImage = nullptr;

    uchar firstTempArray[3] = {0, 0, 0};
    uchar secondTempArray[3] = {0, 0, 0};
    uchar thirdTempArray[3] = {0, 0, 0};
    uchar fourthTempArray[3] = {0, 0, 0};

    // If you sort items in these places, then the first 3 items will be sorted
    std::pair<int, int> pairsForSortThreeElements[3] = {{1, 2}, {0, 1}, {1, 2}};

    // Returns median of one pixel
    uchar getMedian(const cv::Mat &, int y, int x);

    // Returns the median of the pixel (x, y) and (x, y + 1)
    std::pair<uchar, uchar> getPairMedian(const cv::Mat &image, int y, int x, bool pairOnOneLine);

    // Returns prevValueONxy, if |prevValueONxy - newPixel| <= threshold, else return new newPixel
    uchar getActualPixel(uchar newPixel, int y, int x) const;

    // Returns the median of two merged arrays
    uchar getMedianOfTwoSortedArray(const uchar *firstArray, size_t firstArraySize,
                                    const uchar *secondArray, size_t secondArraySize) const;

    // Applies the median filtering algorithm to the sourceImage and writes the result to resultImage
    void filterImageEdges(const cv::Mat &image);

    // Sorts an array of three elements
    void sortThreeElements(uchar *arrayOfThreeElements) const;

    // Converts sourceImage to gray image CV_8U
    void convertSourceImageToGrayCV_8U();

    // Returns the result of merging arrays
    void mergeTwoSortedArray(const uchar *, size_t, const uchar *, size_t, uchar *) const;

    // Returns an array of pixels (x - 1, y), (x, y), (x - 1, y) pexelsOnOneLine = true
    // Returns an array of pixels (x, y - 1), (x, y), (x, y + 1) pexelsOnOneLine = false
    void initThreeAdjacentPixels(const cv::Mat &image, uchar *threeAdjacentPixels, int y, int x,
                                        bool pixelsOnOneLine) const;
};
