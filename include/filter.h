#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "image.h"
#include "utils.h"

class Filter {
public:
    virtual ~Filter() = default;
    virtual Image Apply(const Image& src) const = 0;
};

class MatrixFilter : public Filter {
public:
    explicit MatrixFilter(std::vector<std::vector<double>> kernel)
        : kernel_(std::move(kernel)) {
        if (kernel_.empty() || kernel_[0].empty()) {
            throw std::invalid_argument("MatrixFilter: empty kernel");
        }
        const std::size_t h = kernel_.size();
        const std::size_t w = kernel_[0].size();
        for (const auto& row : kernel_) {
            if (row.size() != w) {
                throw std::invalid_argument("MatrixFilter: ragged kernel");
            }
        }
        if (h % 2 == 0 || w % 2 == 0) {
            throw std::invalid_argument("MatrixFilter: kernel size must be odd");
        }
    }

    Image Apply(const Image& src) const override {
        Image dst(src.Width(), src.Height());

        const long long kw = static_cast<long long>(kernel_[0].size());
        const long long kh = static_cast<long long>(kernel_.size());
        const long long rx = kw / 2;
        const long long ry = kh / 2;

        for (std::size_t y = 0; y < src.Height(); ++y) {
            for (std::size_t x = 0; x < src.Width(); ++x) {
                Pixel acc{0.0, 0.0, 0.0};

                for (long long j = -ry; j <= ry; ++j) {
                    for (long long i = -rx; i <= rx; ++i) {
                        const Pixel& p = GetPixelClamped(
                            src,
                            static_cast<long long>(x) + i,
                            static_cast<long long>(y) + j
                        );
                        const double k = kernel_[static_cast<std::size_t>(j + ry)]
                                                [static_cast<std::size_t>(i + rx)];
                        acc.r += p.r * k;
                        acc.g += p.g * k;
                        acc.b += p.b * k;
                    }
                }

                dst.At(x, y) = ClampPixel01(acc);
            }
        }

        return dst;
    }

private:
    std::vector<std::vector<double>> kernel_;
};