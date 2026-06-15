#include "ui/PromotionMenu.h"
#include "chess/Piece.h"

void PromotionMenu::setPosition(float x, float y) {
    this->selector.setPosition(x, y);

}

PromotionMenu::PromotionMenu(sf::Texture& texture) : texture(texture) {
    this->shown = false;
    this->selector.setTexture(this->texture);
}



bool PromotionMenu::isPressed(sf::Vector2i& mousePos) const {

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (this->selector.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
            return true;
        }
    }

    return false;
}

PieceType PromotionMenu::getSelectedPiece(sf::Vector2i mousePos) {
    mousePos.x -= this->selector.getPosition().x;
    mousePos.y -= this->selector.getPosition().y;
    if (mousePos.y <= 50) return PieceType::REINA;
    else if (mousePos.y <= 100) return PieceType::CABALLO;
    else if (mousePos.y <= 150) return PieceType::ALFIL;
    return PieceType::TORRE;
}

bool PromotionMenu::isShown()
{
    return shown;
}

void PromotionMenu::setShown(bool shown, bool color)
{
    this->shown = shown;
    if (color) {
        this->selector.setTextureRect(sf::IntRect(0, 0, 50, 200));
    }
    else {
        this->selector.setTextureRect(sf::IntRect(50, 0, 50, 200));
    }

}

void PromotionMenu::render(sf::RenderTarget& target)
{
    if(this->shown) target.draw(this->selector);

}
