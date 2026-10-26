#include "BMPImage.h"
#include <fstream>
#include <iostream>

bool BMPImage::read(const std::string& file_name) {
    std::ifstream file(file_name, std::ios::binary);

    if (!file.is_open()) {                                                           // no such file exists or failed to open
        return false;
    }

    std::uint8_t header[54];
    file.read(reinterpret_cast<char*>(header), 54);

    if (header[0] != 'B' || header[1] != 'M') {                                      // incorrect file type
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
    std::ofstream file(file_name, std::ios::binary);
    
    if (!file.is_open()) {
        return false;
    }

    // counting real sizes (with padding)
    int padding = (4 - (width * 3) % 4) % 4;
    int size_image = (width * 3 + padding) * height;                                 // size of pixel data
    int file_size = 54 + size_image;                                                 // full size of file with header

    // writing the header that is needed
    uint8_t header[54] = {
        'B', 'M',                                                                    // 0-1: type of file: .bmp
        0, 0, 0, 0,                                                                  // 2-5: size of file (is written below)
        0, 0, 0, 0,                                                                  // 6-9: reserved
        54, 0, 0, 0,                                                                 // 10-13: bytes until pixel information
        40, 0, 0, 0,                                                                 // 14-17: size of info-header
        0, 0, 0, 0,                                                                  // 18-21: width (is written below)
        0, 0, 0, 0,                                                                  // 22-25: height (is written below)
        1, 0,                                                                        // 26-27: color planes (useless)
        24, 0,                                                                       // 28-29: bitsPerPixel (24: blue green red)
        0, 0, 0, 0,                                                                  // 30-33: compression (0 = none)
        0, 0, 0, 0,                                                                  // 34-37: size of image (is written below)
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0                               // 38-53: useless zeros
    };

    // writing values into header bytes
    *reinterpret_cast<int*>(&header[2])  = file_size;
    *reinterpret_cast<int*>(&header[18]) = width;
    *reinterpret_cast<int*>(&header[22]) = height;
    *reinterpret_cast<int*>(&header[34]) = size_image;

    // write ready header into file
    file.write(reinterpret_cast<char*>(header), 54);

    uint8_t paddingBytes[3] = {0, 0, 0};                                             // buffer for paddings
    
    for (int y = 0; y < height; ++y) {
        int sourceY = height - 1 - y;
    
        // write a line of pure pixels values
        file.write(reinterpret_cast<const char*>(&pixels[sourceY * width]), width * sizeof(Pixel));

        // write to the same line padding zeros if needed
        file.write(reinterpret_cast<const char*>(paddingBytes), padding);
    }

    file.close();
    return true;
}

int BMPImage::getWidth() const { return width; }
int BMPImage::getHeight() const { return height; }
std::vector<Pixel>& BMPImage::getPixels() { return pixels; }