#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>
#include <chrono>
#include <fstream>

float degreesToRadians(float degrees) {
    return degrees * (float)M_PI / 180.f;
}

class Texture {
    std::vector<std::vector<sf::Color>> colorMap;

    public:
    Texture(std::string filePath) {
        std::ifstream file;
        file.open(filePath);
        int width, height;
        std::string temp;
        unsigned int r, g, b;
        file >> temp >> width >> height >> temp;
        for(int i = 0; i < height; i++) {
            std::vector<sf::Color> currentLine;
            for(int j = 0; j < width; j++) {
                file >> r >> g >> b;
                currentLine.push_back(sf::Color((unsigned char)r, (unsigned char)g, (unsigned char)b, 255));
            }
            colorMap.push_back(currentLine);
            currentLine.clear();
        }
        file.close();
    }

    void print() {
        std::cout << colorMap.size() << std::endl;
        for(std::vector<sf::Color> line : colorMap) {
            for(sf::Color color : line) {
                std::cout << "(" << (unsigned int)color.r << " " << (unsigned int)color.g << " " << (unsigned int)color.b << ")\t";
            }
            std::cout << std::endl;
        }
    }

    size_t getHeight() {return colorMap.size();}
    size_t getWidth() {return colorMap.front().size();}
    const std::vector<std::vector<sf::Color>> & getColorMap() {
        return colorMap;
    }
};

class Window {
    sf::RenderWindow* pRenderWindow;
    size_t LENGTH, HEIGHT;
    sf::VertexArray pixels;
    int scaleModifier;

    bool isEqualFloat(float a, float b) {
        return fabsf(a - b) < 10e-10;
    }

    public:
    Window(size_t length, size_t height, int scale, std::string title) {
        LENGTH = length;
        HEIGHT = height;
        scaleModifier = scale;
        pRenderWindow = new sf::RenderWindow(sf::VideoMode((unsigned int)LENGTH * scaleModifier, (unsigned int)HEIGHT * scaleModifier), title);
        pixels = sf::VertexArray(sf::Points, LENGTH * HEIGHT * scaleModifier * scaleModifier);

        for (size_t y = 0; y < HEIGHT * scaleModifier; y++) {
            for (size_t x = 0; x < LENGTH * scaleModifier; x++) {
                std::cout << y << " " << x << " " << std::endl;
                pixels[y * LENGTH * scaleModifier + x].position = sf::Vector2f((float)(x), (float)(y));
                pixels[y * LENGTH * scaleModifier + x].color = sf::Color::Cyan;
            }
        }
    }

    bool isOpen() {
        return pRenderWindow->isOpen();
    }

    void setTruePixelColor(int x, int y, sf::Color color) {
        if (x < 0 || (size_t)x >= LENGTH * scaleModifier || y < 0 || (size_t)y >= HEIGHT * scaleModifier) return;
        pixels[(HEIGHT * scaleModifier - y - 1) * LENGTH * scaleModifier + x].color = color;
    }
    
    void setPixelColor(int x, int y, sf::Color color) {
        setTruePixelColor(x * scaleModifier, y * scaleModifier, color);
    }

    // void setPixelColor(int x, int y, sf::Color color) {
    //     if (x < 0 || (size_t)x >= LENGTH || y < 0 || (size_t)y >= HEIGHT) return;
    //     pixels[(HEIGHT - y - 1) * LENGTH + x].color = color;
    // }

    void drawLine(float x1, float y1, float x2, float y2, sf::Color color) {
        float incriment, x, y;

        if (!isEqualFloat(x1, x2)) {
            incriment = (y2 - y1) / (x2 - x1);
            y = y1;
            for (x = x1; x <= x2; x++) {
                setPixelColor((int)x, (int)y, color);
                y += incriment;
            }
        }

        if (!isEqualFloat(y1, y2)) {
            x = x1;
            incriment = (x2 - x1) / (y2 - y1);
            for (y = y1; y < y2; y++) {
                setPixelColor((int)x, (int)y, color);
                x += incriment;
            }
        }
    }

    void drawVericalLine(int y1, int y2, int x, sf::Color color) {
        if(y1 > y2) std::swap(y1, y2);
        for(int y = y1; y <= y2; y++){
            setPixelColor(x, y, color);
        }
    }

    void drawTextureVerticalLine(int x, float wallHeight, int texturePositionX, Texture &texture) {
        float texturePixelSize = (2.f * wallHeight) / (float)texture.getHeight();
        float y = (float)(HEIGHT / 2) - wallHeight;
        for(int i = 0; i < (int)texture.getHeight(); i++) {
            drawVericalLine((int)y, (int)y + (int)texturePixelSize, x, texture.getColorMap()[texture.getHeight() - i - 1][texturePositionX]);
            y += texturePixelSize;

        }
    }

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

    ~Window() {
        delete pRenderWindow;
    }
};

class Player {
    float x, y, angle, speed = 0.007f, angularSpeed = 0.13f;

public:
    float getX() { return x; }
    float getY() { return y; }
    float getAngle() { return angle; }

    Player(float start_x, float start_y, float start_angle) {
        x = start_x;
        y = start_y;
        angle = start_angle;
    }

    private:
    void moveRelative(float forwardDistance, float rightDistance, const std::vector<std::vector<int>> &map) {
        float new_x = x, new_y = y;
        new_x += forwardDistance * cosf(degreesToRadians(angle));
        new_y += forwardDistance * sinf(degreesToRadians(angle));
        new_x += rightDistance * cosf(degreesToRadians(angle + 90));
        new_y += rightDistance * sinf(degreesToRadians(angle + 90));

        if(!map[(int)new_y][(int)new_x]) {
            x = new_x;
            y = new_y;
        }
    }


    public:
    void movement(float deltaTime, const std::vector<std::vector<int>> &map) {
        // std::cout << angle << " " << angle + 90 << std::endl;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            moveRelative(speed * deltaTime, 0, map);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            moveRelative(-speed * deltaTime, 0, map);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
                moveRelative(0, -speed * deltaTime, map);
            } else {
                angle -= angularSpeed * deltaTime;
            }
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
                moveRelative(0, speed * deltaTime, map);
            } else {
                angle += angularSpeed * deltaTime;
            }
        }

    }
};

class Game {
    Window* pWindow;
    size_t LENGTH, HEIGHT;
    unsigned int renderDelayMs = 30;
    int fov = 60;
    Player* pPlayer;
    unsigned int rayCastingPrecision = 64;
    std::vector<std::vector<int>> map;



public:
    Game(size_t length, size_t height, int scale) {
        LENGTH = length / scale;
        HEIGHT = height / scale;
        pWindow = new Window(LENGTH, HEIGHT, scale, "test");
        pPlayer = new Player(8, 2, 0);

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
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        };
    }

    void rayCasting() {
        float playerX = pPlayer->getX(), playerY = pPlayer->getY();
        float rayAngle = pPlayer->getAngle() - (float)fov / 2.f;
        float incrimentAngle = (float)fov / (float)LENGTH;
        float halfHeight = (float)HEIGHT / 2.f;
        for (unsigned int rayCount = 0; rayCount < LENGTH; rayCount++) {
            float rayX = playerX, rayY = playerY;
            float rayCosIncrement = cosf(degreesToRadians(rayAngle)) / (float)rayCastingPrecision;
            float raySinIncrement = sinf(degreesToRadians(rayAngle)) / (float)rayCastingPrecision;

            bool hitWall = false;
            while (!hitWall) {
                rayX += rayCosIncrement;
                rayY += raySinIncrement;
                hitWall = map[(int)rayY][(int)rayX];
            }

            float distanceToWall = sqrtf(fabsf(playerX - rayX) * fabsf(playerX - rayX) + fabsf(playerY - rayY) * fabsf(playerY - rayY));
            distanceToWall *= cosf(degreesToRadians(rayAngle - pPlayer->getAngle()));
            float wallHeight = (halfHeight / distanceToWall);

            Texture t("texture2.ppm");
            int texturePositionX = (int)((float)t.getWidth() * (rayX + rayY)) % (int)t.getWidth();
            // One of rayX and rayY is close to edge

            // Draw lines.
            // pWindow->drawLine((float)rayCount, 0, (float)rayCount, halfHeight, sf::Color::Green);
            // pWindow->drawLine((float)rayCount, halfHeight, (float)rayCount, (float)HEIGHT, sf::Color::Blue);
            // pWindow->drawLine((float)rayCount, halfHeight - wallHeight, (float)rayCount, halfHeight + wallHeight, sf::Color(100, 62, 10, 100));
            pWindow->drawVericalLine(0, (int)halfHeight, rayCount, sf::Color::Green);
            pWindow->drawVericalLine((int)HEIGHT, (int)halfHeight, rayCount, sf::Color::Blue);
            pWindow->drawVericalLine((int)(halfHeight - wallHeight), (int)(halfHeight + wallHeight), rayCount, sf::Color(100, 62, 10, 100));
            pWindow->drawTextureVerticalLine(rayCount, wallHeight, texturePositionX, t);
            rayAngle += incrimentAngle;
        }
    }

    void play() {
        std::chrono::_V2::system_clock::time_point endOfPrevLoop = std::chrono::high_resolution_clock::now();
        while (pWindow->isOpen()) {
            rayCasting();
            pWindow->display();
            // std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - endOfPrevLoop).count() << std::endl;
            pPlayer->movement((float)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - endOfPrevLoop).count(), map);
            endOfPrevLoop = std::chrono::high_resolution_clock::now();
        }
    }

    ~Game() {
        delete pWindow;
        delete pPlayer;
    }
};

int main() {
    size_t LENGTH = 600, HEIGHT = 240;

    Game game(LENGTH, HEIGHT, 2);
    game.play();

    return 0;
}