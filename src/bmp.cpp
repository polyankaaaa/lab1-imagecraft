#include "bmp.h"
#include <fstream>
#include <stdexcept>
#include <cstdint>

#pragma pack(push, 1)
struct BmpFileHeader {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
};

struct BmpInfoHeader {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
#pragma pack(pop)

static void Require(bool ok, const char* msg) {
    if (!ok) throw std::runtime_error(msg);
}

Image ReadBMP(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    Require((bool)in, "Cannot open BMP for reading");

    BmpFileHeader fh{};
    BmpInfoHeader ih{};

    in.read(reinterpret_cast<char*>(&fh), sizeof(fh));
    in.read(reinterpret_cast<char*>(&ih), sizeof(ih));
    Require((bool)in, "Failed to read BMP headers");

    Require(fh.bfType == 0x4D42, "Not a BMP file (bad signature)");
    Require(ih.biPlanes == 1, "Unsupported BMP: biPlanes != 1");
    Require(ih.biBitCount == 24, "Unsupported BMP: only 24-bit supported");
    Require(ih.biCompression == 0, "Unsupported BMP: compression not supported");
    Require(ih.biWidth > 0, "Unsupported BMP: width <= 0");
    Require(ih.biHeight != 0, "Unsupported BMP: height == 0");

    const int width = ih.biWidth;
    const bool topDown = (ih.biHeight < 0);
    const int height = topDown ? -ih.biHeight : ih.biHeight;

    Image img(width, height);

    const int rowBytes = width * 3;
    const int padding = (4 - (rowBytes % 4)) % 4;

    in.seekg(static_cast<std::streamoff>(fh.bfOffBits), std::ios::beg);
    Require((bool)in, "Failed to seek to pixel data");

    for (int row = 0; row < height; ++row) {
        int y = topDown ? row : (height - 1 - row);

        for (int x = 0; x < width; ++x) {
            uint8_t bgr[3];
            in.read(reinterpret_cast<char*>(bgr), 3);
            Require((bool)in, "Failed to read BMP pixel data");
            Pixel p;
            p.b = bgr[0];
            p.g = bgr[1];
            p.r = bgr[2];
            img.At(x, y) = p;
        }
        if (padding) {
            in.ignore(padding);
            Require((bool)in, "Failed to skip BMP row padding");
        }
    }
    return img;
}

void WriteBMP(const std::string& path, const Image& img) {
    std::ofstream out(path, std::ios::binary);
    Require((bool)out, "Cannot open BMP for writing");

    const int width = img.Width();
    const int height = img.Height();

    const int rowBytes = width * 3;
    const int padding = (4 - (rowBytes % 4)) % 4;
    const uint32_t pixelDataSize = static_cast<uint32_t>((rowBytes + padding) * height);

    BmpFileHeader fh{};
    fh.bfType = 0x4D42;
    fh.bfOffBits = sizeof(BmpFileHeader) + sizeof(BmpInfoHeader);
    fh.bfSize = fh.bfOffBits + pixelDataSize;

    BmpInfoHeader ih{};
    ih.biSize = sizeof(BmpInfoHeader);
    ih.biWidth = width;
    ih.biHeight = height; // bottom-up
    ih.biPlanes = 1;
    ih.biBitCount = 24;
    ih.biCompression = 0;
    ih.biSizeImage = pixelDataSize;

    out.write(reinterpret_cast<const char*>(&fh), sizeof(fh));
    out.write(reinterpret_cast<const char*>(&ih), sizeof(ih));

    const uint8_t pad[3] = {0,0,0};

    for (int row = 0; row < height; ++row) {
        int y = height - 1 - row; // bottom-up
        for (int x = 0; x < width; ++x) {
            const Pixel& p = img.At(x, y);
            uint8_t bgr[3] = { p.b, p.g, p.r };
            out.write(reinterpret_cast<const char*>(bgr), 3);
        }
        if (padding) out.write(reinterpret_cast<const char*>(pad), padding);
    }

    Require((bool)out, "Failed to write BMP");
}