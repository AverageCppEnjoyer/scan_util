#ifndef SCAN_UTIL_DETECT_H
#define SCAN_UTIL_DETECT_H

#include <string>
#include <utility>
#include <vector>

enum class DetectType {
    JS = 0,
    UNIX = 1,
    MACOS = 2,
    NONE = 3
};

const static std::vector<std::pair<std::string,std::string>> detect_rules {
    {"<script>evil_script()</script>", ".js"},
    {"rm -rf ~/Documents", ""},
    {"system(\"launchctl load /Library/LaunchAgents/com.malware.agent\")", ""},
    {"", ""}
};

struct Detect {
public:
    explicit Detect(DetectType dt) : type_(dt) {};

    DetectType type_ = DetectType::NONE;

    [[nodiscard]] std::string GetPattern() const {
        return detect_rules.at(static_cast<int>(type_)).first;
    }

    [[nodiscard]] std::string GetExtension() const {
        return detect_rules.at(static_cast<int>(type_)).second;
    }
};

#endif //SCAN_UTIL_DETECT_H
