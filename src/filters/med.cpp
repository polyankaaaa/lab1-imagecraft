#include "filters/med.h"

#include "utils.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

class MedianFilter final : public Filter {
public:
    explicit MedianFilter(int radius) : r_(radius) {
        if (r_ < 0) throw std::invalid_argument("radius must be >= 0");
    }

    void Apply(Image& image) const override {
        const int w = image.GetWidth();
        const int h = image.GetHeight();
        Image out(w, h);

        std::vector<int> vr;
        std::vector<int> vg;
        std::vector<int> vb;

        vr.reserve(static_cast<size_t>((2 * r_ + 1) * (2 * r_ + 1)));
        vg.reserve(vr.capacity());
        vb.reserve(vr.capacity());

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                vr.clear(); vg.clear(); vb.clear();
                for (int dy = -r_; dy <= r_; ++dy) {
                    for (int dx = -r_; dx <= r_; ++dx) {
                        const Pixel p = GetClamped(image, x + dx, y + dy);
                        vr.push_back(p.r);
                        vg.push_back(p.g);
                        vb.push_back(p.b);
                    }
                }
                auto median = [](std::vector<int>& v) -> uint8_t {
                    const size_t mid = v.size() / 2;
                    std::nth_element(v.begin(), v.begin() + static_cast<std::ptrdiff_t>(mid), v.end());
                    return ClampU8(v[mid]);
                };
                out.SetPixel(x, y, Pixel{median(vr), median(vg), median(vb)});
            }
        }

        image = std::move(out);
    }

private:
    int r_;
};

std::unique_ptr<Filter> MakeMedian(int radius) {
    return std::make_unique<MedianFilter>(radius);
}