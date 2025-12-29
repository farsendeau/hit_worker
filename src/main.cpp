#include <iostream>
#include <string>
#include <array>
#include <ctime>
#include <memory>
#include "../include/core/Game.hpp"

int main()
{
    std::unique_ptr<Game> game{new Game()};

    if (!game->init()) {
        return 1;
    }

    game->run();

    return 0;
}