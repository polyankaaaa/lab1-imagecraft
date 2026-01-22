#include "filters/gs.h"

#include "utils.h"

#include <cmath>

class GrayscaleFilter final : public Filter {
public:
    void Apply(Image& image) const override {
        const int w = image.GetWidth();
        const int h = image.GetHeight();
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                Pixel p = image.GetPixel(x, y);
                const int g = static_cast<int>(std::lround(0.299 * p.r + 0.587 * p.g + 0.114 * p.b));
                const uint8_t gg = ClampU8(g);
                image.SetPixel(x, y, Pixel{gg, gg, gg});
            }
        }
    }
};

std::unique_ptr<Filter> MakeGrayscale() {
    return std::make_unique<GrayscaleFilter>();
}