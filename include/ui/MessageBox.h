#pragma once

#include <SFML/Graphics.hpp>

#include "ui/MenuButton.h"
#include "ui/WoodPanel.h"
#include <memory>

/// Cuadro de mensaje con texto y un botón (p. ej. el aviso de fin de partida).
class MessageBox {

private:
    sf::Font& font;
    sf::Text titleText;
    sf::Text bodyText;
    WoodPanel container;
    std::unique_ptr<MenuButton> button;

    /// Centra el texto dentro del cuadro.
    void centerText();

public:
    MessageBox(sf::Font& font, std::string message, std::string buttonText);
    virtual ~MessageBox();

    /// Devuelve true si se ha pulsado el botón.
    bool isButtonPressed();

    /// Cambia el texto del mensaje y lo recentra.
    void setText(std::string text);

    /// Actualiza el botón según la posición del ratón.
    void update(sf::Vector2i& mousePosWindow);
    /// Dibuja el cuadro, el texto y el botón.
    void render(sf::RenderTarget& target);
};
