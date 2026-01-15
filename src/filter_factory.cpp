#include "filter_factory.h"
#include "utils.h"

#include <stdexcept>
#include <string>
#include <vector>

// Преобразование строки в int с хорошей ошибкой
static int ToInt(const char* s, const char* what) {
    try {
        size_t pos = 0;
        int v = std::stoi(std::string(s), &pos);
        if (pos != std::string(s).size()) {
            throw std::runtime_error("bad");
        }
        return v;
    } catch (...) {
        throw std::runtime_error(std::string("Bad integer for ") + what + ": '" + s + "'");
    }
}

std::vector<FilterPtr> BuildFilters(int argc, char** argv) {
    // Ожидаем: imagecraft <input.bmp> <output.bmp> [filters...]
    if (argc < 3) {
        throw std::runtime_error(
            "Usage: imagecraft <input.bmp> <output.bmp> "
            "[--gs] [--neg] [--sharp] [--edge <t>] [--med <r>] [--crop <w> <h>]"
        );
    }

    std::vector<FilterPtr> filters;

    // Фильтры начинаются с argv[3]
    for (int i = 3; i < argc; ++i) {
        std::string key = argv[i];
        key = ToLower(key);

        if (key == "--gs") {
            filters.push_back(MakeGrayscale());
        } else if (key == "--neg") {
            filters.push_back(MakeNegative());
        } else if (key == "--sharp") {
            filters.push_back(MakeSharpen());
        } else if (key == "--edge") {
            if (i + 1 >= argc) throw std::runtime_error("Missing value for --edge <threshold>");
            int t = ToInt(argv[i + 1], "edge threshold");
            filters.push_back(MakeEdge(t));
            i += 1;
        } else if (key == "--med") {
            if (i + 1 >= argc) throw std::runtime_error("Missing value for --med <radius>");
            int r = ToInt(argv[i + 1], "median radius");
            filters.push_back(MakeMedian(r));
            i += 1;
        } else if (key == "--crop") {
            if (i + 2 >= argc) throw std::runtime_error("Missing values for --crop <w> <h>");
            int w = ToInt(argv[i + 1], "crop width");
            int h = ToInt(argv[i + 2], "crop height");
            filters.push_back(MakeCrop(w, h));
            i += 2;
        } else {
            throw std::runtime_error("Unknown filter: " + key);
        }
    }

    return filters;
}