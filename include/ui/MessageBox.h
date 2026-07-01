#pragma once

#include <SFML/Graphics.hpp>
#include <chrono>

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
    std::unique_ptr<MenuButton> button2;

    bool waitingForRelease;
    std::chrono::steady_clock::time_point lastUpdateTime;

    /// Centra el texto dentro del cuadro.
    void centerText();

public:
    // Constructor con un boton (compatible con el uso anterior)
    MessageBox(sf::Font& font, std::string message, std::string buttonText);
    // Constructor con dos botones
    MessageBox(sf::Font& font, std::string message, std::string button1Text, std::string button2Text);
    virtual ~MessageBox();

    /// Devuelve true si se ha pulsado el primer boton (o el boton de aceptar).
    bool isButtonPressed();
    /// Devuelve true si se ha pulsado el segundo boton (solo en modo 2 botones).
    bool isButton2Pressed();

    /// Cambia el texto del mensaje y lo recentra.
    void setText(std::string text);

    /// Actualiza los botones segun la posicion del raton.
    void update(sf::Vector2i& mousePosWindow);
    /// Dibuja el cuadro, el texto y los botones.
    void render(sf::RenderTarget& target);
};
