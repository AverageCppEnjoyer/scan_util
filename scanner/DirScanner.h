#ifndef SCAN_UTIL_DIRSCANNER_H
#define SCAN_UTIL_DIRSCANNER_H

#include "ScanStats.h"
#include "FileScanner.h"
#include "Detect.h"

#include <filesystem>
#include <future>
#include <memory>
#include <cassert>

enum class ScanAlgoType {
    NAIVE,
    AHO_CORASICK
};

ScanStats ScanDirectory(const std::string& dir_path,ScanAlgoType scan_type = ScanAlgoType::NAIVE) {
    using namespace std::chrono;
    using namespace std::filesystem;

    assert(exists(dir_path) && is_directory(dir_path));

    ScanStats stats;
    const auto start_time = high_resolution_clock::now();

    std::vector<std::future<Detect>> futures;

    for (const auto& entry: directory_iterator(dir_path)) {
        if (entry.is_regular_file()) {
            futures.push_back(
               std::async(
                    std::launch::async, [scan_type](const auto& path) {
                        std::unique_ptr<FileScannerBase> file_scanner;

                        switch (scan_type) {
                            case ScanAlgoType::NAIVE:
                                file_scanner = std::make_unique<SimpleFileScanner>();
                                break;
                            case ScanAlgoType::AHO_CORASICK:
                                file_scanner = std::make_unique<ComplexFileScanner>();
                                break;
                            default:
                                assert(false);
                        }

                        return file_scanner->Scan(path);
                    },
                 entry.path()
               )
            );
        }
    }

    for (auto& future: futures) {
        try {
            stats.UpdateSuspicious(future.get());
        } catch (...) {
            stats.IncErrors();
        }
    }

    const auto end_time = high_resolution_clock::now();
    stats.SetTime(duration_cast<milliseconds>(end_time - start_time));

    return stats;
}

#endif //SCAN_UTIL_DIRSCANNER_H
