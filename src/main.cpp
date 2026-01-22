#include "bmp.h"
#include "filter_factory.h"

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    std::vector<std::string> args;
    args.reserve(static_cast<size_t>(argc));
    for (int i = 0; i < argc; ++i) args.emplace_back(argv[i]);

    const std::string exe = (argc > 0) ? args[0] : "imagecraft";

    if (argc < 3) {
        PrintUsage(exe);
        return 1;
    }

    if (args[1] == "--help" || args[1] == "-h") {
        PrintUsage(exe);
        return 0;
    }

    const std::string input = args[1];
    const std::string output = args[2];

    try {
        Image img = ReadBmp(input);
        auto filters = ParseFilters(args, 3);
        for (const auto& f : filters) {
            f->Apply(img);
        }
        WriteBmp(output, img);
    } catch (const std::invalid_argument& e) {
        if (std::string(e.what()) == "help") {
            PrintUsage(exe);
            return 0;
        }
        std::cerr << "Argument error: " << e.what() << "\n";
        PrintUsage(exe);
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}