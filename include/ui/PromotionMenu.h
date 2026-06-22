#pragma once

#include <SFML/Graphics.hpp>
#include <array>

#include "chess/Piece.h"

enum class OptionState { Idle, Hover, Active };

struct PromotionOption {
    PieceType type;
    sf::VertexArray body;
    sf::RectangleShape bTop, bBot, bLeft, bRight;
    sf::Sprite icon;
    OptionState state;
};

/// Menú de coronación: muestra las piezas elegibles al promocionar un peón
/// y devuelve la seleccionada.
class PromotionMenu {

private:
    sf::RectangleShape container;
    sf::RectangleShape innerFrame;
    sf::Font font;
    sf::Text headerText;

    std::array<PromotionOption, 4> options;

    sf::Vector2f basePosition;
    bool shown;
    bool currentColor;

    void rebuildOption(int index);
    void recolorOption(int index);

public:

    PromotionMenu(sf::Texture& texture);

    /// Actualiza el estado de hover/active de las opciones.
    void update(const sf::Vector2i& mousePos);

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

