#pragma once
   
#include <SFML/Graphics.hpp>

#include "Button.h"
#include <memory>

class MessageBox {  

private:
    sf::Font& font;
    sf::Text messageText;
    sf::RectangleShape container;
    std::unique_ptr<Button> button;

    void centerText();

public:
    MessageBox(sf::Font& font, std::string message, std::string buttonText, sf::Texture& buttonTexture);
    virtual ~MessageBox();

    bool isButtonPressed();

    void setText(std::string text);

    void update(sf::Vector2i& mousePosWindow);
    void render(sf::RenderTarget& target);
};

      