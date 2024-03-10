#ifndef gameH
#define gameH

#include "window.h"
#include "player.h"
#include "map.h"

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
    Game(size_t length, size_t height, int scale, int maze_x_starting_size, int maze_y_starting_size);

    void changeSkyTexture(std::string filePath);

    void renderFrameToBuffer();

    void renderHelperWindowPixel(int x, int y, const sf::Color &color);

    void renderHelperWindow();

    void loadNewMaze();

    void play();

    ~Game();

private:
    float degreesToRadians(float degrees);
};

#endif