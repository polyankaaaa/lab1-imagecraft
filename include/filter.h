#pragma once
#include <memory>
#include "image.h"

class Filter {
public:
    virtual ~Filter() = default;
    virtual void Apply(Image& img) const = 0;
};

using FilterPtr = std::unique_ptr<Filter>;