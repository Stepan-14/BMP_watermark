#include "BMPImage.h"
#include <fstream>
#include <iostream>

bool BMPImage::read(const std::string& file_name) {
    std::ifstream file(file_name, std::ios::binary);

    if (!file.is_open()) {                                                           // no such file exists or failed to open
        std::cerr << "Не удалось открыть файл: " << file_name << std::endl;
        return false;
    }

    std::uint8_t header[54];
    file.read(reinterpret_cast<char*>(header), 54);

    if (header[0] != 'B' || header[1] != 'M') {                                      // incorrect file type
        std::cerr << "Некорректный формат файла. Требуется .bmp файл" << std::endl;
        return false;
    }

    // width, height and bitsPerPixel are read from predifined by the standard bytes of the header
    width = *reinterpret_cast<int*>(&header[18]);
    int rawHeight = *reinterpret_cast<int*>(&header[22]);
    std::uint16_t bitsPerPixel = *reinterpret_cast<std::uint16_t*>(&header[28]);

    if (bitsPerPixel != 24) {                                                        // incorrect value of bitsPerPixel (not blue green red)
        std::cerr << "Поддерживаются только 24-битные .bmp файлы" << std::endl;
        return false;
    }

    bool isBottomUp = (rawHeight > 0);                                               // define if the image is bottom-up oriented
    height = std::abs(rawHeight);

    pixels.resize(width * height);                                                   // resize a vector of pixels to fit the image

    int padding = (4 - (width * 3) % 4) % 4;                                         // counted a padding (same for every line of pixels)

    for (int y = 0; y < height; ++y) {
        // reading one horisontal line of pixels
        int targetY = isBottomUp ? (height - 1 - y) : y;

        // writing pixel data into vector, which index depends on whether image is top-bottom or bottom-top
        file.read(reinterpret_cast<char*>(&pixels[targetY * width]), width * sizeof(Pixel));

        // ignoring paddings (if there are any)
        file.ignore(padding);
    }

    file.close();
    return true;
}

bool BMPImage::save(const std::string& file_name) {
    return true;
}

int BMPImage::getWidth() const { return width; }
int BMPImage::getHeight() const { return height; }
const std::vector<Pixel>& BMPImage::getPixels() const { return pixels; }