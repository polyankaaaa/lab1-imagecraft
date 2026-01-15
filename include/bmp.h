#pragma once
#include <string>
#include "image.h"

Image ReadBMP(const std::string& path);
void WriteBMP(const std::string& path, const Image& img);