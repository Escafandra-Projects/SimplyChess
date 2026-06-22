#include "states/MainMenuState.h"
#include "states/GameState.h"
#include "states/SettingsState.h"

#include <cmath>
#include <numbers>
#include <stdexcept>
#include <fstream>

// ── Layout constants ─────────────────────────────────────────────────────────
static constexpr float WIN_W   = 1280.f;
static constexpr float WIN_H   = 820.f;
static constexpr float PANEL_W = 480.f;
static constexpr float PANEL_H = 530.f;
static constexpr float PANEL_X = (WIN_W - PANEL_W) * 0.5f; // 400
static constexpr float PANEL_Y = (WIN_H - PANEL_H) * 0.5f; // 145
static constexpr float PANEL_CX = PANEL_X + PANEL_W * 0.5f; // 640
static constexpr float PAD_X   = 44.f;
static constexpr float BTN_W   = PANEL_W - 2.f * PAD_X;    // 392
static constexpr float BTN_H   = 52.f;
static constexpr float BTN_GAP = 10.f;
static constexpr float BTN_X   = PANEL_X + PAD_X;          // 444

// ── Colour helpers ────────────────────────────────────────────────────────────
static sf::Color ca(sf::Color c, uint8_t a) { return {c.r, c.g, c.b, a}; }

// ── Init ──────────────────────────────────────────────────────────────────────

void MainMenuState::initFonts()
{
    if (!this->font.loadFromFile("resources/fonts/Gameplay.ttf"))
        throw std::runtime_error("ERROR::MAIN_MENU_STATE::COULD NOT LOAD FONT");
}

void MainMenuState::initBackground()
{
    // Dark wood base
    bgRect.setSize({WIN_W, WIN_H});
    bgRect.setFillColor(sf::Color(26, 12, 6));

    // Radial vignette: TriangleFan, center transparent → edge dark
    constexpr int N = 32;
    vignetteVA.setPrimitiveType(sf::TriangleFan);
    vignetteVA.resize(N + 2);
    vignetteVA[0].position = {PANEL_CX, WIN_H * 0.5f};
    vignetteVA[0].color    = sf::Color(7, 2, 0, 0);

    const float R = std::sqrt(WIN_W * WIN_W + WIN_H * WIN_H) * 0.65f;
    for (int i = 0; i <= N; ++i) {
        float angle = (static_cast<float>(i) / N) * 2.f * static_cast<float>(std::numbers::pi);
        vignetteVA[i + 1].position = {PANEL_CX + std::cos(angle) * R, WIN_H * 0.5f + std::sin(angle) * R};
        vignetteVA[i + 1].color    = sf::Color(7, 2, 0, 204);
    }
}

void MainMenuState::initCornerBrackets()
{
    // Each corner: 1 horizontal + 1 vertical thin rect
    const sf::Color gold(200, 152, 72, 82);
    constexpr float INS  = 22.f;   // inset from window edge
    constexpr float ARM  = 44.f;   // arm length
    constexpr float THK  = 2.f;    // thickness

    // TL
    cornerBrackets[0].setSize({ARM, THK}); cornerBrackets[0].setPosition(INS, INS);
    cornerBrackets[1].setSize({THK, ARM}); cornerBrackets[1].setPosition(INS, INS);
    // TR
    cornerBrackets[2].setSize({ARM, THK}); cornerBrackets[2].setPosition(WIN_W - INS - ARM, INS);
    cornerBrackets[3].setSize({THK, ARM}); cornerBrackets[3].setPosition(WIN_W - INS - THK, INS);
    // BL
    cornerBrackets[4].setSize({ARM, THK}); cornerBrackets[4].setPosition(INS, WIN_H - INS - THK);
    cornerBrackets[5].setSize({THK, ARM}); cornerBrackets[5].setPosition(INS, WIN_H - INS - ARM);
    // BR
    cornerBrackets[6].setSize({ARM, THK}); cornerBrackets[6].setPosition(WIN_W - INS - ARM, WIN_H - INS - THK);
    cornerBrackets[7].setSize({THK, ARM}); cornerBrackets[7].setPosition(WIN_W - INS - THK, WIN_H - INS - ARM);

    for (auto& b : cornerBrackets)
        b.setFillColor(gold);
}

void MainMenuState::initPanel()
{
    panel.setSize({PANEL_W, PANEL_H});
    panel.setPosition(PANEL_X, PANEL_Y);
    panel.setFillColor(sf::Color(140, 102, 68));
    panel.setOutlineColor(sf::Color(60, 30, 12));
    panel.setOutlineThickness(3.f);

    // Inner gold frame line (inset 7px)
    panelInnerFrame.setSize({PANEL_W - 14.f, PANEL_H - 14.f});
    panelInnerFrame.setPosition(PANEL_X + 7.f, PANEL_Y + 7.f);
    panelInnerFrame.setFillColor(sf::Color::Transparent);
    panelInnerFrame.setOutlineColor(sf::Color(208, 158, 78, 38));
    panelInnerFrame.setOutlineThickness(1.f);
}

void MainMenuState::initLogo()
{
    // Main logo: black pawn
    if (!logoTex.loadFromFile("resources/images/pieces/PeonN.png"))
        throw std::runtime_error("ERROR::MAIN_MENU_STATE::COULD NOT LOAD PAWN LOGO");
    logoSprite.setTexture(logoTex);
    auto sz = logoTex.getSize();
    logoSprite.setScale(80.f / sz.x, 80.f / sz.y);
    logoSprite.setPosition(PANEL_CX - 40.f, PANEL_Y + 36.f);

    // Small escafandra watermark — bottom-right of panel
    if (!escafandraTex.loadFromFile("resources/images/menu/logo.png"))
        throw std::runtime_error("ERROR::MAIN_MENU_STATE::COULD NOT LOAD ESCAFANDRA LOGO");
    escafandraSprite.setTexture(escafandraTex);
    auto esz = escafandraTex.getSize();
    constexpr float ESMALL = 28.f;
    escafandraSprite.setScale(ESMALL / esz.x, ESMALL / esz.y);
    escafandraSprite.setPosition(
        PANEL_X + PANEL_W - ESMALL - 12.f,
        PANEL_Y + PANEL_H - ESMALL - 10.f
    );
    escafandraSprite.setColor(sf::Color(242, 226, 192, 80));
}

void MainMenuState::initTitle()
{
    titleText.setFont(font);
    titleText.setString("SIMPLY CHESS");
    titleText.setCharacterSize(26);
    titleText.setLetterSpacing(2.5f);
    titleText.setFillColor(sf::Color(242, 226, 192));

    // Center horizontally below logo
    auto lb = titleText.getLocalBounds();
    float ty = PANEL_Y + 36.f + 80.f + 16.f;
    titleText.setPosition(PANEL_CX - lb.width * 0.5f - lb.left, ty);
}

void MainMenuState::initDivider()
{
    const sf::Color gold(202, 148, 72, 173);
    auto lb = titleText.getLocalBounds();
    float divY = titleText.getPosition().y + lb.height + lb.top + 18.f;

    constexpr float DIAM = 6.f;
    constexpr float GAP  = 10.f;

    float halfLine = (BTN_W - DIAM - 2.f * GAP) * 0.5f;

    // Left line
    divider[0].setSize({halfLine, 1.f});
    divider[0].setPosition(BTN_X, divY + DIAM * 0.5f);
    divider[0].setFillColor(gold);

    // Diamond
    divider[1].setSize({DIAM, DIAM});
    divider[1].setOrigin(DIAM * 0.5f, DIAM * 0.5f);
    divider[1].setPosition(BTN_X + halfLine + GAP + DIAM * 0.5f, divY + DIAM * 0.5f);
    divider[1].setRotation(45.f);
    divider[1].setFillColor(sf::Color(202, 148, 72));

    // Right line
    divider[2].setSize({halfLine, 1.f});
    divider[2].setPosition(BTN_X + halfLine + 2.f * GAP + DIAM, divY + DIAM * 0.5f);
    divider[2].setFillColor(gold);
}

void MainMenuState::initButtons()
{
    const std::array<std::string, 4> labels = {
        "NUEVA PARTIDA", "2 JUGADORES", "OPCIONES", "SALIR"
    };

    float y0 = divider[1].getPosition().y + 3.f + 22.f; // below divider
    for (int i = 0; i < 4; ++i) {
        float by = y0 + i * (BTN_H + BTN_GAP);
        menuButtons.emplace_back(BTN_X, by, BTN_W, BTN_H, &font, labels[i]);
    }
}

void MainMenuState::initVersion()
{
    versionText.setFont(font);
    versionText.setString("v 1.0");
    versionText.setCharacterSize(10);
    versionText.setLetterSpacing(2.f);
    versionText.setFillColor(sf::Color(192, 152, 96, 92));

    auto lb = versionText.getLocalBounds();
    float vy = PANEL_Y + PANEL_H - 28.f - lb.height - lb.top;
    versionText.setPosition(PANEL_CX - lb.width * 0.5f - lb.left, vy);
}

void MainMenuState::initKeybinds()
{
    std::ifstream ifs("config/mainmenustate_keybinds.ini");
    if (ifs.is_open()) {
        std::string key, key2;
        while (ifs >> key >> key2)
            this->keybinds[key] = this->supportedKeys->at(key2);
        ifs.close();
    }
}

// ── Constructor ───────────────────────────────────────────────────────────────

MainMenuState::MainMenuState(sf::RenderWindow* window,
                             std::map<std::string, int>* supportedKeys,
                             std::stack<std::unique_ptr<State>>* states)
    : State(window, supportedKeys, states),
      vignetteVA(sf::TriangleFan),
      animFinished(false),
      mousePressedLastFrame(false)
{
    initFonts();
    initBackground();
    initCornerBrackets();
    initPanel();
    initLogo();
    initTitle();
    initDivider();
    initButtons();
    initVersion();
    initKeybinds();
    applyAlpha(0.f);
}

// ── Alpha animation ───────────────────────────────────────────────────────────

void MainMenuState::applyAlpha(float t)
{
    uint8_t panelA = static_cast<uint8_t>(std::min(1.f, t / 0.56f) * 255.f);

    panel.setFillColor(ca({140, 102, 68}, panelA));
    panel.setOutlineColor(ca({60, 30, 12}, panelA));
    panelInnerFrame.setOutlineColor(ca({208, 158, 78}, static_cast<uint8_t>(panelA * 38 / 255)));

    logoSprite.setColor(ca({255, 255, 255}, panelA));
    escafandraSprite.setColor(ca({242, 226, 192}, static_cast<uint8_t>(80u * panelA / 255u)));
    titleText.setFillColor(ca({242, 226, 192}, panelA));

    for (auto& d : divider) {
        auto fc = d.getFillColor();
        d.setFillColor(ca(fc, panelA));
    }
    versionText.setFillColor(ca({192, 152, 96}, static_cast<uint8_t>(92u * panelA / 255u)));

    for (int i = 0; i < 4; ++i) {
        float bt = std::clamp((t - 0.25f - i * 0.1f) / 0.38f, 0.f, 1.f);
        menuButtons[i].setAlpha(static_cast<uint8_t>(bt * 255.f));
    }
}

// ── Update ────────────────────────────────────────────────────────────────────

void MainMenuState::updateInput(float /*dt*/) {}

void MainMenuState::updateButtons()
{
    for (auto& btn : menuButtons)
        btn.update(this->mousePosWindow);

    bool mousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);

    if (!mousePressedLastFrame) {
        if (menuButtons[0].isPressed()) {
            // vs AI
            this->states->push(std::make_unique<GameState>(this->window, this->supportedKeys, this->states, true));
        } else if (menuButtons[1].isPressed()) {
            // 2 jugadores
            this->states->push(std::make_unique<GameState>(this->window, this->supportedKeys, this->states, false));
        } else if (menuButtons[2].isPressed()) {
            this->states->push(std::make_unique<SettingsState>(this->window, this->supportedKeys, this->states));
        } else if (menuButtons[3].isPressed()) {
            this->quit = true;
        }
    }

    mousePressedLastFrame = mousePressed;
}

void MainMenuState::update(float dt)
{
    updateInput(dt);
    updateMousePositions();

    if (!animFinished) {
        float t = animClock.getElapsedTime().asSeconds();
        applyAlpha(t);
        if (t >= 0.25f + 3 * 0.1f + 0.38f) {
            applyAlpha(999.f); // clamp to full opacity
            animFinished = true;
        }
    }

    updateButtons();
}

// ── Render ────────────────────────────────────────────────────────────────────

void MainMenuState::render(sf::RenderTarget* target)
{
    if (!target) target = this->window;

    target->draw(bgRect);
    target->draw(vignetteVA);

    for (auto& b : cornerBrackets)
        target->draw(b);

    target->draw(panel);
    target->draw(panelInnerFrame);
    target->draw(logoSprite);
    target->draw(escafandraSprite);
    target->draw(titleText);
    for (auto& d : divider)
        target->draw(d);
    for (auto& btn : menuButtons)
        btn.render(*target);
    target->draw(versionText);
}
