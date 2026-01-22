#include "filter.h"
#include "image.h"

#include <algorithm>

class SharpenFilter final : public Filter {
public:
    void Apply(Image& image) const override {
        const int w = image.GetWidth();
        const int h = image.GetHeight();

        Image out(w, h);

        auto clampi = [](int v, int lo, int hi) {
            return std::max(lo, std::min(v, hi));
        };

        auto get = [&](int x, int y) -> Pixel {
            x = clampi(x, 0, w - 1);
            y = clampi(y, 0, h - 1);
            return image.GetPixel(x, y);
        };

        
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                Pixel c  = get(x, y);
                Pixel l  = get(x - 1, y);
                Pixel r  = get(x + 1, y);
                Pixel u  = get(x, y - 1);
                Pixel d  = get(x, y + 1);

                auto sat = [](int v) -> uint8_t {
                    if (v < 0) return 0;
                    if (v > 255) return 255;
                    return static_cast<uint8_t>(v);
                };

                const int rr = 5 * c.r - l.r - r.r - u.r - d.r;
                const int gg = 5 * c.g - l.g - r.g - u.g - d.g;
                const int bb = 5 * c.b - l.b - r.b - u.b - d.b;

                out.SetPixel(x, y, Pixel{sat(rr), sat(gg), sat(bb)});
            }
        }

        image = std::move(out);
    }
};