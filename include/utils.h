#pragma once
#include <algorithm>
#include <cstdint>
#include <string>

inline int ClampInt(int v, int lo, int hi) {
    return std::max(lo, std::min(hi, v));
}

inline uint8_t ClampU8(int v) {
    return static_cast<uint8_t>(ClampInt(v, 0, 255));
}

inline std::string ToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return (char)std::tolower(c); });
    return s;
}