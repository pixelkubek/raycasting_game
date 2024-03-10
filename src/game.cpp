#include<iostream>
#include<chrono>

#include "game.h"

// Class representing game logic.
Game::Game(size_t length, size_t height, int scale, int maze_x_starting_size, int maze_y_starting_size) {
    trueLength = length;
    trueHeight = height;
    gameLength = length / scale;
    gameHeight = height / scale;
    pWindow = new Window(gameLength, gameHeight, scale, "Maze finder");

    // Ensure odd dimensions
    map = Map(maze_x_starting_size + 1 - (maze_x_starting_size % 2), maze_y_starting_size + 1 - (maze_y_starting_size % 2));

    sky = Texture("../textures/skyTexture2P3.ppm");
    // sky = Texture("../textures/starry_night_sky.ppm");

    helperWindowScale = std::max((int)std::min(length / 2 / map.getMap().front().size(), height / 2 / map.getMap().size()), 1); // scale to main window.
}

void Game::changeSkyTexture(std::string filePath) {
    sky = Texture(filePath);
}

void Game::renderFrameToBuffer() {
    // For each pixel in screen width cast a ray to a corresponding angle in fov range and extend
    // until it hits a wall, then calculate necessary wall height

    // Use getters only once
    float playerX = player.getX(), playerY = player.getY(), playerAngle = player.getAngle();

    // Rays range from playerAngle - (fov / 2) to playerAngle + (fov / 2)
    float rayAngle = playerAngle - (float)fov / 2.f;

    float incrimentAngle = (float)fov / (float)gameLength;

    float halfHeight = (float)gameHeight / 2.f;

    const std::vector<std::vector<int>> &mapArray = map.getMap();

    for (unsigned int rayCount = 0; rayCount < gameLength; rayCount++) {
        // Normalise rayAngle.
        while (rayAngle > 360)
            rayAngle -= 360;
        while (rayAngle < 0)
            rayAngle += 360;

        float rayX = playerX, rayY = playerY; // Ray starts from the player.

        // Calculate ray increments in x, y coordinates
        float rayCosIncrement = cosf(degreesToRadians(rayAngle)) / (float)rayCastingPrecision;
        float raySinIncrement = sinf(degreesToRadians(rayAngle)) / (float)rayCastingPrecision;

        // Wall collision check.
        bool hitWall = false, hitFromX;
        while (!hitWall) {
            // Necessary for calculating texture orientation.
            hitFromX = mapArray[(int)rayY][(int)(rayX + raySinIncrement)];

            // 0 degrees is positive y, 90 degrees positive x
            // so the angle to x if 90 - playerAngle, which flips sin and cos
            rayX += raySinIncrement;
            rayY += rayCosIncrement;

            hitWall = mapArray[(int)rayY][(int)rayX];
        }

        // Calculate distance to wall using Pythagoreas theorem
        float distanceToWall = sqrtf(fabsf(playerX - rayX) * fabsf(playerX - rayX) + fabsf(playerY - rayY) * fabsf(playerY - rayY));

        // Fisheye fix, normalise to playerAngle vector
        distanceToWall *= cosf(degreesToRadians(rayAngle - playerAngle));

        float wallHeight = halfHeight / distanceToWall;

        // Load wall texture
        Texture &t = map.getTexture((int)rayX, (int)rayY);

        // Calculate which vertical strip of the texture to use
        // By multiplying a coordinate by texture width, we get it's equivalent
        // in a map made of texture pixels, then by taking the modulus
        // we get which pixel strip in texture.
        // One of rayX and rayY is close to edge, so it doesn't affect the chosen coordinate

        int textureVerticalSlipIdx;
        int textureWidth = (int)t.getWidth();
        if (!hitFromX) {
            textureVerticalSlipIdx = (int)roundf((float)textureWidth * rayX) % textureWidth;
            // Flip the textures when necessary.
            if (roundf(rayY) > floorf(rayY)) textureVerticalSlipIdx = textureWidth - textureVerticalSlipIdx - 1;
        } else {
            textureVerticalSlipIdx = (int)roundf((float)textureWidth * rayY) % textureWidth;
            // Flip the textures when necessary.
            if (roundf(rayX) <= floorf(rayX)) textureVerticalSlipIdx = textureWidth - textureVerticalSlipIdx - 1;
        }


        // Draw floor.
        pWindow->drawVericalLine(0, (int)halfHeight, rayCount, sf::Color(121, 121, 121, 255));

        // Draw sky.
        // pWindow->drawVericalLine((int)HEIGHT, (int)halfHeight, rayCount, sf::Color(0, 199, 199, 255));
        pWindow->drawTextureVerticalLine(rayCount, halfHeight, (float)gameHeight + 1, (int)((float)sky.getWidth() * (rayAngle / 360.f)) % (int)sky.getWidth(), sky);

        // Draw untextured walls.
        // pWindow->drawVericalLine((int)(halfHeight - wallHeight), (int)(halfHeight + wallHeight), rayCount, sf::Color(100, 62, 10, 100));

        // Draw textures.
        pWindow->drawWallTextureVerticalLine(rayCount, wallHeight, textureVerticalSlipIdx, t);

        rayAngle += incrimentAngle;
    }
}

void Game::renderHelperWindowPixel(int x, int y, const sf::Color &color) {
    for (int i = 0; i < helperWindowScale; i++) {
        for (int j = 0; j < helperWindowScale; j++) {
            pWindow->setTruePixelColor(helperWindowScale * x + i, helperWindowScale * y + j, color);
        }
    }
}

void Game::renderHelperWindow() {
    if (!helperVisibility) return;
    const std::vector<std::vector<int>> &mapArray = map.getMap();

    for (int i = 0; i < (int)mapArray.front().size(); i++) {
        for (int j = 0; j < (int)mapArray.size(); j++) {
            renderHelperWindowPixel(i, j, map.getTileColor(i, j));
        }
    }

    renderHelperWindowPixel((int)player.getX(), (int)player.getY(), sf::Color::Blue);
}

void Game::loadNewMaze() {
    // Includes padding, so the size increases
    map = Map((int)map.getMap().front().size(), (int)map.getMap().size());
    player.setX(1.5f);
    player.setY(1.5f);
    helperWindowScale = std::max((int)std::min(trueLength / 2 / map.getMap().front().size(), trueHeight / 2 / map.getMap().size()), 1); // scale to main window.
    changeSkyTexture("../textures/skyTexture" + std::to_string(rand() % 2 + 1) + "P3.ppm");
}

void Game::play() {
    // Used for calculating deltaTime
    std::chrono::_V2::system_clock::time_point endOfPrevLoop = std::chrono::high_resolution_clock::now();
    std::chrono::_V2::system_clock::time_point spacePress = std::chrono::high_resolution_clock::now();
    const std::vector<std::vector<int>> &mapArray = map.getMap();
    int maze_x = (int)map.getMap().front().size() - 2, maze_y = (int)map.getMap().size() - 2;

    // Game loop.
    while (pWindow->isOpen()) {
        renderFrameToBuffer();

        renderHelperWindow();

        pWindow->display(); // Display buffer.

        // Provide time delta between frames
        player.movement((float)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - endOfPrevLoop).count(), mapArray);

        if (player.getX() >= (float)maze_x && player.getY() >= (float)maze_y + 0.7f) {
            std::cout << "Solved: " << maze_x << " * " << maze_y << std::endl;
            loadNewMaze();
            maze_x += 2;
            maze_y += 2;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - spacePress).count() > 300) {
            helperVisibility = !helperVisibility;
            spacePress = std::chrono::high_resolution_clock::now();
        }

        endOfPrevLoop = std::chrono::high_resolution_clock::now();
    }
}

Game::~Game() {
    delete pWindow;
}

float Game::degreesToRadians(float degrees) {
    return degrees * (float)M_PI / 180.f;
}