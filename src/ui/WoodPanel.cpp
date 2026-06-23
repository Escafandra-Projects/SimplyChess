#include "ui/WoodPanel.h"

const sf::Color WoodPanel::BORDER{60, 30, 12};
const sf::Color WoodPanel::INNER{208, 158, 78};

namespace {
    // Gradiente vertical del diseño: #A88060 -> #9A7250 -> #8A6240 -> #987050 -> #A88060
    struct Stop { float pos; sf::Color col; };
    const Stop STOPS[] = {
        {0.00f, {168, 128, 96}},
        {0.18f, {154, 114, 80}},
        {0.46f, {138,  98, 64}},
        {0.76f, {152, 112, 80}},
        {1.00f, {168, 128, 96}},
    };

    sf::Color withA(sf::Color c, std::uint8_t a) {
        return {c.r, c.g, c.b, static_cast<std::uint8_t>(c.a * a / 255)};
    }
}

WoodPanel::WoodPanel(sf::FloatRect bounds) {
    setBounds(bounds);
}

void WoodPanel::setBounds(sf::FloatRect bounds) {
    bounds_ = bounds;
    build();
}

void WoodPanel::setAlpha(std::uint8_t a) {
    alpha_ = a;
    build();
}

void WoodPanel::build() {
    const float x = bounds_.left, y = bounds_.top;
    const float w = bounds_.width, h = bounds_.height;

    // ── Cuerpo con gradiente: una banda de quads por cada par de paradas ──
    constexpr int N = sizeof(STOPS) / sizeof(STOPS[0]);
    gradient_.clear();
    gradient_.setPrimitiveType(sf::Quads);
    gradient_.resize((N - 1) * 4);
    int v = 0;
    for (int i = 0; i < N - 1; ++i) {
        float yTop = y + STOPS[i].pos     * h;
        float yBot = y + STOPS[i + 1].pos * h;
        sf::Color cTop = withA(STOPS[i].col,     alpha_);
        sf::Color cBot = withA(STOPS[i + 1].col, alpha_);
        gradient_[v + 0] = {{x,     yTop}, cTop};
        gradient_[v + 1] = {{x + w, yTop}, cTop};
        gradient_[v + 2] = {{x + w, yBot}, cBot};
        gradient_[v + 3] = {{x,     yBot}, cBot};
        v += 4;
    }

    // ── Veta: finas líneas verticales casi transparentes ──
    grain_.clear();
    grain_.setPrimitiveType(sf::Lines);
    sf::Color grainC = withA(sf::Color(0, 0, 0, 6), alpha_);
    for (float gx = x + 12.f; gx < x + w; gx += 13.f) {
        grain_.append({{gx, y},     grainC});
        grain_.append({{gx, y + h}, grainC});
    }

    // ── Borde exterior 3px ──
    border_.setSize({w, h});
    border_.setPosition(x, y);
    border_.setFillColor(sf::Color::Transparent);
    border_.setOutlineColor(withA(BORDER, alpha_));
    border_.setOutlineThickness(3.f);

    // ── Marco interior dorado 1px, inset 7px ──
    inner_.setSize({w - 14.f, h - 14.f});
    inner_.setPosition(x + 7.f, y + 7.f);
    inner_.setFillColor(sf::Color::Transparent);
    inner_.setOutlineColor(withA({INNER.r, INNER.g, INNER.b, 40}, alpha_));
    inner_.setOutlineThickness(1.f);
}

void WoodPanel::render(sf::RenderTarget& target) const {
    target.draw(gradient_);
    target.draw(grain_);
    target.draw(border_);
    target.draw(inner_);
}
