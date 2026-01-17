#pragma once

#include <vector>
#include "filter.h"

// Парсит аргументы командной строки и возвращает список фильтров,
// которые надо применить по порядку.
std::vector<FilterPtr> BuildFilters(int argc, char** argv);

// Фабрики фильтров (если у вас они называются иначе — переименуйте вызовы в filter_factory.cpp)
FilterPtr MakeGrayscale();
FilterPtr MakeCrop(int w, int h);
FilterPtr MakeNegative();
FilterPtr MakeSharpen();
FilterPtr MakeEdge(int threshold);
FilterPtr MakeMedian(int radius);
