#pragma once

#include <SFML/Graphics.hpp>

#include "chess/Piece.h"

/// Menú de coronación: muestra las piezas elegibles al promocionar un peón
/// y devuelve la seleccionada.
class PromotionMenu {

private:
    sf::Texture& texture;
    sf::Sprite selector;

    bool shown;


public:

    PromotionMenu(sf::Texture& texture);

    /// Devuelve true si se ha hecho clic sobre el menú.
    bool isPressed(sf::Vector2i& mousePos) const;
    /// Devuelve el tipo de pieza elegido según la posición del clic.
    PieceType getSelectedPiece(sf::Vector2i mousePos);

    /// Indica si el menú está visible.
    bool isShown();

    /// Muestra u oculta el menú; 'color' elige la fila de piezas (blancas/negras).
    void setShown(bool shown, bool color);

    /// Coloca el menú en la posición indicada.
    void setPosition(float x, float y);

    /// Dibuja el menú si está visible.
    void render(sf::RenderTarget& target);
};
