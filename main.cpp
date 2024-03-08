#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

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
        pixels[(HEIGHT - y) * LENGTH + x].color = color;
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
    float x, y, angle;

    public:
    float getX() {return x;}
    float getY() {return y;}
    float getAngle() {return angle;}

public:
    Player(float start_x, float start_y, float start_angle) {
        x = start_x;
        y = start_y;
        angle = start_angle;
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

    float degreesToRadians(float degrees) {
        return degrees * (float)M_PI / 180;
    }

public:
    Game(size_t length, size_t height) {
        LENGTH = length;
        HEIGHT = height;
        pWindow = new Window(LENGTH, HEIGHT, "test");

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
        float rayAngle = pPlayer->getAngle() - (float)fov / 2.f;
        float incrimentAngle = (float)fov / (float)LENGTH;
        for(unsigned int rayCount = 0; rayCount < LENGTH; rayCount++) {

            rayAngle += incrimentAngle;
        }
    }

    void play() {
        while (pWindow->isOpen()) {
            pWindow->drawLine(100, 100, 100, 200, sf::Color::Black);
            pWindow->display();
        }
    }

    ~Game() {
        delete pWindow;
    }
};

int main() {
    size_t LENGTH = 800, HEIGHT = 600;

    Game game(LENGTH, HEIGHT);
    game.play();

    return 0;
}