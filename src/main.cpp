#include "bmp.h"
#include "filter_factory.h"

#include <iostream>
#include <string>

int main(int argc, char** argv) {
    try {
        if (argc < 3) {
            std::cerr
                << "Usage: imagecraft <input.bmp> <output.bmp> "
                   "[--gs] [--neg] [--sharp] [--edge <t>] [--med <r>] [--crop <w> <h>]\n";
            return 1;
        }

        const std::string inPath = argv[1];
        const std::string outPath = argv[2];

        Image img = ReadBMP(inPath);

        auto filters = BuildFilters(argc, argv);
        for (const auto& f : filters) {
            f->Apply(img);
        }

        WriteBMP(outPath, img);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}