#include "filters/edge.h"

#include "utils.h"

#include <cmath>
#include <stdexcept>

class EdgeFilter final : public Filter {
public:
    explicit EdgeFilter(double threshold01) : t_(threshold01) {
        if (t_ < 0.0 || t_ > 1.0) throw std::invalid_argument("edge threshold must be in [0..1]");
    }

    void Apply(Image& image) const override {
        const int w = image.GetWidth();
        const int h = image.GetHeight();

        Image gray(w, h);
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                const Pixel p = image.GetPixel(x, y);
                const int g = static_cast<int>(std::lround(0.299 * p.r + 0.587 * p.g + 0.114 * p.b));
                const uint8_t gg = ClampU8(g);
                gray.SetPixel(x, y, Pixel{gg, gg, gg});
            }
        }

        Image out(w, h);

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                const Pixel c = GetClamped(gray, x, y);
                const Pixel l = GetClamped(gray, x - 1, y);
                const Pixel r = GetClamped(gray, x + 1, y);
                const Pixel u = GetClamped(gray, x, y - 1);
                const Pixel d = GetClamped(gray, x, y + 1);

                const int v = 4 * c.r - l.r - r.r - u.r - d.r;
                const double v01 = static_cast<double>(ClampInt(v, 0, 255)) / 255.0;
                const uint8_t outv = (v01 > t_) ? 255 : 0;
                out.SetPixel(x, y, Pixel{outv, outv, outv});
            }
        }

        image = std::move(out);
    }

private:
    double t_;
};

std::unique_ptr<Filter> MakeEdge(double threshold01) {
    return std::make_unique<EdgeFilter>(threshold01);
}