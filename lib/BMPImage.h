#pragma once
#include <cstdint>
#include <string>
#include <vector>

struct Pixel {
    std::uint8_t blue = 0;
    std::uint8_t green = 0;
    std::uint8_t red = 0;
};

class BMPImage {
public:
    BMPImage() = default;
    
    bool read(const std::string& file_name);

    bool save(const std::string& file_name);

    int getWidth() const;
    int getHeight() const;
    std::vector<Pixel>& getPixels();

private:
    int width = 0;
    int height = 0;
    
    std::vector<Pixel> pixels;
};