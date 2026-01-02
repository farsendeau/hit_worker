#ifndef CAMERA_HPP
#define CAMERA_HPP

class Player;

class Camera
{
    private:
        float x{};
        float y{};
    public:
        Camera(/* args */);
        ~Camera();

        void follow(const Player& player); // Centre la camera sur le player

        // Accesseur
        float getX() { return x; }
        void setX(float value) { x = value; }
        float getY() { return y; }
        void setY(float value) { y = value; }
    };

#endif // CAMERA_HPP