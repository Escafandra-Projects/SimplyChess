#pragma once

#include "states/State.h"
#include "ui/MenuButton.h"
#include <array>
#include <vector>
#include <memory>

class MainMenuState : public State
{
private:
    // --- recursos ---
    sf::Font font;
    sf::Texture logoTex;
    sf::Sprite logoSprite;
    sf::Texture escafandraTex;
    sf::Sprite escafandraSprite;
    sf::Text escafandraLabel;

    // --- fondo y viñeta ---
    sf::RectangleShape bgRect;
    sf::VertexArray vignetteVA;

    // --- adornos de esquina (2 por esquina × 4 esquinas) ---
    std::array<sf::RectangleShape, 8> cornerBrackets;

    // --- panel central ---
    sf::RectangleShape panel;
    sf::RectangleShape panelInnerFrame;

    // --- título ---
    sf::Text titleText;

    // --- separador ornamental: línea izq, diamante, línea der ---
    std::array<sf::RectangleShape, 3> divider;

    // --- botones ---
    std::vector<MenuButton> menuButtons;

    // --- pie de versión ---
    sf::Text versionText;

    // --- animación ---
    sf::Clock animClock;
    bool animFinished;

    bool mousePressedLastFrame;

    // --- init ---
    void initFonts();
    void initBackground();
    void initCornerBrackets();
    void initPanel();
    void initLogo();
    void initTitle();
    void initDivider();
    void initButtons();
    void initVersion();
    void initKeybinds();

    // --- helpers ---
    void applyAlpha(float t);
    void updateButtons();

public:
    MainMenuState(sf::RenderWindow* window, std::map<std::string, int>* supportedKeys,
                  std::stack<std::unique_ptr<State>>* states);
    virtual ~MainMenuState() = default;

    void updateInput(float dt) override;
    void update(float dt) override;
    void render(sf::RenderTarget* target = nullptr) override;
};
