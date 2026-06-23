#pragma once
#include <SFML/Graphics.hpp>

/// Panel de madera oscura reutilizable: gradiente vertical (5 paradas) + veta
/// sutil + borde oscuro + marco interior dorado. Mismo aspecto en todas las
/// pantallas (menú, partida, ajustes, pausa, fin de partida, coronación).
class WoodPanel {
public:
    // Paleta única del tema (referencia de diseño "dark wood").
    static const sf::Color BORDER;   // borde exterior 3px
    static const sf::Color INNER;    // marco interior dorado 1px

    WoodPanel() = default;
    explicit WoodPanel(sf::FloatRect bounds);

    void setBounds(sf::FloatRect bounds);
    /// Escala la opacidad de todo el panel (para el fade-in del menú).
    void setAlpha(std::uint8_t a);

    void render(sf::RenderTarget& target) const;

private:
    sf::FloatRect bounds_;
    sf::VertexArray gradient_{sf::Quads};
    sf::VertexArray grain_{sf::Lines};
    sf::RectangleShape border_;
    sf::RectangleShape inner_;
    std::uint8_t alpha_ = 255;

    void build();
};
