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
    // Centre la caméra sur le joueur
    x = player.getCenterX() - (VIRTUAL_WIDTH / 2.0f);
    // les limites sont appliquées dans GamePlayState::update()
}