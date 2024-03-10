#ifndef windowH
#define windowH

#include <SFML/Graphics.hpp>
#include <cmath>

#include "texture.h"

// Rendering window class.
class Window {
    sf::RenderWindow *pRenderWindow;
    size_t gameLength, gameHeight, trueHeight, trueLength;
    sf::VertexArray pixels;
    int scaleModifier;
    bool visibility = true;

public:
    Window(size_t length, size_t height, int scale, std::string title) ;

    bool isOpen();

    // Set the color of pixel in coords x, y
    void setTruePixelColor(int x, int y, const sf::Color &color);

    // Set the color of a simulation pixel
    void setGamePixelColor(int x, int y, sf::Color color);

    // Draw a vertical line.
    void drawVericalLine(int y1, int y2, int x, const sf::Color &color);

    // Draws a vertical strip of a texture, texturePositionX determines which.
    void drawTextureVerticalLine(int x, float y1, float y2, int texturePositionX, Texture &texture);

    // Must be a separate function to avoid float precision artifacts at half height
    void drawWallTextureVerticalLine(int x, float wallHeight, int texturePositionX, Texture &texture);

    // Push updates to display
    void display();

    void toggleVisibility();

    ~Window();
};
#endif