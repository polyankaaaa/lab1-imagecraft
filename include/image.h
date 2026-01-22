#pragma once

#include <cstdint>
#include <vector>

struct Pixel {
    uint8_t r{};
    uint8_t g{};
    uint8_t b{};
};

class Image {
public:
    Image() = default;
    Image(int width, int height);

    int GetWidth() const;
    int GetHeight() const;

    Pixel GetPixel(int x, int y) const;
    void SetPixel(int x, int y, Pixel p);

    std::vector<Pixel>& Data();
    const std::vector<Pixel>& Data() const;

private:
    int width_ = 0;
    int height_ = 0;
    std::vector<Pixel> data_;
};