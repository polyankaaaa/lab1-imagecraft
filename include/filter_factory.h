#pragma once

#include <memory>
#include <vector>

class Filter;

std::vector<std::unique_ptr<Filter>> CreateFilters(int argc, char** argv, int start_index);