#pragma once
#include <map>
#include <string>

#include <SFML/Graphics.hpp>
      
#include "Button.h"
#include <memory>

class PauseMenu {

private:
    sf::Font& font;
    sf::Text menuText;  

    sf::RectangleShape container;
    std::map<std::string, std::unique_ptr<Button>> buttons;

public:

    PauseMenu(sf::Font& font);
    virtual ~PauseMenu();


    bool isButtonPressed(std::string key);
    void addButton(std::string key, float x, float y, std::string text, sf::Texture& buttonTexture);

    void update(sf::Vector2i& mousePosWindow);
    void render(sf::RenderTarget& target);
};