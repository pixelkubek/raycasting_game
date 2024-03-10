#include "window.h"

Window::Window(size_t length, size_t height, int scale, std::string title) {
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

bool Window::isOpen() {
    return pRenderWindow->isOpen();
}

// Set the color of pixel in coords x, y
void Window::setTruePixelColor(int x, int y, const sf::Color &color) {
    if (x < 0 || (size_t)x >= trueLength || y < 0 || (size_t)y >= trueHeight) return;

    // Flip vertical orientation.
    pixels[(trueHeight - y - 1) * trueLength + x].color = color;
}

// Set the color of a simulation pixel
void Window::setGamePixelColor(int x, int y, sf::Color color) {
    // Sets a square od sides scaleModifier of real pixels.
    for (int i = 0; i < scaleModifier; i++) {
        for (int j = 0; j < scaleModifier; j++) {
            setTruePixelColor(x * scaleModifier + i, y * scaleModifier + j, color);
        }
    }
}

// Draw a vertical line.
void Window::drawVericalLine(int y1, int y2, int x, const sf::Color &color) {
    if (y1 > y2) std::swap(y1, y2);
    for (int y = std::max(y1, 0); y <= std::min(y2, (int)gameHeight); y++) {
        setGamePixelColor(x, y, color);
    }
}

// Draws a vertical strip of a texture, texturePositionX determines which.
void Window::drawTextureVerticalLine(int x, float y1, float y2, int texturePositionX, Texture &texture) {
    float texturePixelSize = (y2 - y1 + 1) / (float)texture.getHeight();

    float y = y1;

    for (int i = 0; i < (int)texture.getHeight(); i++) {
        Window::drawVericalLine((int)round(y), (int)ceil(y + texturePixelSize), x, texture.getColorMap()[texture.getHeight() - i - 1][texturePositionX]);
        y += texturePixelSize;
    }
}

// Must be a separate function to avoid float precision artifacts at half height
void Window::drawWallTextureVerticalLine(int x, float wallHeight, int texturePositionX, Texture &texture) {
    float texturePixelSize = (2.f * wallHeight) / (float)texture.getHeight();
    float y = (float)(gameHeight / 2) - wallHeight;

    for (int i = 0; i < (int)texture.getHeight(); i++) {
        drawVericalLine((int)round(y), (int)ceil(y + texturePixelSize), x, texture.getColorMap()[texture.getHeight() - i - 1][texturePositionX]);
        y += texturePixelSize;
    }
}

// Push updates to display
void Window::display() {
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

void Window::toggleVisibility() {
    pRenderWindow->setVisible(!visibility);
    visibility = !visibility;
    // if(visibility) pRenderWindow->requestFocus();
}

Window::~Window() {
    delete pRenderWindow;
}
