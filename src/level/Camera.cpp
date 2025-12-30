#include "level/Camera.hpp"

Camera::Camera(/* args */)
{
}

Camera::~Camera()
{
}

void Camera::focusPlayer()
{
    // TODO: Implémenter le suivi du joueur
}


//==== Accesseur ====

float Camera::getX()
{
    return x;
}

void Camera::setX(float value)
{
    x = value;
}

float Camera::getY()
{
    return y;
}

void Camera::setY(float value)
{
    y = value;
}