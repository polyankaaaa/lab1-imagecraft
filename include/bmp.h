#pragma once

#include <string>

#include "image.h"

Image ReadBmp(const std::string& path);
void WriteBmp(const std::string& path, const Image& image);