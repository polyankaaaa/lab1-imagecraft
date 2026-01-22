#include "filters/hist_eq.h"

#include "utils.h"

#include <array>
#include <cmath>
#include <vector>

class HistEqFilter final : public Filter {
public:
    void Apply(Image& image) const override {
        const int w = image.GetWidth();
        const int h = image.GetHeight();
        const size_t n = static_cast<size_t>(w) * static_cast<size_t>(h);

        std::array<uint32_t, 256> hist{};
        hist.fill(0);

        std::vector<uint8_t> lum;
        lum.reserve(n);

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                const Pixel p = image.GetPixel(x, y);
                const int g = static_cast<int>(std::lround(0.299 * p.r + 0.587 * p.g + 0.114 * p.b));
                const uint8_t gg = ClampU8(g);
                lum.push_back(gg);
                hist[gg] += 1;
            }
        }

        std::array<uint32_t, 256> cdf{};
        uint32_t running = 0;
        for (int i = 0; i < 256; ++i) {
            running += hist[static_cast<size_t>(i)];
            cdf[static_cast<size_t>(i)] = running;
        }

        uint32_t cdf_min = 0;
        for (int i = 0; i < 256; ++i) {
            if (hist[static_cast<size_t>(i)] != 0) {
                cdf_min = cdf[static_cast<size_t>(i)];
                break;
            }
        }

        Image out(w, h);
        size_t idx = 0;
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x, ++idx) {
                const uint8_t v = lum[idx];
                const double mapped = (static_cast<double>(cdf[v] - cdf_min) / static_cast<double>(n - cdf_min)) * 255.0;
                const uint8_t m = ClampU8(static_cast<int>(std::lround(mapped)));
                out.SetPixel(x, y, Pixel{m, m, m});
            }
        }

        image = std::move(out);
    }
};

std::unique_ptr<Filter> MakeHistEq() {
    return std::make_unique<HistEqFilter>();
}