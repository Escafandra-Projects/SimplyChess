#include "ui/PromotionMenu.h"
#include "chess/Piece.h"

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
}

PromotionMenu::PromotionMenu(sf::Texture& texture) : basePosition(0.f, 0.f), shown(false), currentColor(true) {
    // Container background
    container.setSize({70.f, 240.f});
    container.setFillColor(sf::Color(140, 102, 68));
    container.setOutlineColor(sf::Color(60, 30, 12));
    container.setOutlineThickness(3.f);

    // Golden inner frame
    innerFrame.setSize({56.f, 226.f}); // 70 - 2*7, 240 - 2*7
    innerFrame.setFillColor(sf::Color::Transparent);
    innerFrame.setOutlineColor(sf::Color(208, 158, 78, 38));
    innerFrame.setOutlineThickness(1.f);

    // Header label
    if (font.loadFromFile("resources/fonts/Gameplay.ttf")) {
        headerText.setFont(font);
        headerText.setString("PROMOTE");
        headerText.setCharacterSize(11);
        headerText.setFillColor(sf::Color(60, 30, 12));
        sf::FloatRect textRect = headerText.getLocalBounds();
        headerText.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
    }

    // Set up option buttons
    options[0].type = PieceType::REINA;
    options[1].type = PieceType::CABALLO;
    options[2].type = PieceType::ALFIL;
    options[3].type = PieceType::TORRE;

    for (int i = 0; i < 4; ++i) {
        options[i].icon.setTexture(texture);
        options[i].body = sf::VertexArray(sf::Quads, 4);
        options[i].state = OptionState::Idle;
    }
}

void PromotionMenu::setPosition(float x, float y) {
    this->basePosition = sf::Vector2f(x, y);
    this->container.setPosition(x, y);
    this->innerFrame.setPosition(x + 7.f, y + 7.f);
    if (headerText.getFont()) {
        this->headerText.setPosition(x + 35.f, y + 15.f);
    }

    for (int i = 0; i < 4; ++i) {
        rebuildOption(i);
    }
}

void PromotionMenu::rebuildOption(int index) {
    float bx = basePosition.x + 10.f;
    float by = basePosition.y + 30.f + index * 50.f;

    float px = bx;
    float py = by;

    OptionState state = options[index].state;
    if (state == OptionState::Hover) py -= 1.f;
    if (state == OptionState::Active) { px += 2.f; py += 2.f; }

    float w = 50.f;
    float h = 50.f;

    options[index].body[0].position = {px,     py    };
    options[index].body[1].position = {px + w, py    };
    options[index].body[2].position = {px + w, py + h};
    options[index].body[3].position = {px,     py + h};

    options[index].bTop.setPosition(px,         py         ); options[index].bTop.setSize({w, 2.f});
    options[index].bBot.setPosition(px,         py + h - 2.f); options[index].bBot.setSize({w, 2.f});
    options[index].bLeft.setPosition(px,        py         ); options[index].bLeft.setSize({2.f, h});
    options[index].bRight.setPosition(px + w - 2.f, py    ); options[index].bRight.setSize({2.f, h});

    options[index].icon.setPosition(px, py);

    recolorOption(index);
}

void PromotionMenu::recolorOption(int index) {
    sf::Color top, bot, bt, bb, bl, br;
    OptionState state = options[index].state;

    if (state == OptionState::Active) {
        top = bot = activeC;
        bt = bevDarkB;   bb = bevBrightL;
        bl = bevDarkR;   br = bevBrightT;
    } else if (state == OptionState::Hover) {
        top = hoverTop; bot = hoverBot;
        bt = bevBrightT; bb = bevDarkB;
        bl = bevBrightL; br = bevDarkR;
    } else {
        top = idleTop; bot = idleBot;
        bt = bevBrightT; bb = bevDarkB;
        bl = bevBrightL; br = bevDarkR;
    }

    options[index].body[0].color = top;
    options[index].body[1].color = top;
    options[index].body[2].color = bot;
    options[index].body[3].color = bot;

    options[index].bTop.setFillColor(bt);
    options[index].bBot.setFillColor(bb);
    options[index].bLeft.setFillColor(bl);
    options[index].bRight.setFillColor(br);
}

void PromotionMenu::update(const sf::Vector2i& mousePos) {
    if (!shown) return;

    for (int i = 0; i < 4; ++i) {
        sf::FloatRect bounds(basePosition.x + 10.f, basePosition.y + 30.f + i * 50.f, 50.f, 50.f);

        OptionState prev = options[i].state;
        if (bounds.contains(static_cast<sf::Vector2f>(mousePos))) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                options[i].state = OptionState::Active;
            } else {
                options[i].state = OptionState::Hover;
            }
        } else {
            options[i].state = OptionState::Idle;
        }

        if (options[i].state != prev) {
            rebuildOption(i);
        }
    }
}

bool PromotionMenu::isPressed(sf::Vector2i& mousePos) const {
    if (!shown) return false;

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        for (int i = 0; i < 4; ++i) {
            sf::FloatRect bounds(basePosition.x + 10.f, basePosition.y + 30.f + i * 50.f, 50.f, 50.f);
            if (bounds.contains(static_cast<sf::Vector2f>(mousePos))) {
                return true;
            }
        }
    }
    return false;
}

PieceType PromotionMenu::getSelectedPiece(sf::Vector2i mousePos) {
    for (int i = 0; i < 4; ++i) {
        sf::FloatRect bounds(basePosition.x + 10.f, basePosition.y + 30.f + i * 50.f, 50.f, 50.f);
        if (bounds.contains(static_cast<sf::Vector2f>(mousePos))) {
            return options[i].type;
        }
    }
    return PieceType::REINA;
}

bool PromotionMenu::isShown() {
    return shown;
}

void PromotionMenu::setShown(bool shown, bool color) {
    this->shown = shown;
    this->currentColor = color;

    if (shown) {
        for (int i = 0; i < 4; ++i) {
            int textureX = color ? 0 : 50;
            int textureY = i * 50;
            options[i].icon.setTextureRect(sf::IntRect(textureX, textureY, 50, 50));
            options[i].state = OptionState::Idle;
            rebuildOption(i);
        }
    }
}

void PromotionMenu::render(sf::RenderTarget& target) {
    if (!shown) return;

    target.draw(container);
    target.draw(innerFrame);
    if (headerText.getFont()) {
        target.draw(headerText);
    }

    for (int i = 0; i < 4; ++i) {
        target.draw(options[i].body);
        target.draw(options[i].bTop);
        target.draw(options[i].bBot);
        target.draw(options[i].bLeft);
        target.draw(options[i].bRight);
        target.draw(options[i].icon);
    }
}

