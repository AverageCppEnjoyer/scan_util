#ifndef SCAN_UTIL_FILESCANNER_H
#define SCAN_UTIL_FILESCANNER_H

#include <cassert>
#include <filesystem>
#include <vector>
#include <string>

#include "Detect.h"

namespace fs = std::filesystem;

class FileScannerBase {
protected:
    static const std::vector<Detect> searchable_detects_;
    //In general, the scanner does not know about the file extension,
    // so it is necessary to select suspicious detects for a specific file
    std::vector<Detect> FilterAllowedDetects(const fs::path& filepath) const;
public:
    virtual Detect Scan(const fs::path& filepath) const = 0;
    virtual ~FileScannerBase() = default;
};

class SimpleFileScanner : public FileScannerBase {
public:
    Detect Scan(const fs::path& filepath) const override;
    ~SimpleFileScanner() override = default;
};

class ComplexFileScanner : public FileScannerBase {
public:
    Detect Scan(const fs::path& filepath) const override;
    ~ComplexFileScanner() override = default;
};

#endif //SCAN_UTIL_FILESCANNER_H
