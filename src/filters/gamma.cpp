#include "filters/gamma.h"

#include <cmath>
#include <stdexcept>

class GammaFilter final : public Filter {
public:
    explicit GammaFilter(double gamma) : g_(gamma) {
        if (g_ <= 0.0) throw std::invalid_argument("gamma must be > 0");
    }

    void Apply(Image& image) const override {
        const int w = image.GetWidth();
        const int h = image.GetHeight();
        const double inv = 1.0 / g_;

        auto corr = [&](uint8_t c) -> uint8_t {
            const double x = static_cast<double>(c) / 255.0;
            const double y = std::pow(x, inv);
            int v = static_cast<int>(std::lround(y * 255.0));
            if (v < 0) v = 0;
            if (v > 255) v = 255;
            return static_cast<uint8_t>(v);
        };

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                Pixel p = image.GetPixel(x, y);
                p.r = corr(p.r);
                p.g = corr(p.g);
                p.b = corr(p.b);
                image.SetPixel(x, y, p);
            }
        }
    }

private:
    double g_;
};

std::unique_ptr<Filter> MakeGamma(double gamma) {
    return std::make_unique<GammaFilter>(gamma);
}