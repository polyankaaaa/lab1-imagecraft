#include "bmp.h"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <vector>

#pragma pack(push, 1)
struct BmpFileHeader {
    uint16_t bfType = 0;
    uint32_t bfSize = 0;
    uint16_t bfReserved1 = 0;
    uint16_t bfReserved2 = 0;
    uint32_t bfOffBits = 0;
};

struct BmpInfoHeader {
    uint32_t biSize = 0;
    int32_t biWidth = 0;
    int32_t biHeight = 0;
    uint16_t biPlanes = 0;
    uint16_t biBitCount = 0;
    uint32_t biCompression = 0;
    uint32_t biSizeImage = 0;
    int32_t biXPelsPerMeter = 0;
    int32_t biYPelsPerMeter = 0;
    uint32_t biClrUsed = 0;
    uint32_t biClrImportant = 0;
};
#pragma pack(pop)

static uint8_t ToByte(float v) {
    if (v < 0.0f) v = 0.0f;
    if (v > 1.0f) v = 1.0f;
    return static_cast<uint8_t>(v * 255.0f + 0.5f);
}

static float ToFloat(uint8_t v) {
    return static_cast<float>(v) / 255.0f;
}

Image ReadBmp(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        throw std::runtime_error("Cannot open input file: " + path);
    }

    BmpFileHeader fh{};
    BmpInfoHeader ih{};

    in.read(reinterpret_cast<char*>(&fh), sizeof(fh));
    in.read(reinterpret_cast<char*>(&ih), sizeof(ih));
    if (!in) {
        throw std::runtime_error("Bad BMP headers: " + path);
    }

    if (fh.bfType != 0x4D42) {
        throw std::runtime_error("Not a BMP file: " + path);
    }
    if (ih.biSize != 40) {
        throw std::runtime_error("Unsupported BMP info header (need BITMAPINFOHEADER=40): " + path);
    }
    if (ih.biPlanes != 1) {
        throw std::runtime_error("Unsupported BMP planes: " + path);
    }
    if (ih.biBitCount != 24) {
        throw std::runtime_error("Unsupported BMP bit depth (need 24-bit): " + path);
    }
    if (ih.biCompression != 0) {
        throw std::runtime_error("Unsupported BMP compression (need BI_RGB): " + path);
    }
    if (ih.biWidth <= 0 || ih.biHeight == 0) {
        throw std::runtime_error("Unsupported BMP dimensions: " + path);
    }

    const int width = ih.biWidth;
    const bool topDown = (ih.biHeight < 0);
    const int height = topDown ? -ih.biHeight : ih.biHeight;

    Image image(static_cast<std::size_t>(width), static_cast<std::size_t>(height));

    const std::size_t rowBytes = static_cast<std::size_t>(width) * 3;
    const std::size_t rowStride = (rowBytes + 3) / 4 * 4;
    const std::size_t padding = rowStride - rowBytes;

    in.seekg(static_cast<std::streamoff>(fh.bfOffBits), std::ios::beg);
    if (!in) {
        throw std::runtime_error("Bad BMP pixel offset: " + path);
    }

    for (int y = 0; y < height; ++y) {
        const int dstY = topDown ? y : (height - 1 - y);

        for (int x = 0; x < width; ++x) {
            uint8_t b = 0, g = 0, r = 0;
            in.read(reinterpret_cast<char*>(&b), 1);
            in.read(reinterpret_cast<char*>(&g), 1);
            in.read(reinterpret_cast<char*>(&r), 1);
            if (!in) {
                throw std::runtime_error("Unexpected EOF while reading pixels: " + path);
            }

            Pixel& p = image.At(static_cast<std::size_t>(x), static_cast<std::size_t>(dstY));
            p.r = ToFloat(r);
            p.g = ToFloat(g);
            p.b = ToFloat(b);
        }

        in.ignore(static_cast<std::streamsize>(padding));
        if (!in) {
            throw std::runtime_error("Unexpected EOF while reading row padding: " + path);
        }
    }

    return image;
}

void WriteBmp(const std::string& path, const Image& image) {
    const std::size_t width = image.Width();
    const std::size_t height = image.Height();

    if (width == 0 || height == 0) {
        throw std::runtime_error("Cannot write empty image: " + path);
    }

    const std::size_t rowBytes = width * 3;
    const std::size_t rowStride = (rowBytes + 3) / 4 * 4;
    const std::size_t padding = rowStride - rowBytes;

    BmpFileHeader fh{};
    BmpInfoHeader ih{};

    fh.bfType = 0x4D42;
    fh.bfOffBits = sizeof(BmpFileHeader) + sizeof(BmpInfoHeader);
    fh.bfSize = static_cast<uint32_t>(fh.bfOffBits + rowStride * height);

    ih.biSize = 40;
    ih.biWidth = static_cast<int32_t>(width);
    ih.biHeight = static_cast<int32_t>(height);
    ih.biPlanes = 1;
    ih.biBitCount = 24;
    ih.biCompression = 0;
    ih.biSizeImage = static_cast<uint32_t>(rowStride * height);

    std::ofstream out(path, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Cannot open output file: " + path);
    }

    out.write(reinterpret_cast<const char*>(&fh), sizeof(fh));
    out.write(reinterpret_cast<const char*>(&ih), sizeof(ih));

    const uint8_t pad[3] = {0, 0, 0};

    for (std::size_t y = 0; y < height; ++y) {
        const std::size_t srcY = height - 1 - y;

        for (std::size_t x = 0; x < width; ++x) {
            const Pixel& p = image.At(x, srcY);
            const uint8_t r = ToByte(p.r);
            const uint8_t g = ToByte(p.g);
            const uint8_t b = ToByte(p.b);

            out.write(reinterpret_cast<const char*>(&b), 1);
            out.write(reinterpret_cast<const char*>(&g), 1);
            out.write(reinterpret_cast<const char*>(&r), 1);
        }

        out.write(reinterpret_cast<const char*>(pad), static_cast<std::streamsize>(padding));
    }

    if (!out) {
        throw std::runtime_error("Failed while writing BMP: " + path);
    }
}