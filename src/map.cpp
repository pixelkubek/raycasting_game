#include <iostream>

#include "map.h"

void Map::setWallTexture(std::string filePath) {
    wallTexture = Texture(filePath);
}

void Map::setEntranceTexture(std::string filePath) {
    entranceTexture = Texture(filePath);
}

void Map::setExitTexture(std::string filePath) {
    exitTexture = Texture(filePath);
}

Map::Map() {
    map = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 1, 1, 0, 1, 0, 0, 1},
        {1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
        {1, 0, 0, 1, 0, 0, 1, 0, 0, 1},
        {1, 0, 0, 1, 0, 1, 1, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 2, 1},
    };

    setWallTexture("../textures/myTexture3.ppm");
    setEntranceTexture("../textures/entranceTextureP3.ppm");
    setExitTexture("../textures/exitTextureP3.ppm");
}

Texture &Map::getTexture(int x, int y) {
    switch (map[y][x]) {
    case 1:
        return wallTexture;

    case 2:
        return exitTexture;

    case 3:
        return entranceTexture;

    default:
        return deafultTexture;
    }
}

sf::Color Map::getTileColor(int x, int y) {
    switch (map[y][x]) {
    case 1:
        return sf::Color::Black;

    case 2:
        return sf::Color::Green;

    case 3:
        return sf::Color::Yellow;

    default:
        return sf::Color::White;
    }
}

// Random int in range (inclusive)
int Map::randInRange(int start, int end) {
    return rand() % (end - start + 1) + start;
}

// Regenerate map into a maze
void Map::generateMaze(int x1, int y1, int x2, int y2) {
    int xRange = x2 - x1, yRange = y2 - y1;
    if (xRange <= 0 || yRange <= 0) return; // Recursion exit point

    if (xRange <= yRange) {
        // Find and odd y coord to put wall
        int wallPosition;
        do {
            wallPosition = randInRange(y1, y2);
        } while (wallPosition % 2 != 0);

        // Fill the wall
        for (int i = x1; i <= x2; i++) {
            map[wallPosition][i] = 1;
        }

        int holePosition;
        do {
            holePosition = randInRange(x1, x2);
        } while (holePosition % 2 == 0);

        map[wallPosition][holePosition] = 0;

        generateMaze(x1, y1, x2, wallPosition - 1);
        generateMaze(x1, wallPosition + 1, x2, y2);
    } else {
        // Find and odd x coord to put wall
        int wallPosition;
        do {
            wallPosition = randInRange(x1, x2);
        } while (wallPosition % 2 != 0);

        // Fill the wall
        for (int i = y1; i <= y2; i++) {
            map[i][wallPosition] = 1;
        }

        int holePosition;
        do {
            holePosition = randInRange(y1, y2);
        } while (holePosition % 2 == 0);

        map[holePosition][wallPosition] = 0;

        generateMaze(x1, y1, wallPosition - 1, y2);
        generateMaze(wallPosition + 1, y1, x2, y2);
    }
}

// size_x and size_y MUST be odd
Map::Map(int size_x, int size_y) {
    if (size_x % 2 == 0 || size_y % 2 == 0)
        throw std::invalid_argument("size_x and size_y must be odd");

    map.clear();
    map.push_back(std::vector<int>((size_t)size_x + 2, 1));
    for (int i = 1; i < size_y + 1; i++) {
        map.push_back(std::vector<int>((size_t)size_x + 2, 0));
        map.back().front() = map.back().back() = 1;
    }
    map.push_back(std::vector<int>((size_t)size_x + 2, 1));

    generateMaze(1, 1, size_x, size_y);
    map[size_y + 1][size_x] = 2; // Set exit.
    map[0][1] = 3;               // Set entrance;

    // setWallTexture("../textures/starry_night.ppm");
    setWallTexture("../textures/myTexture" + std::to_string(rand() % 3 + 1) + ".ppm");
    setEntranceTexture("../textures/entranceTextureP3.ppm");
    setExitTexture("../textures/exitTextureP3.ppm");
}

const std::vector<std::vector<int>> &Map::getMap() { return map; }

void Map::print() {
    for (std::vector<int> line : map) {
        for (int val : line) {
            std::cout << val;
        }
        std::cout << std::endl;
    }
}
