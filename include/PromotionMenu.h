#pragma once

#include <SFML/Graphics.hpp>

class PromotionMenu {

private:
    sf::Texture& texture;
    sf::Sprite selector;

    bool shown;
  

public:

    PromotionMenu(sf::Texture& texture);

    const bool isPressed(sf::Vector2i& mousePos) const;
    unsigned short int getSelectedPiece(sf::Vector2i mousePos);

    sf::Texture& getTexture();

    bool isShown();

    void setShown(bool shown, bool color);

    void setPosition(float x, float y);

    void update();

    void render(sf::RenderTarget& target);
};

    