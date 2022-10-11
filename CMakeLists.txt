cmake_minimum_required(VERSION 3.16.3)
project(scan_util)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(scan_util main.cpp scanner/FileScanner.h scanner/FileScanner.cpp scanner/ScanStats.h scanner/ScanStats.cpp scanner/DirScanner.h scanner/Detect.h scanner/AhoCorasick.h)
