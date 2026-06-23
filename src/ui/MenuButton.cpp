#include "ui/MenuButton.h"

namespace {
    const sf::Color idleTop  {158, 120,  84};
    const sf::Color idleBot  {126,  92,  58};
    const sf::Color hoverTop {178, 138, 100};
    const sf::Color hoverBot {144, 108,  68};
    const sf::Color activeC  {108,  74,  42};

    // Bevel: bright = top/left idle, dark = right/bottom idle
    const sf::Color bevBrightT {202, 164, 106};
    const sf::Color bevBrightL {186, 146,  96};
    const sf::Color bevDarkR   { 56,  28,  10};
    const sf::Color bevDarkB   { 42,  18,   8};

    sf::Color withAlpha(sf::Color c, uint8_t a) { return {c.r, c.g, c.b, a}; }
}

MenuButton::MenuButton(float x, float y, float bw, float bh, sf::Font* font, const std::string& label, unsigned characterSize)
    : state(MenuBtnState::Idle), baseX(x), baseY(y), w(bw), h(bh), alpha(255),
      body(sf::Quads, 4)
{
    text.setFont(*font);
    text.setString(label);
    text.setCharacterSize(characterSize);
    text.setLetterSpacing(1.5f);
    rebuild();
    recolor();
}

bool MenuButton::isPressed() const {
    return state == MenuBtnState::Active;
}

void MenuButton::setAlpha(uint8_t a) {
    alpha = a;
    recolor();
}

void MenuButton::setText(const sf::String& label) {
    text.setString(label);
    rebuild();
    recolor();
}

void MenuButton::rebuild() {
    float px = baseX, py = baseY;
    if (state == MenuBtnState::Hover)  py -= 1.f;
    if (state == MenuBtnState::Active) { px += 2.f; py += 2.f; }

    body[0].position = {px,     py    };
    body[1].position = {px + w, py    };
    body[2].position = {px + w, py + h};
    body[3].position = {px,     py + h};

    bTop.setPosition(px,         py         ); bTop.setSize({w, 2.f});
    bBot.setPosition(px,         py + h - 2.f); bBot.setSize({w, 2.f});
    bLeft.setPosition(px,        py         ); bLeft.setSize({2.f, h});
    bRight.setPosition(px + w - 2.f, py    ); bRight.setSize({2.f, h});

    auto lb = text.getLocalBounds();
    text.setPosition(
        px + (w  - lb.width ) / 2.f - lb.left,
        py + (h  - lb.height) / 2.f - lb.top
    );
}

void MenuButton::recolor() {
    sf::Color top, bot, bt, bb, bl, br;

    if (state == MenuBtnState::Active) {
        top = bot = activeC;
        bt = bevDarkB;   bb = bevBrightL;
        bl = bevDarkR;   br = bevBrightT;
    } else if (state == MenuBtnState::Hover) {
        top = hoverTop; bot = hoverBot;
        bt = bevBrightT; bb = bevDarkB;
        bl = bevBrightL; br = bevDarkR;
    } else {
        top = idleTop; bot = idleBot;
        bt = bevBrightT; bb = bevDarkB;
        bl = bevBrightL; br = bevDarkR;
    }

    body[0].color = withAlpha(top, alpha);
    body[1].color = withAlpha(top, alpha);
    body[2].color = withAlpha(bot, alpha);
    body[3].color = withAlpha(bot, alpha);

    bTop.setFillColor(withAlpha(bt, alpha));
    bBot.setFillColor(withAlpha(bb, alpha));
    bLeft.setFillColor(withAlpha(bl, alpha));
    bRight.setFillColor(withAlpha(br, alpha));
    text.setFillColor(withAlpha({238, 224, 194}, alpha));
}

void MenuButton::update(const sf::Vector2i& mousePos) {
    MenuBtnState prev = state;
    state = MenuBtnState::Idle;

    sf::FloatRect bounds(baseX, baseY, w, h);
    if (bounds.contains(static_cast<sf::Vector2f>(mousePos))) {
        state = sf::Mouse::isButtonPressed(sf::Mouse::Left)
            ? MenuBtnState::Active : MenuBtnState::Hover;
    }

    if (state != prev) { rebuild(); recolor(); }
}

void MenuButton::render(sf::RenderTarget& target) {
    target.draw(body);
    target.draw(bTop); target.draw(bBot);
    target.draw(bLeft); target.draw(bRight);
    target.draw(text);
}
