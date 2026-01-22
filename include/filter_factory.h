#pragma once

#include <memory>
#include <string>
#include <vector>

#include "filter.h"

std::vector<std::unique_ptr<Filter>> ParseFilters(const std::vector<std::string>& args, size_t start_index);
void PrintUsage(const std::string& exe);