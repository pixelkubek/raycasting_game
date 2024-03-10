#ifndef playerH
#define playerH

#include <vector>

// Class representing a player and his movement.
class Player {
    float x, y, angle, speed = 0.005f, horizontalSpeed = 0.002f, angularSpeed = 0.18f;

public:
    Player();

    float getX();
    void setX(float newX);
    float getY();
    void setY(float newY);
    float getAngle();

    Player(float start_x, float start_y, float start_angle);

private:
    // move forward and right relative to current position and angle
    void moveRelative(float forwardDistance, float rightDistance, const std::vector<std::vector<int>> &map);
    float degreesToRadians(float degrees);

public:
    void movement(float deltaTime, const std::vector<std::vector<int>> &map);
};

#endif