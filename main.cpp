#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>
#include <chrono>

float degreesToRadians(float degrees) {
    return degrees * (float)M_PI / 180.f;
}

class Window {
    sf::RenderWindow* pRenderWindow;
    size_t LENGTH, HEIGHT;
    sf::VertexArray pixels;

    bool isEqualFloat(float a, float b) {
        return fabsf(a - b) < 10e-10;
    }

    public:
    Window(size_t length, size_t height, std::string title) {
        LENGTH = length;
        HEIGHT = height;
        pRenderWindow = new sf::RenderWindow(sf::VideoMode((unsigned int)LENGTH, (unsigned int)HEIGHT), title);
        pixels = sf::VertexArray(sf::Points, LENGTH * HEIGHT);

        for (size_t y = 0; y < HEIGHT; y++) {
            for (size_t x = 0; x < LENGTH; x++) {
                pixels[y * LENGTH + x].position = sf::Vector2f((float)x, (float)y);
            }
        }
    }

    bool isOpen() {
        return pRenderWindow->isOpen();
    }

    void setPixelColor(int x, int y, sf::Color color) {
        if (x < 0 || (size_t)x >= LENGTH || y < 0 || (size_t)y >= HEIGHT) return;
        pixels[(HEIGHT - y - 1) * LENGTH + x].color = color;
    }

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

    void movement(float deltaTime) {
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            x += speed * cosf(degreesToRadians(angle)) * deltaTime;
            y += speed * sinf(degreesToRadians(angle)) * deltaTime;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            x -= speed * cosf(degreesToRadians(angle)) * deltaTime;
            y -= speed * sinf(degreesToRadians(angle)) * deltaTime;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            angle -= angularSpeed * deltaTime;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            angle += angularSpeed * deltaTime;
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
    Game(size_t length, size_t height) {
        LENGTH = length;
        HEIGHT = height;
        pWindow = new Window(LENGTH, HEIGHT, "test");
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

            // Draw lines.
            // pWindow->drawLine((float)rayCount, 0, (float)rayCount, halfHeight, sf::Color::Green);
            // pWindow->drawLine((float)rayCount, halfHeight, (float)rayCount, (float)HEIGHT, sf::Color::Blue);
            // pWindow->drawLine((float)rayCount, halfHeight - wallHeight, (float)rayCount, halfHeight + wallHeight, sf::Color(100, 62, 10, 100));
            pWindow->drawVericalLine(0, (int)halfHeight, rayCount, sf::Color::Green);
            pWindow->drawVericalLine((int)HEIGHT, (int)halfHeight, rayCount, sf::Color::Blue);
            pWindow->drawVericalLine((int)(halfHeight - wallHeight), (int)(halfHeight + wallHeight), rayCount, sf::Color(100, 62, 10, 100));
            rayAngle += incrimentAngle;
        }
    }

    void play() {
        std::chrono::_V2::system_clock::time_point endOfPrevLoop = std::chrono::high_resolution_clock::now();
        while (pWindow->isOpen()) {
            rayCasting();
            pWindow->display();
            // std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - endOfPrevLoop).count() << std::endl;
            pPlayer->movement((float)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - endOfPrevLoop).count());
            endOfPrevLoop = std::chrono::high_resolution_clock::now();
        }
    }

    ~Game() {
        delete pWindow;
        delete pPlayer;
    }
};

int main() {
    size_t LENGTH = 1280, HEIGHT = 720;

    Game game(LENGTH, HEIGHT);
    game.play();

    return 0;
}