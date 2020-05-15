# CMakeLists.txt
cmake_minimum_required(VERSION 3.13)
project(MedianFilter)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

#set(CMAKE_PREFIX_PATH "/Users/Fadeev/installation/OpenCV-")
set(CMAKE_PREFIX_PATH "YOUR_PATH_TO_OPENCV") 

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(MedianFilter main.cpp MedianFilterCV_8U.h MedianFilterCV_8U.cpp)
target_link_libraries(MedianFilter ${OpenCV_LIBS})