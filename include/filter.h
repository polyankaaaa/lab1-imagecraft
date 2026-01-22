#pragma once

#include "image.h"

class Filter {
public:
    virtual ~Filter() = default;
    virtual void Apply(Image& image) const = 0;
};