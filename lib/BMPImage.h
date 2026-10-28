#pragma once
#include <cstdint>
#include <string>
#include <vector>

struct Pixel {
    std::uint8_t blue = 0;
    std::uint8_t green = 0;
    std::uint8_t red = 0;
};

struct WatermarkConfig {
    int posX = 0;                                                                    // cordinate x for watermark
    int posY = 0;                                                                    // cordinate y for watermark
    int alpha = 50;                                                                  // percentage of original color (or of watermark color - doesn't matter)
};

WatermarkConfig loadConfig(const std::string& ini_path);

class BMPImage {
public:
    BMPImage() = default;
    
    bool read(const std::string& file_name);

    bool save(const std::string& file_name);

    bool applyWatermark(const BMPImage& original, const BMPImage& watermark_file, const std::string& ini_path, BMPImage& resultImage);

    int getWidth() const;
    int getHeight() const;
    std::vector<Pixel>& getPixels();

private:
    int width = 0;
    int height = 0;
    
    std::vector<Pixel> pixels;
};