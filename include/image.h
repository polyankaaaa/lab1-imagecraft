#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>

struct Pixel {
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;
};

class Image {
public:
    Image() = default;

    Image(std::size_t width, std::size_t height)
        : width_(width), height_(height), data_(width * height) {}

    std::size_t Width() const { return width_; }
    std::size_t Height() const { return height_; }

    Pixel& At(std::size_t x, std::size_t y) {
        if (x >= width_ || y >= height_) {
            throw std::out_of_range("Image::At out of range");
        }
        return data_[y * width_ + x];
    }

    const Pixel& At(std::size_t x, std::size_t y) const {
        if (x >= width_ || y >= height_) {
            throw std::out_of_range("Image::At out of range");
        }
        return data_[y * width_ + x];
    }

private:
    std::size_t width_ = 0;
    std::size_t height_ = 0;
    std::vector<Pixel> data_;
};