#include "texture.h"
#include <fstream>
#include <iostream>

Texture::Texture() {
    colorMap.push_back({sf::Color::Black});
}

// Load texture from P3 .ppm file.
Texture::Texture(std::string filePath) {
    std::ifstream file;
    file.open(filePath);

    int width, height;
    std::string temp;
    unsigned int r, g, b;

    // Ignore P3 and max range.
    file >> temp >> width >> height >> temp;

    // Load rgb values into 2d color vector.
    for (int i = 0; i < height; i++) {
        std::vector<sf::Color> currentLine;
        for (int j = 0; j < width; j++) {
            file >> r >> g >> b;
            currentLine.push_back(sf::Color((unsigned char)r, (unsigned char)g, (unsigned char)b, 255));
        }
        colorMap.push_back(currentLine);
        currentLine.clear();
    }
    file.close();
}

// Print r, g, b values of colors in array
void Texture::print() {
    std::cout << colorMap.size() << std::endl;
    for (std::vector<sf::Color> line : colorMap) {
        for (sf::Color color : line) {
            std::cout << "(" << (unsigned int)color.r << " " << (unsigned int)color.g << " " << (unsigned int)color.b << ")\t";
        }
        std::cout << std::endl;
    }
}

size_t Texture::getHeight() { return colorMap.size(); }
size_t Texture::getWidth() { return colorMap.front().size(); }
const std::vector<std::vector<sf::Color>> &Texture::getColorMap() {
    return colorMap;
}
