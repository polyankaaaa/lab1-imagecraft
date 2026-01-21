#include "filter.h"

class NegativeFilter : public Filter {
public:
    Image Apply(const Image& src) const override {
        Image dst = src;

        for (std::size_t y = 0; y < src.Height(); ++y) {
            for (std::size_t x = 0; x < src.Width(); ++x) {
                const Pixel& p = src.At(x, y);
                dst.At(x, y) = Pixel{1.0 - p.r, 1.0 - p.g, 1.0 - p.b};
            }
        }

        return dst;
    }
};