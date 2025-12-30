#ifndef CAMERA_HPP
#define CAMERA_HPP

class Camera
{
    private:
        float x{};
        float y{};
    public:
        Camera(/* args */);
        ~Camera();

        void focusPlayer(); // Centre la camera sur le player

        // Accesseur
        float getX();
        void setX(float value);
        float getY();
        void setY(float value);
        
    };

#endif // CAMERA_HPP