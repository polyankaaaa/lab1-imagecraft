#include "filters/neg.h"

class NegativeFilter final : public Filter {
public:
    void Apply(Image& image) const override {
        const int w = image.GetWidth();
        const int h = image.GetHeight();
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                Pixel p = image.GetPixel(x, y);
                p.r = static_cast<uint8_t>(255 - p.r);
                p.g = static_cast<uint8_t>(255 - p.g);
                p.b = static_cast<uint8_t>(255 - p.b);
                image.SetPixel(x, y, p);
            }
        }
    }
};

std::unique_ptr<Filter> MakeNegative() {
    return std::make_unique<NegativeFilter>();
}