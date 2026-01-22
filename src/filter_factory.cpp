#include "filter_factory.h"

#include "filters/blur.h"
#include "filters/crop.h"
#include "filters/edge.h"
#include "filters/gamma.h"
#include "filters/gs.h"
#include "filters/hist_eq.h"
#include "filters/med.h"
#include "filters/neg.h"
#include "filters/sharp.h"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <stdexcept>

static int ToInt(const std::string& s) {
    char* end = nullptr;
    long v = std::strtol(s.c_str(), &end, 10);
    if (!end || *end != '\0') throw std::invalid_argument("bad integer: " + s);
    if (v < std::numeric_limits<int>::min() || v > std::numeric_limits<int>::max()) throw std::invalid_argument("int out of range: " + s);
    return static_cast<int>(v);
}

static double ToDouble(const std::string& s) {
    char* end = nullptr;
    double v = std::strtod(s.c_str(), &end);
    if (!end || *end != '\0') throw std::invalid_argument("bad number: " + s);
    return v;
}

void PrintUsage(const std::string& exe) {
    std::cout
        << "Usage:\n"
        << "  " << exe << " <input.bmp> <output.bmp> [filters...]\n\n"
        << "Filters:\n"
        << "  --crop <width> <height>\n"
        << "  --gs\n"
        << "  --neg\n"
        << "  --sharp\n"
        << "  --edge <threshold01>\n"
        << "  --blur <sigma>\n"
        << "  --med <radius>\n"
        << "  --gamma <gamma>\n"
        << "  --histeq\n";
}

std::vector<std::unique_ptr<Filter>> ParseFilters(const std::vector<std::string>& args, size_t start_index) {
    std::vector<std::unique_ptr<Filter>> fs;

    size_t i = start_index;
    while (i < args.size()) {
        const std::string& f = args[i];

        if (f == "--gs") {
            fs.push_back(MakeGrayscale());
            ++i;
        } else if (f == "--neg") {
            fs.push_back(MakeNegative());
            ++i;
        } else if (f == "--sharp") {
            fs.push_back(MakeSharpen());
            ++i;
        } else if (f == "--histeq") {
            fs.push_back(MakeHistEq());
            ++i;
        } else if (f == "--crop") {
            if (i + 2 >= args.size()) throw std::invalid_argument("--crop expects 2 arguments");
            const int w = ToInt(args[i + 1]);
            const int h = ToInt(args[i + 2]);
            fs.push_back(MakeCrop(w, h));
            i += 3;
        } else if (f == "--edge") {
            if (i + 1 >= args.size()) throw std::invalid_argument("--edge expects 1 argument");
            const double t = ToDouble(args[i + 1]);
            fs.push_back(MakeEdge(t));
            i += 2;
        } else if (f == "--blur") {
            if (i + 1 >= args.size()) throw std::invalid_argument("--blur expects 1 argument");
            const double sigma = ToDouble(args[i + 1]);
            fs.push_back(MakeBlur(sigma));
            i += 2;
        } else if (f == "--med") {
            if (i + 1 >= args.size()) throw std::invalid_argument("--med expects 1 argument");
            const int r = ToInt(args[i + 1]);
            fs.push_back(MakeMedian(r));
            i += 2;
        } else if (f == "--gamma") {
            if (i + 1 >= args.size()) throw std::invalid_argument("--gamma expects 1 argument");
            const double g = ToDouble(args[i + 1]);
            fs.push_back(MakeGamma(g));
            i += 2;
        } else if (f == "--help" || f == "-h") {
            throw std::invalid_argument("help");
        } else {
            throw std::invalid_argument("unknown filter: " + f);
        }
    }

    return fs;
}