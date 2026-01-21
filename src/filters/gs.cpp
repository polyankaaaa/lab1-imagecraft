#include "filter.h"

class GrayscaleFilter : public Filter {
public:
    Image Apply(const Image& src) const override {
        Image dst = src;

        for (std::size_t y = 0; y < src.Height(); ++y) {
            for (std::size_t x = 0; x < src.Width(); ++x) {
                const Pixel& p = src.At(x, y);
                const double v = 0.299 * p.r + 0.587 * p.g + 0.114 * p.b;
                dst.At(x, y) = Pixel{v, v, v};
            }
        }

        return dst;
    }
};