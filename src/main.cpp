 #include "Game.h"
         
/*
    SimplyChess v 0.7 (Beta)
    Fecha de creación: 03/09/2020
    Fecha de compilación: 01/02/2020
    Tiempo de programación: 30h 30m
       
    Errores:
       - 
    WARNING!
        - La gridPos y la Pos van cambiadas x e y
        - Al comer pieza deberiamos borrar el sprite, no moverlo a -100, -100 
       
     Futuras actualizaciones (versión 1.0)  
        - Tiempo
        - Ajustes
        - Funciones startGame() y endGame()
        - Entrada por teclado de los nombres (nuevo elemento de interfaz)
        - Jaques (en proceso)
        - Historial
        - Movimientos oficiales (caja de texto)
        - Logo
        - Multilenguaje
        - Multi coop
        - Distintas skins
        - Distribución inicial random
*/

int main() {
    // Creates and runs the game
    Game game;
    game.run();

    return 0;
}
