#include <cstdlib>

#include <opencv2/opencv.hpp>
#include "MedianFilterCV_8U.h"

int main(int argc, char *argv[]) {
    MedianFilterCV_8U *filter = nullptr;

    if (argc == 4) {
        filter = new MedianFilterCV_8U(argv[1], std::atoi(argv[3]));
    } else if (argc == 3) {
        filter = new MedianFilterCV_8U(argv[1]);
    } else {
        std::cout << "Wrong arguments, follow this format:" << std::endl;
        std::cout << "./MedianFilter PATH_TO_INPUT_IMAGE PATH_TO_OUTPUT_IMAGE" << std::endl;
        std::cout << "OR" << std::endl;
        std::cout << "./MedianFilter PATH_TO_INPUT_IMAGE PATH_TO_OUTPUT_IMAGE THRESHOLD_VALUE" << std::endl;
        return 1;
    }

    filter->filter();
    cv::imwrite(argv[2], filter->getResultImage());
    delete filter;
}
