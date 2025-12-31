#include "level/Camera.hpp"
#include "utils/constant.h"
#include "entity/Player.hpp"

Camera::Camera(/* args */)
{
}

Camera::~Camera()
{
}

void Camera::follow(const Player& player)
{
    // Centre sur le joueur
    x = player.getX() - (VIRTUAL_WIDTH / 2.0f) + (player.getWidth() / 2.0f);
    // les limits sont appliquées dans GamePlaySate::update()
}