#include "filter.h"
#include "image.h"

#include <algorithm>
#include <utility>

CropFilter::CropFilter(int width, int height)
    : width_(width), height_(height) {
}

void CropFilter::Apply(Image& image) const {
    const int old_w = image.GetWidth();
    const int old_h = image.GetHeight();

    const int new_w = std::min(width_, old_w);
    const int new_h = std::min(height_, old_h);

    Image out(new_w, new_h);

    for (int y = 0; y < new_h; ++y) {
        for (int x = 0; x < new_w; ++x) {
            out.SetPixel(x, y, image.GetPixel(x, y));
        }
    }

    image = std::move(out);
}