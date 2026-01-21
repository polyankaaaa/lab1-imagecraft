#pragma once

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <string>

#include "image.h"

inline double Clamp01(double v) {
    if (v < 0.0) {
        return 0.0;
    }
    if (v > 1.0) {
        return 1.0;
    }
    return v;
}

inline Pixel ClampPixel01(const Pixel& p) {
    Pixel res = p;
    res.r = Clamp01(res.r);
    res.g = Clamp01(res.g);
    res.b = Clamp01(res.b);
    return res;
}

inline std::size_t ClampIndex(long long v, std::size_t max_exclusive) {
    if (max_exclusive == 0) {
        throw std::invalid_argument("ClampIndex: empty dimension");
    }
    if (v < 0) {
        return 0;
    }
    long long hi = static_cast<long long>(max_exclusive - 1);
    if (v > hi) {
        return static_cast<std::size_t>(hi);
    }
    return static_cast<std::size_t>(v);
}

inline const Pixel& GetPixelClamped(const Image& img, long long x, long long y) {
    const std::size_t cx = ClampIndex(x, img.Width());
    const std::size_t cy = ClampIndex(y, img.Height());
    return img.At(cx, cy);
}

inline Pixel& GetPixelClamped(Image& img, long long x, long long y) {
    const std::size_t cx = ClampIndex(x, img.Width());
    const std::size_t cy = ClampIndex(y, img.Height());
    return img.At(cx, cy);
}

inline int ParseIntStrict(const std::string& s, const std::string& what) {
    std::size_t pos = 0;
    int value = 0;
    try {
        long long tmp = std::stoll(s, &pos);
        if (pos != s.size()) {
            throw std::invalid_argument("trailing characters");
        }
        if (tmp < static_cast<long long>(std::numeric_limits<int>::min()) ||
            tmp > static_cast<long long>(std::numeric_limits<int>::max())) {
            throw std::out_of_range("int overflow");
        }
        value = static_cast<int>(tmp);
    } catch (const std::exception& e) {
        throw std::invalid_argument("Invalid " + what + ": '" + s + "'");
    }
    return value;
}

inline double ParseDoubleStrict(const std::string& s, const std::string& what) {
    std::size_t pos = 0;
    double value = 0.0;
    try {
        value = std::stod(s, &pos);
        if (pos != s.size()) {
            throw std::invalid_argument("trailing characters");
        }
    } catch (const std::exception&) {
        throw std::invalid_argument("Invalid " + what + ": '" + s + "'");
    }
    return value;
}