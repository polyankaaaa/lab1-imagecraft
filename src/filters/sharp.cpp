#include "filters/sharp.h"

#include "utils.h"

class SharpenFilter final : public Filter {
public:
    void Apply(Image& image) const override {
        const int w = image.GetWidth();
        const int h = image.GetHeight();
        Image out(w, h);

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                const Pixel c = GetClamped(image, x, y);
                const Pixel l = GetClamped(image, x - 1, y);
                const Pixel r = GetClamped(image, x + 1, y);
                const Pixel u = GetClamped(image, x, y - 1);
                const Pixel d = GetClamped(image, x, y + 1);

                const int rr = 5 * c.r - l.r - r.r - u.r - d.r;
                const int gg = 5 * c.g - l.g - r.g - u.g - d.g;
                const int bb = 5 * c.b - l.b - r.b - u.b - d.b;

                out.SetPixel(x, y, Pixel{ClampU8(rr), ClampU8(gg), ClampU8(bb)});
            }
        }

        image = std::move(out);
    }
};

std::unique_ptr<Filter> MakeSharpen() {
    return std::make_unique<SharpenFilter>();
}