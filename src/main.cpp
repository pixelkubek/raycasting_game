#include <SFML/Graphics.hpp>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include <string>

#include "texture.h"

float degreesToRadians(float degrees) {
    return degrees * (float)M_PI / 180.f;
}

// Class representing a game map geometry, textures and marked places
class Map {
    std::vector<std::vector<int>> map;
    Texture wallTexture, deafultTexture, entranceTexture, exitTexture;

public:
    void setWallTexture(std::string filePath) {
        wallTexture = Texture(filePath);
    }

    void setEntranceTexture(std::string filePath) {
        entranceTexture = Texture(filePath);
    }

    void setExitTexture(std::string filePath) {
        exitTexture = Texture(filePath);
    }

    Map() {
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

    Texture &getTexture(int x, int y) {
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

    sf::Color getTileColor(int x, int y) {
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

private:
    // Random int in range (inclusive)
    int randInRange(int start, int end) {
        return rand() % (end - start + 1) + start;
    }

    // Regenerate map into a maze
    void generateMaze(int x1, int y1, int x2, int y2) {
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

public:
    // size_x and size_y MUST be odd
    Map(int size_x, int size_y) {
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

    const std::vector<std::vector<int>> &getMap() { return map; }

    void print() {
        for (std::vector<int> line : map) {
            for (int val : line) {
                std::cout << val;
            }
            std::cout << std::endl;
        }
    }
};

// Rendering window class.
class Window {
    sf::RenderWindow *pRenderWindow;
    size_t gameLength, gameHeight, trueHeight, trueLength;
    sf::VertexArray pixels;
    int scaleModifier;
    bool visibility = true;

public:
    Window(size_t length, size_t height, int scale, std::string title) {
        // gameLength and gameHeight are the dimensions of the simulation, trueLength and trueHeight represent the number of pixels.
        gameLength = length;
        gameHeight = height;
        scaleModifier = scale;
        trueLength = gameLength * scaleModifier;
        trueHeight = gameHeight * scaleModifier;

        pRenderWindow = new sf::RenderWindow(sf::VideoMode((unsigned int)trueLength, (unsigned int)trueHeight), title);

        // Add trueLength * trueHeight pixels.
        pixels = sf::VertexArray(sf::Points, trueLength * trueHeight);

        // Set each pixel to right position on screen
        for (size_t y = 0; y < trueHeight; y++) {
            for (size_t x = 0; x < trueLength; x++) {
                pixels[y * trueLength + x].position = sf::Vector2f((float)(x), (float)(y));
            }
        }
    }

    bool isOpen() {
        return pRenderWindow->isOpen();
    }

    // Set the color of pixel in coords x, y
    void setTruePixelColor(int x, int y, const sf::Color &color) {
        if (x < 0 || (size_t)x >= trueLength || y < 0 || (size_t)y >= trueHeight) return;

        // Flip vertical orientation.
        pixels[(trueHeight - y - 1) * trueLength + x].color = color;
    }

    // Set the color of a simulation pixel
    void setGamePixelColor(int x, int y, sf::Color color) {
        // Sets a square od sides scaleModifier of real pixels.
        for (int i = 0; i < scaleModifier; i++) {
            for (int j = 0; j < scaleModifier; j++) {
                setTruePixelColor(x * scaleModifier + i, y * scaleModifier + j, color);
            }
        }
    }

    // Draw a vertical line.
    void drawVericalLine(int y1, int y2, int x, const sf::Color &color) {
        if (y1 > y2) std::swap(y1, y2);
        for (int y = std::max(y1, 0); y <= std::min(y2, (int)gameHeight); y++) {
            setGamePixelColor(x, y, color);
        }
    }

    // Draws a vertical strip of a texture, texturePositionX determines which.
    void drawTextureVerticalLine(int x, float y1, float y2, int texturePositionX, Texture &texture) {
        float texturePixelSize = (y2 - y1 + 1) / (float)texture.getHeight();

        float y = y1;

        for (int i = 0; i < (int)texture.getHeight(); i++) {
            drawVericalLine((int)round(y), (int)ceil(y + texturePixelSize), x, texture.getColorMap()[texture.getHeight() - i - 1][texturePositionX]);
            y += texturePixelSize;
        }
    }

    // Must be a separate function to avoid float precision artifacts at half height
    void drawWallTextureVerticalLine(int x, float wallHeight, int texturePositionX, Texture &texture) {
        float texturePixelSize = (2.f * wallHeight) / (float)texture.getHeight();
        float y = (float)(gameHeight / 2) - wallHeight;

        for (int i = 0; i < (int)texture.getHeight(); i++) {
            drawVericalLine((int)round(y), (int)ceil(y + texturePixelSize), x, texture.getColorMap()[texture.getHeight() - i - 1][texturePositionX]);
            y += texturePixelSize;
        }
    }

    // Push updates to display
    void display() {
        sf::Event event;
        while (pRenderWindow->pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                pRenderWindow->close();
            else if (event.type == sf::Event::Resized)
                pRenderWindow->clear();
        }

        // pRenderWindow->clear();

        pRenderWindow->draw(pixels);

        pRenderWindow->display();
    }

    void toggleVisibility() {
        pRenderWindow->setVisible(!visibility);
        visibility = !visibility;
        // if(visibility) pRenderWindow->requestFocus();
    }


    ~Window() {
        delete pRenderWindow;
    }
};

// Class representing a player and his movement.
class Player {
    float x, y, angle, speed = 0.005f, horizontalSpeed = 0.002f, angularSpeed = 0.18f;

public:
    Player() {
        // Maze generation algorithm ensures these coordinates are free
        x = 1.5f;
        y = 1.5f;
        angle = 0.f;
    }

    float getX() { return x; }
    void setX(float newX) { x = newX; }
    float getY() { return y; }
    void setY(float newY) { y = newY; }
    float getAngle() { return angle; }

    Player(float start_x, float start_y, float start_angle) {
        x = start_x;
        y = start_y;
        angle = start_angle;
    }

private:
    // move forward and right relative to current position and angle
    void moveRelative(float forwardDistance, float rightDistance, const std::vector<std::vector<int>> &map) {
        float new_x = x, new_y = y;
        new_x += forwardDistance * sinf(degreesToRadians(angle));
        new_y += forwardDistance * cosf(degreesToRadians(angle));
        new_x += rightDistance * sinf(degreesToRadians(angle + 90));
        new_y += rightDistance * cosf(degreesToRadians(angle + 90));

        // Check for collision.
        if (!map[(int)new_y][(int)new_x]) {
            x = new_x;
            y = new_y;
        } else if (!map[(int)y][(int)new_x]) { // Slide on walls instead of stopping
            x = new_x;
        } else if (!map[(int)new_y][(int)x]) {
            y = new_y;
        }
    }

public:
    void movement(float deltaTime, const std::vector<std::vector<int>> &map) {
        // std::cout << angle << "\n";
        // DeltaTime ensures similar real time player speed between different framerates.
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            moveRelative(speed * deltaTime, 0, map);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            moveRelative(-speed * deltaTime, 0, map);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
                moveRelative(0, -horizontalSpeed * deltaTime, map);
            } else {
                angle -= angularSpeed * deltaTime;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
                moveRelative(0, horizontalSpeed * deltaTime, map);
            } else {
                angle += angularSpeed * deltaTime;
            }
        }

        while (angle > 360)
            angle -= 360;
        while (angle < 0)
            angle += 360;
    }
};

// Class representing game logic.
class Game {
    Window *pWindow;
    Player player;
    size_t gameLength, gameHeight, trueLength, trueHeight;
    int fov = 60, helperWindowScale;
    unsigned int rayCastingPrecision = 64;
    Map map;
    Texture sky;
    bool helperVisibility = false;

public:
    Game(size_t length, size_t height, int scale, int maze_x_starting_size, int maze_y_starting_size) {
        trueLength = length;
        trueHeight = height;
        gameLength = length / scale;
        gameHeight = height / scale;
        pWindow = new Window(gameLength, gameHeight, scale, "Maze finder");


        map = Map(maze_x_starting_size, maze_y_starting_size);

        sky = Texture("../textures/skyTexture2P3.ppm");
        // sky = Texture("../textures/starry_night_sky.ppm");

        helperWindowScale = std::max((int)std::min(length / 2 / map.getMap().front().size(), height / 2 / map.getMap().size()), 1); // scale to main window.
    }

    void changeSkyTexture(std::string filePath) {
        sky = Texture(filePath);
    }

    void renderFrameToBuffer() {
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
            float rayCosIncrement = sinf(degreesToRadians(rayAngle)) / (float)rayCastingPrecision;
            float raySinIncrement = cosf(degreesToRadians(rayAngle)) / (float)rayCastingPrecision;

            // Wall collision check.
            bool hitWall = false, hitFromX;
            while (!hitWall) {
                // Necessary for calculating texture orientation.
                hitFromX = mapArray[(int)rayY][(int)(rayX + rayCosIncrement)];

                rayX += rayCosIncrement;
                rayY += raySinIncrement;

                hitWall = mapArray[(int)rayY][(int)rayX];
            }

            // Calculate distance to wall using Pythagoreas theorem
            float distanceToWall = sqrtf(fabsf(playerX - rayX) * fabsf(playerX - rayX) + fabsf(playerY - rayY) * fabsf(playerY - rayY));

            // Fisheye fix approximation.
            distanceToWall *= cosf(degreesToRadians(rayAngle - playerAngle));

            float wallHeight = halfHeight / distanceToWall;

            // Load wall texture (todo - get from map class)
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

            // if(45 < rayAngle && rayAngle < 135) textureVerticalSlipIdx = textureWidth - textureVerticalSlipIdx - 1;

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

    void renderHelperWindowPixel(int x, int y, const sf::Color &color) {
        for (int i = 0; i < helperWindowScale; i++) {
            for (int j = 0; j < helperWindowScale; j++) {
                pWindow->setTruePixelColor(helperWindowScale * x + i, helperWindowScale * y + j, color);
            }
        }
    }

    void renderHelperWindow() {
        if (!helperVisibility) return;
        const std::vector<std::vector<int>> &mapArray = map.getMap();

        for (int i = 0; i < (int)mapArray.front().size(); i++) {
            for (int j = 0; j < (int)mapArray.size(); j++) {
                renderHelperWindowPixel(i, j, map.getTileColor(i, j));
            }
        }

        renderHelperWindowPixel((int)player.getX(), (int)player.getY(), sf::Color::Blue);
    }

    void loadNewMaze() {
        // Includes padding, so the size increases
        map = Map((int)map.getMap().front().size(), (int)map.getMap().size());
        player.setX(1.5f);
        player.setY(1.5f);
        helperWindowScale = std::max((int)std::min(trueLength / 2 / map.getMap().front().size(), trueHeight / 2 / map.getMap().size()), 1); // scale to main window.
        changeSkyTexture("../textures/skyTexture" + std::to_string(rand() % 2 + 1) + "P3.ppm");
    }

    void play() {
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

    ~Game() {
        delete pWindow;
    }
};

int main() {
    std::cout << "works";
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