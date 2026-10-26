#include <iostream>
#include "BMPImage.h"

int main() {
    BMPImage image;
    image.read("input/Car.bmp");
    
    BMPImage watermark;
    // image.read("input/<file_name>.bmp");
    

    // start of the test program that inverts colors
    std::vector<Pixel>& allPixels = image.getPixels();
    
    for (size_t i = 0; i < allPixels.size(); ++i) {
        allPixels[i].blue  = 255 - allPixels[i].blue;                                // invert blue
        allPixels[i].red   = 255 - allPixels[i].red;                                 // invert red
        allPixels[i].green = 255 - allPixels[i].green;                               // invert green
    }

    if (image.save("output/output.bmp")) {
        std::cout << "Измененный файл успешно сохранен" << std::endl;
    } else {
        std::cout << "Ошибка при сохранении файла" << std::endl;
    }
    // finish of the test program
}