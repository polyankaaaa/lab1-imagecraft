#include "bmp.h"
#include "filter_factory.h"

#include <exception>
#include <iostream>
#include <string>

static void PrintUsage(std::ostream& os) {
    os
        << "Usage:\n"
        << "  ./image_craft <input.bmp> <output.bmp> [filters...]\n\n"
        << "Example:\n"
        << "  ./image_craft input.bmp /tmp/output.bmp -crop 800 600 -gs -blur 0.5\n\n"
        << "Filters:\n"
        << "  -crop <width> <height>\n"
        << "  -gs\n"
        << "  -neg\n"
        << "  -sharp\n"
        << "  -edge <threshold>\n"
        << "  -blur <sigma>\n"
        << "  -gamma <value>\n"
        << "  -med <window>\n"
        << "  -hist_eq\n";
}

int main(int argc, char* argv[]) {
    try {
        if (argc < 3) {
            PrintUsage(std::cerr);
            return 1;
        }

        const std::string input_path = argv[1];
        const std::string output_path = argv[2];

        Image img = ReadBmp(input_path);

        auto filters = CreateFilters(argc, argv, 3);
        for (auto& f : filters) {
            img = f->Apply(img);
        }

        WriteBmp(output_path, img);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n\n";
        PrintUsage(std::cerr);
        return 1;
    }
}