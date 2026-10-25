#pragma once
#include <cstdint>
#include <fstream>

struct Pixel {
    uint8_t blue = 0;
    uint8_t green = 0;
    uint8_t red = 0;
};

class BMPImage {
    bool read_image(const std::string& filename);
};