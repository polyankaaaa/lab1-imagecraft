#include "bmp.h"

#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <vector>

static uint16_t ReadU16(std::istream& in) {
    uint8_t b0{}, b1{};
    in.read(reinterpret_cast<char*>(&b0), 1);
    in.read(reinterpret_cast<char*>(&b1), 1);
    return static_cast<uint16_t>(b0 | (static_cast<uint16_t>(b1) << 8));
}

static uint32_t ReadU32(std::istream& in) {
    uint8_t b[4]{};
    in.read(reinterpret_cast<char*>(b), 4);
    return static_cast<uint32_t>(b[0]) |
           (static_cast<uint32_t>(b[1]) << 8) |
           (static_cast<uint32_t>(b[2]) << 16) |
           (static_cast<uint32_t>(b[3]) << 24);
}

static int32_t ReadI32(std::istream& in) {
    return static_cast<int32_t>(ReadU32(in));
}

static void WriteU16(std::ostream& out, uint16_t v) {
    uint8_t b[2]{static_cast<uint8_t>(v & 0xFF), static_cast<uint8_t>((v >> 8) & 0xFF)};
    out.write(reinterpret_cast<const char*>(b), 2);
}

static void WriteU32(std::ostream& out, uint32_t v) {
    uint8_t b[4]{
        static_cast<uint8_t>(v & 0xFF),
        static_cast<uint8_t>((v >> 8) & 0xFF),
        static_cast<uint8_t>((v >> 16) & 0xFF),
        static_cast<uint8_t>((v >> 24) & 0xFF)
    };
    out.write(reinterpret_cast<const char*>(b), 4);
}

static void WriteI32(std::ostream& out, int32_t v) {
    WriteU32(out, static_cast<uint32_t>(v));
}

Image ReadBmp(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) throw std::runtime_error("cannot open input file");

    char sig[2]{};
    in.read(sig, 2);
    if (sig[0] != 'B' || sig[1] != 'M') throw std::runtime_error("not a BMP file");

    ReadU32(in);
    ReadU16(in);
    ReadU16(in);
    const uint32_t data_offset = ReadU32(in);

    const uint32_t dib_size = ReadU32(in);
    if (dib_size < 40) throw std::runtime_error("unsupported BMP DIB header");

    const int32_t width = ReadI32(in);
    const int32_t height_raw = ReadI32(in);
    const uint16_t planes = ReadU16(in);
    const uint16_t bpp = ReadU16(in);
    const uint32_t compression = ReadU32(in);
    ReadU32(in);
    ReadI32(in);
    ReadI32(in);
    ReadU32(in);
    ReadU32(in);

    if (planes != 1) throw std::runtime_error("unsupported BMP planes");
    if (bpp != 24) throw std::runtime_error("only 24-bit BMP is supported");
    if (compression != 0) throw std::runtime_error("compressed BMP is not supported");
    if (width <= 0 || height_raw == 0) throw std::runtime_error("invalid BMP size");

    const bool top_down = (height_raw < 0);
    const int height = top_down ? -height_raw : height_raw;

    in.seekg(static_cast<std::streamoff>(data_offset), std::ios::beg);
    if (!in) throw std::runtime_error("invalid BMP offset");

    Image img(width, height);

    const int row_bytes = width * 3;
    const int padding = (4 - (row_bytes % 4)) % 4;
    std::vector<uint8_t> row(static_cast<size_t>(row_bytes));

    for (int y = 0; y < height; ++y) {
        const int dst_y = top_down ? y : (height - 1 - y);

        in.read(reinterpret_cast<char*>(row.data()), row_bytes);
        if (!in) throw std::runtime_error("unexpected end of file");

        for (int x = 0; x < width; ++x) {
            const uint8_t b = row[static_cast<size_t>(x * 3 + 0)];
            const uint8_t g = row[static_cast<size_t>(x * 3 + 1)];
            const uint8_t r = row[static_cast<size_t>(x * 3 + 2)];
            img.SetPixel(x, dst_y, Pixel{r, g, b});
        }

        if (padding) {
            char pad[3]{};
            in.read(pad, padding);
            if (!in) throw std::runtime_error("unexpected end of file");
        }
    }

    return img;
}

void WriteBmp(const std::string& path, const Image& image) {
    const int width = image.GetWidth();
    const int height = image.GetHeight();
    if (width <= 0 || height <= 0) throw std::runtime_error("empty image");

    const int row_bytes = width * 3;
    const int padding = (4 - (row_bytes % 4)) % 4;
    const uint32_t data_size = static_cast<uint32_t>((row_bytes + padding) * height);
    const uint32_t data_offset = 14 + 40;
    const uint32_t file_size = data_offset + data_size;

    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("cannot open output file");

    out.put('B');
    out.put('M');
    WriteU32(out, file_size);
    WriteU16(out, 0);
    WriteU16(out, 0);
    WriteU32(out, data_offset);

    WriteU32(out, 40);
    WriteI32(out, width);
    WriteI32(out, height);
    WriteU16(out, 1);
    WriteU16(out, 24);
    WriteU32(out, 0);
    WriteU32(out, data_size);
    WriteI32(out, 2835);
    WriteI32(out, 2835);
    WriteU32(out, 0);
    WriteU32(out, 0);

    std::vector<uint8_t> row(static_cast<size_t>(row_bytes));
    const uint8_t pad[3]{0, 0, 0};

    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            const Pixel p = image.GetPixel(x, y);
            row[static_cast<size_t>(x * 3 + 0)] = p.b;
            row[static_cast<size_t>(x * 3 + 1)] = p.g;
            row[static_cast<size_t>(x * 3 + 2)] = p.r;
        }
        out.write(reinterpret_cast<const char*>(row.data()), row_bytes);
        if (padding) out.write(reinterpret_cast<const char*>(pad), padding);
    }

    if (!out) throw std::runtime_error("failed to write BMP");
}