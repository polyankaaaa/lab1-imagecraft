#pragma once

#include <memory>

#include "filter.h"

std::unique_ptr<Filter> MakeMedian(int radius);