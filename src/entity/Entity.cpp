#include "entity/Entity.hpp"

Entity::Entity(float startX, float startY, float initWidth, float initHeight, float initVelocityX)
: x(startX), y(startY), width(initWidth), height(initHeight), velocityX(initVelocityX)
{
}