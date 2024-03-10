#include "player.h"

#include<SFML/Graphics.hpp>
#include<cmath>
#include<iostream>

Player::Player() {
    // Maze generation algorithm ensures these coordinates are free
    x = 1.5f;
    y = 1.5f;
    angle = 0.f;
}

float Player::getX() { return x; }
void Player::setX(float newX) { x = newX; }
float Player::getY() { return y; }
void Player::setY(float newY) { y = newY; }
float Player::getAngle() { return angle; }

Player::Player(float start_x, float start_y, float start_angle) {
    x = start_x;
    y = start_y;
    angle = start_angle;
}

// move forward and right relative to current position and angle
void Player::moveRelative(float forwardDistance, float rightDistance, const std::vector<std::vector<int>> &map) {
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

void Player::movement(float deltaTime, const std::vector<std::vector<int>> &map) {
    // std::cout << angle << " " << x << " " << y << "\n";
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

float Player::degreesToRadians(float degrees) {
    return degrees * (float)M_PI / 180.f;
}
