#include "BMPImage.h"
#include <fstream>
#include <iostream>
#include <string>
#include <charconv>

//bmpclass.h

bool BMPImage::read(const std::string& file_name) {
    std::ifstream file(file_name, std::ios::binary);

    if (!file.is_open()) {                                                           // no such file exists or failed to open
        return false;
    }

    std::uint8_t header[54];
    file.read(reinterpret_cast<char*>(header), 54);

    // check if 54 bytes (size of header) was read
    if (file.gcount() != 54) {
        std::cerr << "Файл слишком короткий, заголовок поврежден" << std::endl;
        return false;
    }

    if (header[0] != 'B' || header[1] != 'M') {                                      // incorrect file type
        return false;
    }

    // width, height and bitsPerPixel are read from predifined by the standard bytes of the header
    width = *reinterpret_cast<int*>(&header[18]);
    int raw_height = *reinterpret_cast<int*>(&header[22]);
    std::uint16_t bits_per_pixel = *reinterpret_cast<std::uint16_t*>(&header[28]);

    if (bits_per_pixel != 24) {                                                      // incorrect value of bitsPerPixel (not blue green red = 3 * 8 = 24)
        std::cerr << "Поддерживаются только 24-битные .bmp файлы" << std::endl;
        return false;
    }

    bool is_bottom_up = (raw_height > 0);                                              // define if the image is bottom-up oriented
    height = std::abs(raw_height);

    pixels.resize(width * height);                                                   // resize a vector of pixels to fit the image

    int padding = (4 - (width * 3) % 4) % 4;                                         // counted a padding (same for every line of pixels)
    std::size_t bytes_to_read = width * sizeof(Pixel);
    
    for (int y = 0; y < height; ++y) {
        // reading one horisontal line of pixels
        int target_y = is_bottom_up ? (height - 1 - y) : y;

        // writing pixel data into vector, which index depends on whether image is top-bottom or bottom-top
        file.read(reinterpret_cast<char*>(&pixels[target_y * width]), width * sizeof(Pixel));

        // check if full line was read
        if (file.gcount() != bytes_to_read) {
            std::cerr << "Файл оборвался на строке " << y << std::endl;
            return false;
        }

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

    uint8_t padding_bytes[3] = {0, 0, 0};                                             // buffer for paddings
    
    for (int y = 0; y < height; ++y) {
        int source_y = height - 1 - y;
    
        // write a line of pure pixels values
        file.write(reinterpret_cast<const char*>(&pixels[source_y * width]), width * sizeof(Pixel));

        // write to the same line padding zeros if needed
        file.write(reinterpret_cast<const char*>(padding_bytes), padding);
    }

    file.close();
    return true;
}

// Убирает пробелы, \t, \r по краям строки
std::string trim(const std::string& s) {
    int lindex = 0, rindex = s.size() - 1;
    while (lindex < s.size() && (s[lindex] == ' ' || s[lindex] == '\t' || s[lindex] == '\r')) ++lindex;
    while (rindex >= 0 && (s[rindex] == ' ' || s[rindex] == '\t' || s[rindex] == '\r')) --rindex;

    if (lindex == s.size()) return "";

    return s.substr(lindex, rindex - lindex + 1);
}

bool loadConfig(const std::string& ini_path, WatermarkConfig& config) {
    std::ifstream file(ini_path);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);

        if (line == "") continue;

        std::size_t pos = line.find('='); // выбор параметра для записи значения и представление числа в строчном типе
        if (pos == std::string::npos) return false;
        std::string key = trim(line.substr(0, pos)); 
        std::string svalue = trim(line.substr(pos + 1));

        int value = 0;
        const char* begin = svalue.data();
        const char* end = begin + svalue.size();
        auto result = std::from_chars(begin, end, value);
        if (result.ec != std::errc() || result.ptr != end) return false; // проверка на корректность введенного числа

        if (key == "posX") config.posX = value;
        else if (key == "posY") config.posY = value;
        else if (key == "alpha") config.alpha = value;
    }

    return true;
}

std::uint8_t blend_channel(std::uint8_t orig, std::uint8_t mark, int alpha) {
    int new_color_int = orig * (100 - alpha) + mark * alpha;
    new_color_int /= 100;
    std::uint8_t new_color = new_color_int;

    return new_color;
}

bool BMPImage::applyWatermark(const BMPImage& original, const BMPImage& watermark_file, const std::string& ini_path, BMPImage& resultImage) {
    WatermarkConfig curConfig;
    if (!loadConfig(ini_path, curConfig)) {
        std::cerr << "Ошибка чтения файла .ini\n";
        return false;
    }
    if (curConfig.posX < 0 || curConfig.posY < 0 || (0 > curConfig.alpha || curConfig.alpha > 100)) {
        std::cerr << "Некорректные данные для конфигурации\n";
        return false;
    }

    int markWidth = watermark_file.getWidth(), markHeight = watermark_file.getHeight();
    int originalWidth = original.getWidth(), originalHeight = original.getHeight();

    resultImage.width = originalWidth;
    resultImage.height = originalHeight;

    // копирование оригинального изображения
    resultImage.pixels.resize(originalHeight * originalWidth);
    for (int i = 0; i < originalHeight * originalWidth; ++i) {
        resultImage.pixels[i] = original.pixels[i]; 
    }

    for (int cy = 0; cy < markHeight; ++cy) {
        for (int cx = 0; cx < markWidth; ++cx) {
            int posx = curConfig.posX + cx;
            int posy = curConfig.posY + cy;

            if (posx < originalWidth && posy < originalHeight) {
                // выбор нужного пикселя в векторе original и watermark_file для каждого канала
                std::uint8_t orig_channelB = original.pixels[posy * originalWidth + posx].blue; 
                std::uint8_t mark_channelB = watermark_file.pixels[cy * markWidth + cx].blue;
                std::uint8_t new_channelB = blend_channel(orig_channelB, mark_channelB, curConfig.alpha);

                std::uint8_t orig_channelG = original.pixels[posy * originalWidth + posx].green;
                std::uint8_t mark_channelG = watermark_file.pixels[cy * markWidth + cx].green;
                std::uint8_t new_channelG = blend_channel(orig_channelG, mark_channelG, curConfig.alpha);

                std::uint8_t orig_channelR = original.pixels[posy * originalWidth + posx].red;
                std::uint8_t mark_channelR = watermark_file.pixels[cy * markWidth + cx].red;
                std::uint8_t new_channelR = blend_channel(orig_channelR, mark_channelR, curConfig.alpha);

                // запись нового пикселя
                resultImage.pixels[posy * originalWidth + posx].blue = new_channelB;
                resultImage.pixels[posy * originalWidth + posx].green = new_channelG;
                resultImage.pixels[posy * originalWidth + posx].red = new_channelR;
            }
        }
    }

    return true;
}

int BMPImage::getWidth() const { return width; }
int BMPImage::getHeight() const { return height; }
std::vector<Pixel>& BMPImage::getPixels() { return pixels; }