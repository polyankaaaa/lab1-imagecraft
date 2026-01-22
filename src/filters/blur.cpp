#include "filters/blur.h"

#include "utils.h"

#include <stdexcept>
#include <vector>

class BlurFilter final : public Filter {
public:
    explicit BlurFilter(double sigma) : sigma_(sigma) {
        if (sigma_ < 0.0) throw std::invalid_argument("sigma must be >= 0");
    }

    void Apply(Image& image) const override {
        const int w = image.GetWidth();
        const int h = image.GetHeight();
        const std::vector<double> k = GaussianKernel1D(sigma_);
        const int radius = static_cast<int>((k.size() - 1) / 2);

        Image tmp(w, h);
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                double rr = 0.0, gg = 0.0, bb = 0.0;
                for (int i = -radius; i <= radius; ++i) {
                    const Pixel p = GetClamped(image, x + i, y);
                    const double wgt = k[static_cast<size_t>(i + radius)];
                    rr += wgt * p.r;
                    gg += wgt * p.g;
                    bb += wgt * p.b;
                }
                tmp.SetPixel(x, y, Pixel{ClampU8(static_cast<int>(std::lround(rr))),
                                         ClampU8(static_cast<int>(std::lround(gg))),
                                         ClampU8(static_cast<int>(std::lround(bb)))});
            }
        }

        Image out(w, h);
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                double rr = 0.0, gg = 0.0, bb = 0.0;
                for (int i = -radius; i <= radius; ++i) {
                    const Pixel p = GetClamped(tmp, x, y + i);
                    const double wgt = k[static_cast<size_t>(i + radius)];
                    rr += wgt * p.r;
                    gg += wgt * p.g;
                    bb += wgt * p.b;
                }
                out.SetPixel(x, y, Pixel{ClampU8(static_cast<int>(std::lround(rr))),
                                         ClampU8(static_cast<int>(std::lround(gg))),
                                         ClampU8(static_cast<int>(std::lround(bb)))});
            }
        }

        image = std::move(out);
    }

private:
    double sigma_;
};

std::unique_ptr<Filter> MakeBlur(double sigma) {
    return std::make_unique<BlurFilter>(sigma);
}