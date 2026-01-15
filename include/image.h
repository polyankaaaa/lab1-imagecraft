#pragma once
#include <cstdint>
#include <vector>
#include <stdexcept>

struct Pixel {
    uint8_t r = 0, g = 0, b = 0;
};

class Image {
public:
    Image() = default;
    Image(int w, int h) { Resize(w, h); }

    void Resize(int w, int h) {
        if (w <= 0 || h <= 0) throw std::runtime_error("Image size must be positive");
        width_ = w;
        height_ = h;
        data_.assign(static_cast<size_t>(w) * static_cast<size_t>(h), Pixel{});
    }

    int Width() const { return width_; }
    int Height() const { return height_; }

    Pixel& At(int x, int y) {
        if (x < 0 || y < 0 || x >= width_ || y >= height_) throw std::out_of_range("Pixel index out of range");
        return data_[static_cast<size_t>(y) * static_cast<size_t>(width_) + static_cast<size_t>(x)];
    }
    const Pixel& At(int x, int y) const {
        if (x < 0 || y < 0 || x >= width_ || y >= height_) throw std::out_of_range("Pixel index out of range");
        return data_[static_cast<size_t>(y) * static_cast<size_t>(width_) + static_cast<size_t>(x)];
    }

private:
    int width_ = 0;
    int height_ = 0;
    std::vector<Pixel> data_;
};