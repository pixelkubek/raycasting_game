#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <cmath>
#include <vector>

class Window {
    sf::RenderWindow* pRenderWindow;
    size_t LENGTH, HEIGHT;
    sf::VertexArray pixels;

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

    void setPixelColor(size_t x, size_t y, sf::Color color) {
        pixels[y * LENGTH + x].color = color;
    }

    void drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, sf::Color color){
        
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
            {1,1,1,1,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,1,1,0,1,0,0,1},
            {1,0,0,1,0,0,1,0,0,1},
            {1,0,0,1,0,0,1,0,0,1},
            {1,0,0,1,0,1,1,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,1,1,1,1,1,1,1,1,1},
        };
    }

    void play() {
        while(pWindow->isOpen()){
            pWindow->setPixelColor(100, 100, sf::Color(rand()));
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