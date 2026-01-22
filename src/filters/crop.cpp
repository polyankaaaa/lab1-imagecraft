#include "filters/crop.h"

#include <stdexcept>

class CropFilter final : public Filter {
public:
    CropFilter(int width, int height) : new_w_(width), new_h_(height) {
        if (new_w_ <= 0 || new_h_ <= 0) throw std::invalid_argument("invalid crop size");
    }

    void Apply(Image& image) const override {
        const int w = image.GetWidth();
        const int h = image.GetHeight();
        const int cw = (new_w_ < w) ? new_w_ : w;
        const int ch = (new_h_ < h) ? new_h_ : h;

        Image out(cw, ch);
        for (int y = 0; y < ch; ++y) {
            for (int x = 0; x < cw; ++x) {
                out.SetPixel(x, y, image.GetPixel(x, y));
            }
        }
        image = std::move(out);
    }

private:
    int new_w_;
    int new_h_;
};

std::unique_ptr<Filter> MakeCrop(int width, int height) {
    return std::make_unique<CropFilter>(width, height);
}