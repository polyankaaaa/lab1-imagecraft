#include "filter_factory.h"

#include "filter.h"
#include "image.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

int ToInt(const std::string& s) {
    std::size_t pos = 0;
    int v = 0;
    try {
        v = std::stoi(s, &pos);
    } catch (...) {
        throw std::invalid_argument("Invalid integer: " + s);
    }
    if (pos != s.size()) {
        throw std::invalid_argument("Invalid integer: " + s);
    }
    return v;
}

double ToDouble(const std::string& s) {
    std::size_t pos = 0;
    double v = 0.0;
    try {
        v = std::stod(s, &pos);
    } catch (...) {
        throw std::invalid_argument("Invalid double: " + s);
    }
    if (pos != s.size()) {
        throw std::invalid_argument("Invalid double: " + s);
    }
    return v;
}

std::size_t ClampIndex(long long v, std::size_t lo, std::size_t hi) {
    if (v < static_cast<long long>(lo)) return lo;
    if (v > static_cast<long long>(hi)) return hi;
    return static_cast<std::size_t>(v);
}

Pixel ClampPixel01(Pixel p) {
    p.r = std::clamp(p.r, 0.0, 1.0);
    p.g = std::clamp(p.g, 0.0, 1.0);
    p.b = std::clamp(p.b, 0.0, 1.0);
    return p;
}

class CropFilter final : public Filter {
public:
    CropFilter(int w, int h) : w_(w), h_(h) {
        if (w_ <= 0 || h_ <= 0) {
            throw std::invalid_argument("Crop size must be positive");
        }
    }

    Image Apply(const Image& src) const override {
        const std::size_t new_w = std::min<std::size_t>(src.Width(), static_cast<std::size_t>(w_));
        const std::size_t new_h = std::min<std::size_t>(src.Height(), static_cast<std::size_t>(h_));

        Image dst(new_w, new_h);
        for (std::size_t y = 0; y < new_h; ++y) {
            for (std::size_t x = 0; x < new_w; ++x) {
                dst.At(x, y) = src.At(x, y);
            }
        }
        return dst;
    }

private:
    int w_;
    int h_;
};

class GrayscaleFilter final : public Filter {
public:
    Image Apply(const Image& src) const override {
        Image dst = src;
        for (std::size_t y = 0; y < dst.Height(); ++y) {
            for (std::size_t x = 0; x < dst.Width(); ++x) {
                const Pixel p = dst.At(x, y);
                const double v = 0.299 * p.r + 0.587 * p.g + 0.114 * p.b;
                dst.At(x, y) = ClampPixel01(Pixel{v, v, v});
            }
        }
        return dst;
    }
};

class NegativeFilter final : public Filter {
public:
    Image Apply(const Image& src) const override {
        Image dst = src;
        for (std::size_t y = 0; y < dst.Height(); ++y) {
            for (std::size_t x = 0; x < dst.Width(); ++x) {
                Pixel p = dst.At(x, y);
                p.r = 1.0 - p.r;
                p.g = 1.0 - p.g;
                p.b = 1.0 - p.b;
                dst.At(x, y) = ClampPixel01(p);
            }
        }
        return dst;
    }
};

class ConvolutionFilter final : public Filter {
public:
    explicit ConvolutionFilter(std::vector<std::vector<double>> kernel) : k_(std::move(kernel)) {
        if (k_.empty() || k_[0].empty()) {
            throw std::invalid_argument("Kernel must be non-empty");
        }
        const std::size_t h = k_.size();
        const std::size_t w = k_[0].size();
        for (const auto& row : k_) {
            if (row.size() != w) {
                throw std::invalid_argument("Kernel rows must have same size");
            }
        }
        if (h % 2 == 0 || w % 2 == 0) {
            throw std::invalid_argument("Kernel dimensions must be odd");
        }
    }

    Image Apply(const Image& src) const override {
        Image dst(src.Width(), src.Height());

        const std::size_t kh = k_.size();
        const std::size_t kw = k_[0].size();
        const long long ry = static_cast<long long>(kh / 2);
        const long long rx = static_cast<long long>(kw / 2);

        for (std::size_t y = 0; y < src.Height(); ++y) {
            for (std::size_t x = 0; x < src.Width(); ++x) {
                Pixel acc{0.0, 0.0, 0.0};

                for (long long dy = -ry; dy <= ry; ++dy) {
                    for (long long dx = -rx; dx <= rx; ++dx) {
                        const std::size_t sx = ClampIndex(static_cast<long long>(x) + dx, 0, src.Width() - 1);
                        const std::size_t sy = ClampIndex(static_cast<long long>(y) + dy, 0, src.Height() - 1);

                        const double w = k_[static_cast<std::size_t>(dy + ry)][static_cast<std::size_t>(dx + rx)];
                        const Pixel p = src.At(sx, sy);

                        acc.r += p.r * w;
                        acc.g += p.g * w;
                        acc.b += p.b * w;
                    }
                }

                dst.At(x, y) = ClampPixel01(acc);
            }
        }

        return dst;
    }

private:
    std::vector<std::vector<double>> k_;
};

class SharpenFilter final : public ConvolutionFilter {
public:
    SharpenFilter()
        : ConvolutionFilter({
              {0, -1, 0},
              {-1, 5, -1},
              {0, -1, 0},
          }) {}
};

class EdgeFilter final : public Filter {
public:
    explicit EdgeFilter(double threshold) : t_(threshold) {}

    Image Apply(const Image& src) const override {
        GrayscaleFilter gs;
        Image gray = gs.Apply(src);

        ConvolutionFilter conv({
            {0, -1, 0},
            {-1, 4, -1},
            {0, -1, 0},
        });

        Image edged = conv.Apply(gray);

        Image dst(edged.Width(), edged.Height());
        for (std::size_t y = 0; y < dst.Height(); ++y) {
            for (std::size_t x = 0; x < dst.Width(); ++x) {
                const double v = edged.At(x, y).r;  // gray => r=g=b
                dst.At(x, y) = (v > t_) ? Pixel{1.0, 1.0, 1.0} : Pixel{0.0, 0.0, 0.0};
            }
        }
        return dst;
    }

private:
    double t_;
};

class GaussianBlurFilter final : public Filter {
public:
    explicit GaussianBlurFilter(double sigma) : sigma_(sigma) {
        if (!(sigma_ > 0.0)) {
            throw std::invalid_argument("Sigma must be > 0");
        }
        BuildKernel();
    }

    Image Apply(const Image& src) const override {
        Image tmp(src.Width(), src.Height());
        Image dst(src.Width(), src.Height());

        const long long r = static_cast<long long>(kernel_.size() / 2);

        // horizontal
        for (std::size_t y = 0; y < src.Height(); ++y) {
            for (std::size_t x = 0; x < src.Width(); ++x) {
                Pixel acc{0.0, 0.0, 0.0};
                for (long long dx = -r; dx <= r; ++dx) {
                    const std::size_t sx = ClampIndex(static_cast<long long>(x) + dx, 0, src.Width() - 1);
                    const double w = kernel_[static_cast<std::size_t>(dx + r)];
                    const Pixel p = src.At(sx, y);
                    acc.r += p.r * w;
                    acc.g += p.g * w;
                    acc.b += p.b * w;
                }
                tmp.At(x, y) = ClampPixel01(acc);
            }
        }

        // vertical
        for (std::size_t y = 0; y < src.Height(); ++y) {
            for (std::size_t x = 0; x < src.Width(); ++x) {
                Pixel acc{0.0, 0.0, 0.0};
                for (long long dy = -r; dy <= r; ++dy) {
                    const std::size_t sy = ClampIndex(static_cast<long long>(y) + dy, 0, src.Height() - 1);
                    const double w = kernel_[static_cast<std::size_t>(dy + r)];
                    const Pixel p = tmp.At(x, sy);
                    acc.r += p.r * w;
                    acc.g += p.g * w;
                    acc.b += p.b * w;
                }
                dst.At(x, y) = ClampPixel01(acc);
            }
        }

        return dst;
    }

private:
    void BuildKernel() {
        const int radius = std::max(1, static_cast<int>(std::ceil(3.0 * sigma_)));
        const int size = 2 * radius + 1;

        kernel_.assign(static_cast<std::size_t>(size), 0.0);

        const double denom = 2.0 * sigma_ * sigma_;
        double sum = 0.0;

        for (int i = -radius; i <= radius; ++i) {
            const double v = std::exp(-(static_cast<double>(i) * static_cast<double>(i)) / denom);
            kernel_[static_cast<std::size_t>(i + radius)] = v;
            sum += v;
        }

        for (double& v : kernel_) {
            v /= sum;
        }
    }

    double sigma_;
    std::vector<double> kernel_;
};

class MedianFilter final : public Filter {
public:
    explicit MedianFilter(int window) : window_(window) {
        if (window_ <= 0 || window_ % 2 == 0) {
            throw std::invalid_argument("Median window must be positive odd");
        }
    }

    Image Apply(const Image& src) const override {
        Image dst(src.Width(), src.Height());
        const int r = window_ / 2;

        std::vector<double> buf;
        buf.reserve(static_cast<std::size_t>(window_ * window_));

        for (std::size_t y = 0; y < src.Height(); ++y) {
            for (std::size_t x = 0; x < src.Width(); ++x) {
                Pixel out{0.0, 0.0, 0.0};

                out.r = MedianChannel(src, x, y, r, 0, buf);
                out.g = MedianChannel(src, x, y, r, 1, buf);
                out.b = MedianChannel(src, x, y, r, 2, buf);

                dst.At(x, y) = ClampPixel01(out);
            }
        }

        return dst;
    }

private:
    static double MedianChannel(const Image& src, std::size_t x, std::size_t y, int r, int ch, std::vector<double>& buf) {
        buf.clear();

        for (int dy = -r; dy <= r; ++dy) {
            for (int dx = -r; dx <= r; ++dx) {
                const std::size_t sx = ClampIndex(static_cast<long long>(x) + dx, 0, src.Width() - 1);
                const std::size_t sy = ClampIndex(static_cast<long long>(y) + dy, 0, src.Height() - 1);

                const Pixel p = src.At(sx, sy);
                if (ch == 0) buf.push_back(p.r);
                if (ch == 1) buf.push_back(p.g);
                if (ch == 2) buf.push_back(p.b);
            }
        }

        const std::size_t mid = buf.size() / 2;
        std::nth_element(buf.begin(), buf.begin() + static_cast<long long>(mid), buf.end());
        return buf[mid];
    }

    int window_;
};

} // namespace

std::vector<std::unique_ptr<Filter>> CreateFilters(int argc, char** argv, int start_index) {
    if (start_index < 0 || start_index > argc) {
        throw std::invalid_argument("Invalid start_index");
    }

    std::vector<std::unique_ptr<Filter>> filters;

    int i = start_index;
    while (i < argc) {
        const std::string op = argv[i];

        if (op == "-crop") {
            if (i + 2 >= argc) throw std::invalid_argument("Usage: -crop width height");
            const int w = ToInt(argv[i + 1]);
            const int h = ToInt(argv[i + 2]);
            filters.push_back(std::make_unique<CropFilter>(w, h));
            i += 3;
            continue;
        }

        if (op == "-gs") {
            filters.push_back(std::make_unique<GrayscaleFilter>());
            i += 1;
            continue;
        }

        if (op == "-neg") {
            filters.push_back(std::make_unique<NegativeFilter>());
            i += 1;
            continue;
        }

        if (op == "-sharp") {
            filters.push_back(std::make_unique<SharpenFilter>());
            i += 1;
            continue;
        }

        if (op == "-edge") {
            if (i + 1 >= argc) throw std::invalid_argument("Usage: -edge threshold");
            const double t = ToDouble(argv[i + 1]);
            filters.push_back(std::make_unique<EdgeFilter>(t));
            i += 2;
            continue;
        }

        if (op == "-blur") {
            if (i + 1 >= argc) throw std::invalid_argument("Usage: -blur sigma");
            const double sigma = ToDouble(argv[i + 1]);
            filters.push_back(std::make_unique<GaussianBlurFilter>(sigma));
            i += 2;
            continue;
        }

        if (op == "-med") {
            if (i + 1 >= argc) throw std::invalid_argument("Usage: -med window");
            const int w = ToInt(argv[i + 1]);
            filters.push_back(std::make_unique<MedianFilter>(w));
            i += 2;
            continue;
        }

        throw std::invalid_argument("Unknown filter: " + op);
    }

    return filters;
}