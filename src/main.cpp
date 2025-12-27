#include <iostream>
#include <string>
#include <array>
#include <ctime>
#include <memory>
#include "../include/core/Game.hpp"

void erreur(const std::string &text);

int main()
{
    std::unique_ptr<Game> game{new Game()};

    if (!game->init()) {
        return 1;
    }

    game->run();

    return 0;
}


void erreur(const std::string &text)
{
    ALLEGRO_DISPLAY *d;
    d = al_is_system_installed() ? al_get_current_display() : nullptr;
    al_show_native_message_box(d, "ERREUR", text.c_str(), NULL, NULL, 0);

    // Nettoyage avant de quitter
    al_uninstall_system();  // Nettoie toutes les ressources Allegro

    exit(EXIT_FAILURE);
}