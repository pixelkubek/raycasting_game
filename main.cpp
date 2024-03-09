#include <SFML/Graphics.hpp>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

float degreesToRadians(float degrees) {
    return degrees * (float)M_PI / 180.f;
}

class Texture {
    std::vector<std::vector<sf::Color>> colorMap;

public:
    Texture() {
        colorMap.push_back({sf::Color::Black});
    }
    // Load texture from P3 .ppm file.
    Texture(std::string filePath) {
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
    void print() {
        std::cout << colorMap.size() << std::endl;
        for (std::vector<sf::Color> line : colorMap) {
            for (sf::Color color : line) {
                std::cout << "(" << (unsigned int)color.r << " " << (unsigned int)color.g << " " << (unsigned int)color.b << ")\t";
            }
            std::cout << std::endl;
        }
    }

    size_t getHeight() { return colorMap.size(); }
    size_t getWidth() { return colorMap.front().size(); }
    const std::vector<std::vector<sf::Color>> &getColorMap() {
        return colorMap;
    }
};

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
        setWallTexture("myTexture4.ppm");
        setEntranceTexture("entranceTextureP3.ppm");
        setExitTexture("exitTextureP3.ppm");
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
        switch (map[y][x])
        {
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
        map[0][1] = 3; // Set entrance;

        setWallTexture("myTexture4.ppm");
        setEntranceTexture("entranceTextureP3.ppm");
        setExitTexture("exitTextureP3.ppm");
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

    bool isEqualFloat(float a, float b) {
        return fabsf(a - b) < 10e-10;
    }

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

    // Draw line between two points.
    void drawLine(float x1, float y1, float x2, float y2, const sf::Color &color) {
        // todo handle different orders of pionts
        float incriment, x, y;

        if (!isEqualFloat(x1, x2)) {
            incriment = (y2 - y1) / (x2 - x1);
            y = y1;
            for (x = x1; x <= x2; x++) {
                setGamePixelColor((int)x, (int)y, color);
                y += incriment;
            }
        }

        if (!isEqualFloat(y1, y2)) {
            x = x1;
            incriment = (x2 - x1) / (y2 - y1);
            for (y = y1; y <= y2; y++) {
                setGamePixelColor((int)x, (int)y, color);
                x += incriment;
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
    void drawTextureVerticalLine(int x, float wallHeight, int texturePositionX, Texture &texture) {
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
        }

        pRenderWindow->clear();

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
    void setX(float newX) {x = newX;}
    float getY() { return y; }
    void setY(float newY) {y = newY;}
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
        } else if (!map[(int)y][(int)new_x]){
           x = new_x;
        } else if (!map[(int)new_y][(int)x]){
           y = new_y;
        }
    }

public:
    void movement(float deltaTime, const std::vector<std::vector<int>> &map) {
        // DeltaTime ensures similar real time player speed with different framerates.
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

        while(angle > 360) angle -= 360;
        while(angle < 0) angle += 360;
    }
};

// Class representing game logic.
class Game {
    Window *pWindow, *pHelperWindow;
    Player player;
    size_t LENGTH, HEIGHT;
    int fov = 60, maze_x, maze_y;
    unsigned int rayCastingPrecision = 64;
    Map map;

public:
    Game(size_t length, size_t height, int scale, int maze_x_size, int maze_y_size) {
        LENGTH = length / scale;
        HEIGHT = height / scale;
        pWindow = new Window(LENGTH, HEIGHT, scale, "Maze finder");
        maze_x = maze_x_size;
        maze_y = maze_y_size;

        map = Map(maze_x, maze_y);
        // map.print();

        int helperWindowScale = std::max((int)std::max(length / map.getMap().front().size(), height / map.getMap().size()), 4); // scale to main window.
        helperWindowScale /= 4;

        pHelperWindow = new Window((int)map.getMap().front().size(), (int)map.getMap().size(), helperWindowScale, "Helper");
        pHelperWindow->toggleVisibility();
    }

    void renderFrameToBuffer() {
        // For each pixel in screen width cast a ray to a corresponding angle in fov range and extend
        // until it hits a wall, then calculate necessary wall height

        // Use getters only once
        float playerX = player.getX(), playerY = player.getY(), playerAngle = player.getAngle();

        // Rays range from playerAngle - (fov / 2) to playerAngle + (fov / 2)
        float rayAngle = playerAngle - (float)fov / 2.f;

        float incrimentAngle = (float)fov / (float)LENGTH;

        float halfHeight = (float)HEIGHT / 2.f;

        const std::vector<std::vector<int>> &mapArray = map.getMap();

        for (unsigned int rayCount = 0; rayCount < LENGTH; rayCount++) {
            float rayX = playerX, rayY = playerY; // Ray starts from the player.

            // Calculate ray increments in x, y coordinates
            float rayCosIncrement = sinf(degreesToRadians(rayAngle)) / (float)rayCastingPrecision;
            float raySinIncrement = cosf(degreesToRadians(rayAngle)) / (float)rayCastingPrecision;

            // Wall collision check.
            bool hitWall = false;
            while (!hitWall) {
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
            if (fabsf(rayX - roundf(rayX)) > fabsf(rayY - roundf(rayY))) {
                textureVerticalSlipIdx = (int)roundf((float)textureWidth * rayX) % textureWidth;
            } else {
                textureVerticalSlipIdx = (int)roundf((float)textureWidth * rayY) % textureWidth;
            }

            // Draw floor.
            pWindow->drawVericalLine(0, (int)halfHeight, rayCount, sf::Color(121, 121, 121, 255));

            // Draw sky.
            pWindow->drawVericalLine((int)HEIGHT, (int)halfHeight, rayCount, sf::Color(0, 199, 199, 255));

            // Draw untextured walls.
            // pWindow->drawVericalLine((int)(halfHeight - wallHeight), (int)(halfHeight + wallHeight), rayCount, sf::Color(100, 62, 10, 100));

            // Draw textures.
            pWindow->drawTextureVerticalLine(rayCount, wallHeight, textureVerticalSlipIdx, t);

            rayAngle += incrimentAngle;
        }
    }

    void renderHelperWindow() {
        const std::vector<std::vector<int>> &mapArray = map.getMap();

        for(int i = 0; i < (int)mapArray.front().size(); i++) {
            for(int j = 0; j < (int)mapArray.size(); j++) {
                pHelperWindow->setGamePixelColor(i, j, map.getTileColor(i, j));
            }
        }

        pHelperWindow->setGamePixelColor((int)player.getX(), (int)player.getY(), sf::Color::Blue);
    }

    void loadNewMaze() {
        map = Map(maze_x, maze_y);
        player.setX(1.5f);
        player.setY(1.5f);
    }

    void play() {
        // Used for calculating deltaTime
        std::chrono::_V2::system_clock::time_point endOfPrevLoop = std::chrono::high_resolution_clock::now();
        std::chrono::_V2::system_clock::time_point spacePress = std::chrono::high_resolution_clock::now();
        const std::vector<std::vector<int>> &mapArray = map.getMap();

        // Game loop.
        while (pWindow->isOpen()) {
            renderFrameToBuffer();

            renderHelperWindow();

            pWindow->display(); // Display buffer.
            pHelperWindow->display();

            // Provide time delta between frames
            player.movement((float)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - endOfPrevLoop).count(), mapArray);

            if(player.getX() >= (float)maze_x && player.getY() >= (float)maze_y + 0.7f)
                loadNewMaze();

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - spacePress).count() > 300) {
                pHelperWindow->toggleVisibility();
                spacePress = std::chrono::high_resolution_clock::now();
            }
            
            endOfPrevLoop = std::chrono::high_resolution_clock::now();
        }
    }

    ~Game() {
        delete pWindow;
        delete pHelperWindow;
    }
};

int main() {
    srand((unsigned int)time(NULL));
    std::ifstream options;
    size_t LENGTH = 1280, HEIGHT = 720, SCALE = 3, MAZE_WIDTH = 11, MAZE_HEIGHT = 11;
    std::string temp;

    options.open("settings.txt") ;
    options >> temp >> LENGTH >> temp >> HEIGHT >> temp >> SCALE >> temp >> MAZE_WIDTH >> temp >> MAZE_HEIGHT;

    options.close();

    Game game(LENGTH, HEIGHT, (int)SCALE, (int)MAZE_WIDTH, (int)MAZE_HEIGHT);
    game.play();

    return 0;
}