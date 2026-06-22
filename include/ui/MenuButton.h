#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class MenuBtnState { Idle, Hover, Active };

/// Botón de menú principal con efecto bisel 3D (sin hoja de sprites).
class MenuButton {
public:
    MenuButton(float x, float y, float w, float h, sf::Font* font, const std::string& label);

    bool isPressed() const;
    void setAlpha(uint8_t a);
    void update(const sf::Vector2i& mousePos);
    void render(sf::RenderTarget& target);

private:
    MenuBtnState state;
    float baseX, baseY, w, h;
    uint8_t alpha;

    sf::VertexArray body; // Quads: TL TR BR BL — gradiente vertical
    sf::RectangleShape bTop, bBot, bLeft, bRight;
    sf::Text text;

    void rebuild();
    void recolor();
};
