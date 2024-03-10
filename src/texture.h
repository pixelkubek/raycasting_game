#include<vector>
#include<SFML/Graphics.hpp>

class Texture {
    std::vector<std::vector<sf::Color>> colorMap;

public:
    Texture();

    // Load texture from P3 .ppm file.
    Texture(std::string filePath);

    // Print r, g, b values of colors in array
    void print();

    size_t getHeight();
    size_t getWidth();
    const std::vector<std::vector<sf::Color>> &getColorMap();
};