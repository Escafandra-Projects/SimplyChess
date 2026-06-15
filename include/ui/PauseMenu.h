#pragma once
#include <map>
#include <string>

#include <SFML/Graphics.hpp>

#include "ui/Button.h"
#include <memory>

/// Menú de pausa: un contenedor con un conjunto de botones indexados por clave.
class PauseMenu {

private:
    sf::Font& font;
    sf::Text menuText;

    sf::RectangleShape container;
    std::map<std::string, std::unique_ptr<Button>> buttons;

public:

    PauseMenu(sf::Font& font);
    virtual ~PauseMenu();


    /// Devuelve true si el botón identificado por 'key' está pulsado.
    bool isButtonPressed(std::string key);
    /// Añade un botón al menú con la clave y posición dadas.
    void addButton(std::string key, float x, float y, std::string text, sf::Texture& buttonTexture);

    /// Actualiza todos los botones según la posición del ratón.
    void update(sf::Vector2i& mousePosWindow);
    /// Dibuja el contenedor, los botones y el título.
    void render(sf::RenderTarget& target);
};
