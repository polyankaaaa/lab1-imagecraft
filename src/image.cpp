#include "image.h"

#include <stdexcept>

Image::Image(int width, int height)
    : width_(width)
    , height_(height)
    , data_(static_cast<size_t>(width) * static_cast<size_t>(height)) {
    if (width < 0 || height < 0) {
        throw std::invalid_argument("negative image size");
    }
}

int Image::GetWidth() const { return width_; }
int Image::GetHeight() const { return height_; }

Pixel Image::GetPixel(int x, int y) const {
    return data_.at(static_cast<size_t>(y) * static_cast<size_t>(width_) + static_cast<size_t>(x));
}

void Image::SetPixel(int x, int y, Pixel p) {
    data_.at(static_cast<size_t>(y) * static_cast<size_t>(width_) + static_cast<size_t>(x)) = p;
}

std::vector<Pixel>& Image::Data() { return data_; }
const std::vector<Pixel>& Image::Data() const { return data_; }