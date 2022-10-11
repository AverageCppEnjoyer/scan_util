#ifndef SCAN_UTIL_SCANSTATS_H
#define SCAN_UTIL_SCANSTATS_H
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

#include "Detect.h"

class ScanStats {
private:
    int32_t processed_files_ = 0;
    int32_t js_suspicious_ = 0;
    int32_t unix_suspicious_ = 0;
    int32_t macos_suspicious_ = 0;
    int32_t errors_ = 0;

    std::chrono::duration<int32_t,std::ratio<1,1000>> exec_time_;

    friend std::ostream& operator<<(std::ostream& os,const ScanStats& stats);
public:
    void IncErrors();
    void SetTime(std::chrono::milliseconds ms_count);
    void UpdateSuspicious(const Detect& d);
};

#endif //SCAN_UTIL_SCANSTATS_H
