#include "core/Game.h"

#include <iostream>
#include <stdexcept>

/*
    SimplyChess v 0.7 (Beta)
    Fecha de creación: 03/09/2020
*/

int main() {
    // Crea y ejecuta el juego, capturando errores de inicialización (carga de recursos).
    try {
        Game game;
        game.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error fatal: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
