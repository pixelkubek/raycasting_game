#ifndef mapH
#define mapH

#include <vector>

#include "texture.h"

// Class representing a game map geometry, textures and marked places
class Map {
    std::vector<std::vector<int>> map;
    Texture wallTexture, deafultTexture, entranceTexture, exitTexture;

public:
    void setWallTexture(std::string filePath);

    void setEntranceTexture(std::string filePath);

    void setExitTexture(std::string filePath);

    Map();

    Texture &getTexture(int x, int y);

    sf::Color getTileColor(int x, int y);

private:
    // Random int in range (inclusive)
    int randInRange(int start, int end);

    // Regenerate map into a maze
    void generateMaze(int x1, int y1, int x2, int y2);

public:
    // size_x and size_y MUST be odd
    Map(int size_x, int size_y);

    const std::vector<std::vector<int>> &getMap();

    void print();
};
#endif