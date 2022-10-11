#include "FileScanner.h"
#include "AhoCorasick.h"

#include <fstream>
#include <sstream>

const std::vector<Detect> FileScannerBase::searchable_detects_ = std::vector<Detect> {
        Detect{DetectType::JS},
        Detect{DetectType::UNIX},
        Detect{DetectType::MACOS}
};

std::vector<Detect> FileScannerBase::FilterAllowedDetects(const fs::path& filepath) const {
    std::vector<Detect> allowed_detects;

    for (const auto &d: FileScannerBase::searchable_detects_) {
        if (d.GetExtension().empty() || filepath.extension() == d.GetExtension()) {
            allowed_detects.push_back(d);
        }
    }

    return allowed_detects;
}

Detect SimpleFileScanner::Scan(const fs::path& filepath) const {
    auto allowed_detects = FilterAllowedDetects(filepath);
    std::ifstream input_stream(filepath);

    for (std::string line; std::getline(input_stream, line);) {
        for (const auto& detect: allowed_detects) {
            if (line.find(detect.GetPattern()) != std::string::npos) {
                return detect;
            }
        }
    }

    return Detect(DetectType::NONE);
}

Detect ComplexFileScanner::Scan(const fs::path& filepath) const {
    auto allowed_detects = FilterAllowedDetects(filepath);

    AhoCorasick ac;
    for(const auto& detect : allowed_detects) {
        ac.AddString(detect.GetPattern());
    }
    ac.Init();

    std::ifstream infile(filepath);
    std::stringstream buffer;
    buffer << infile.rdbuf();

    auto maybe_search_result = ac.Search(buffer.str());

    return (maybe_search_result.has_value()) ? allowed_detects[maybe_search_result.value()] : Detect(DetectType::NONE);
}
