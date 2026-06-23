#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>
#include "chess/MoveHistory.h"

class MoveListPanel {
private:
    const sf::Font& font;
    sf::FloatRect bounds;
    
    // Background and Header
    sf::RectangleShape backgroundRect;
    sf::RectangleShape headerBg;
    sf::Text headerTextNo;
    sf::Text headerTextWhite;
    sf::Text headerTextBlack;
    sf::RectangleShape headerBorder;
    
    // Scrolling state
    float scrollOffset;
    float maxScroll;
    
    // Row layout constants
    const float rowHeight = 30.f;
    const float colWidthNo = 70.f;
    const float colWidthWhite = 155.f;
    const float colWidthBlack = 155.f;
    
    // Data structures for rows
    struct RowData {
        sf::RectangleShape rowBg;
        sf::Text noText;
        sf::Text whiteText;
        sf::Text blackText;
        sf::RectangleShape highlightRect;
        bool hasHighlight;
        float highlightX;
        float highlightWidth;
    };
    
    std::vector<RowData> rows;
    int lastHistorySize;
    bool dark;

    void rebuildRows(const MoveHistory& history);

public:
    MoveListPanel(const sf::Font& font, sf::FloatRect bounds, bool darkTheme = false);
    ~MoveListPanel();

    void update(const MoveHistory& history);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void render(sf::RenderTarget& target);
};
