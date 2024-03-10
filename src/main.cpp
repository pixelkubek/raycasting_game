#include <fstream>
#include <iostream>
#include <random>

#include "texture.h"
#include "map.h"
#include "window.h"
#include "player.h"
#include "game.h"

int main() {
    srand((unsigned int)time(NULL));
    std::ifstream options;
    size_t LENGTH = 1280, HEIGHT = 720, SCALE = 3, MAZE_WIDTH = 11, MAZE_HEIGHT = 11;
    std::string temp;

    options.open("../settings.txt");
    options >> temp >> LENGTH >> temp >> HEIGHT >> temp >> SCALE >> temp >> MAZE_WIDTH >> temp >> MAZE_HEIGHT;

    options.close();

    Game game(LENGTH, HEIGHT, (int)SCALE, (int)MAZE_WIDTH, (int)MAZE_HEIGHT);
    game.play();

    return 0;
}