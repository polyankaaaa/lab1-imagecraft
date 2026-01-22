#pragma once

#include <memory>

#include "filter.h"

std::unique_ptr<Filter> MakeGamma(double gamma);