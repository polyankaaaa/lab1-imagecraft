#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

#include "image.h"

inline int ClampInt(int v, int lo, int hi) {
    return std::max(lo, std::min(v, hi));
}

inline uint8_t ClampU8(int v) {
    if (v < 0) return 0;
    if (v > 255) return 255;
    return static_cast<uint8_t>(v);
}

inline double Luma01(const Pixel& p) {
    return (0.299 * p.r + 0.587 * p.g + 0.114 * p.b) / 255.0;
}

inline Pixel GetClamped(const Image& img, int x, int y) {
    x = ClampInt(x, 0, img.GetWidth() - 1);
    y = ClampInt(y, 0, img.GetHeight() - 1);
    return img.GetPixel(x, y);
}

inline std::vector<double> GaussianKernel1D(double sigma) {
    if (sigma <= 0.0) {
        return {1.0};
    }
    const int radius = static_cast<int>(std::ceil(3.0 * sigma));
    const int size = 2 * radius + 1;
    std::vector<double> k(static_cast<size_t>(size));
    double sum = 0.0;
    for (int i = -radius; i <= radius; ++i) {
        const double v = std::exp(-(i * i) / (2.0 * sigma * sigma));
        k[static_cast<size_t>(i + radius)] = v;
        sum += v;
    }
    for (double& v : k) v /= sum;
    return k;
}