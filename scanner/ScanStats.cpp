#include <iomanip>
#include <cassert>

#include "ScanStats.h"

void ScanStats::SetTime(std::chrono::milliseconds ms_count) {
    exec_time_ = ms_count;
}

void ScanStats::IncErrors() {
    errors_++;
}

void ScanStats::UpdateSuspicious(const Detect &d) {
    processed_files_++;
    switch (d.type_) {
        case DetectType::JS:
            js_suspicious_++;
            break;
        case DetectType::UNIX:
            unix_suspicious_++;
            break;
        case DetectType::MACOS:
            macos_suspicious_++;
            break;
        case DetectType::NONE:
            break;
        default:
            assert(false);
    }
}

std::ostream& operator<<(std::ostream& os,const ScanStats& s) {
    using namespace std::chrono;
    auto ms = s.exec_time_;
    auto hrs = duration_cast<hours>(ms);
    ms -= hrs;
    auto mnts = duration_cast<minutes>(ms);
    ms -= mnts;
    auto sec = duration_cast<seconds>(ms);
    ms -= sec;

    os << "====== Scan result ======" << '\n';
    os << "Processed files: " <<  s.processed_files_ << '\n';
    os << "JS detects: " << s.js_suspicious_ << '\n';
    os << "Unix detects: " << s.unix_suspicious_ << '\n';
    os << "macOS detects: " << s.macos_suspicious_ << '\n';
    os << "Errors: " << s.errors_ << '\n';
    os << "Execution time: "
        << std::setw(2) << std::setfill('0') << hrs.count() << ":"
          << std::setw(2) << std::setfill('0') << mnts.count() << ":"
              << std::setw(2) << std::setfill('0') << sec.count() << '\n';
    os << "=========================" << '\n';

    return os;
}