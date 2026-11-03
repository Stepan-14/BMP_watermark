#include <iostream>
#include "BMPImage.h"

int main() {
    const std::string ini_path = "input/settings.ini";
    const std::string watermark_name = "input/logo.bmp";
    const std::string input_name = "input/Car.bmp";
    const std::string output_name = "output/output.bmp";

    BMPImage image;
    image.read(input_name);
    
    BMPImage watermark;
    watermark.read(watermark_name);

    BMPImage resultImage;
    if (image.applyWatermark(watermark, ini_path, resultImage, output_name)) {
        std::cout << "Recording and Saving were ended correctly!\n";
    }
}