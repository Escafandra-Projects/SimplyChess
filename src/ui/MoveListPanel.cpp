#include "ui/MoveListPanel.h"
#include <algorithm>

MoveListPanel::MoveListPanel(const sf::Font& font, sf::FloatRect bounds, bool darkTheme)
    : font(font), bounds(bounds), scrollOffset(0.f), maxScroll(0.f), lastHistorySize(-1),
      dark(darkTheme) {

    if (dark) {
        backgroundRect.setFillColor(sf::Color::Transparent);
        backgroundRect.setOutlineThickness(0.f);
    } else {
        backgroundRect.setSize(sf::Vector2f(bounds.width, bounds.height));
        backgroundRect.setPosition(bounds.left, bounds.top);
        backgroundRect.setFillColor(sf::Color(250, 240, 230, 200));
        backgroundRect.setOutlineThickness(2.f);
        backgroundRect.setOutlineColor(sf::Color(75, 53, 47, 255));
    }

    sf::Color hdrBg  = dark ? sf::Color(30, 14, 5, 160)  : sf::Color(75, 53, 47, 255);
    sf::Color hdrTxt = dark ? sf::Color(200, 168, 110, 180) : sf::Color::White;
    sf::Color hdrBrd = dark ? sf::Color(200, 148, 70, 50)  : sf::Color(75, 53, 47, 255);

    headerBg.setSize(sf::Vector2f(bounds.width, 30.f));
    headerBg.setPosition(bounds.left, bounds.top);
    headerBg.setFillColor(hdrBg);

    auto setupHeader = [&](sf::Text& t, const std::string& s, float cx) {
        t.setFont(font);
        t.setCharacterSize(dark ? 12 : 18);
        if (dark) t.setLetterSpacing(1.5f);
        t.setFillColor(hdrTxt);
        t.setString(s);
        auto lb = t.getLocalBounds();
        t.setPosition(bounds.left + cx - lb.width * 0.5f - lb.left,
                      bounds.top + 15.f - lb.height * 0.5f - lb.top);
    };
    setupHeader(headerTextNo,    "No.",   colWidthNo * 0.5f);
    setupHeader(headerTextWhite, "White", colWidthNo + colWidthWhite * 0.5f);
    setupHeader(headerTextBlack, "Black", colWidthNo + colWidthWhite + colWidthBlack * 0.5f);

    headerBorder.setSize(sf::Vector2f(bounds.width, 1.f));
    headerBorder.setPosition(bounds.left, bounds.top + 30.f);
    headerBorder.setFillColor(hdrBrd);
}

MoveListPanel::~MoveListPanel() {
}

void MoveListPanel::rebuildRows(const MoveHistory& history) {
    rows.clear();
    const auto& moves = history.getMoves();
    int totalMoves = moves.size();
    int numRows = (totalMoves + 1) / 2;
    
    for (int i = 0; i < numRows; ++i) {
        RowData row;
        
        // Background rectangle
        row.rowBg.setSize(sf::Vector2f(bounds.width, rowHeight));
        row.rowBg.setPosition(0.f, i * rowHeight);
        
        // Zebra striping
        if (dark) {
            row.rowBg.setFillColor(i % 2 == 0 ? sf::Color(255,255,255,10) : sf::Color::Transparent);
        } else if (i % 2 == 0) {
            row.rowBg.setFillColor(sf::Color(240, 230, 220, 180));
        } else {
            row.rowBg.setFillColor(sf::Color(250, 240, 230, 180));
        }
        
        // No text
        row.noText.setFont(font);
        row.noText.setCharacterSize(dark ? 11 : 18);
        if (dark) row.noText.setLetterSpacing(1.2f);
        row.noText.setFillColor(dark ? sf::Color(200,160,100,92) : sf::Color(75, 53, 47));
        row.noText.setString(std::to_string(i + 1) + ".");
        sf::FloatRect noBounds = row.noText.getLocalBounds();
        row.noText.setPosition(
            (colWidthNo / 2.f) - (noBounds.width / 2.f) - noBounds.left,
            i * rowHeight + (rowHeight / 2.f) - (noBounds.height / 2.f) - noBounds.top
        );
        
        // White text
        int whiteMoveIdx = i * 2;
        row.whiteText.setFont(font);
        row.whiteText.setCharacterSize(dark ? 11 : 18);
        if (dark) row.whiteText.setLetterSpacing(1.5f);
        row.whiteText.setFillColor(dark ? sf::Color(235,215,170,191) : sf::Color(75, 53, 47));
        if (whiteMoveIdx < totalMoves) {
            row.whiteText.setString(moves[whiteMoveIdx].toSAN());
        } else {
            row.whiteText.setString("");
        }
        sf::FloatRect whiteBounds = row.whiteText.getLocalBounds();
        row.whiteText.setPosition(
            colWidthNo + (colWidthWhite / 2.f) - (whiteBounds.width / 2.f) - whiteBounds.left,
            i * rowHeight + (rowHeight / 2.f) - (whiteBounds.height / 2.f) - whiteBounds.top
        );
        
        // Black text
        int blackMoveIdx = i * 2 + 1;
        row.blackText.setFont(font);
        row.blackText.setCharacterSize(dark ? 11 : 18);
        if (dark) row.blackText.setLetterSpacing(1.5f);
        row.blackText.setFillColor(dark ? sf::Color(200,175,130,158) : sf::Color(75, 53, 47));
        if (blackMoveIdx < totalMoves) {
            row.blackText.setString(moves[blackMoveIdx].toSAN());
        } else {
            row.blackText.setString("");
        }
        sf::FloatRect blackBounds = row.blackText.getLocalBounds();
        row.blackText.setPosition(
            colWidthNo + colWidthWhite + (colWidthBlack / 2.f) - (blackBounds.width / 2.f) - blackBounds.left,
            i * rowHeight + (rowHeight / 2.f) - (blackBounds.height / 2.f) - blackBounds.top
        );
        
        // Highlight last move in gold
        row.hasHighlight = false;
        if (i == numRows - 1) {
            int lastMoveIdx = totalMoves - 1;
            row.hasHighlight = true;
            row.highlightRect.setSize(sf::Vector2f(lastMoveIdx % 2 == 0 ? colWidthWhite : colWidthBlack, rowHeight));
            if (lastMoveIdx % 2 == 0) {
                // White
                row.highlightRect.setPosition(colWidthNo, i * rowHeight);
                row.whiteText.setFillColor(sf::Color(139, 101, 8)); // Dark goldenrod for legibility on gold background
            } else {
                // Black
                row.highlightRect.setPosition(colWidthNo + colWidthWhite, i * rowHeight);
                row.blackText.setFillColor(sf::Color(139, 101, 8));
            }
            row.highlightRect.setFillColor(dark ? sf::Color(202,168,90,50) : sf::Color(255, 215, 0, 120));
        }
        
        rows.push_back(row);
    }
    
    // Recalculate maxScroll
    float totalHeight = numRows * rowHeight;
    float headerH = dark ? 0.f : 30.f; // tema oscuro: sin cabecera (como el diseño)
    float visibleHeight = bounds.height - headerH;
    maxScroll = std::max(0.f, totalHeight - visibleHeight);
}

void MoveListPanel::update(const MoveHistory& history) {
    if (history.size() != lastHistorySize) {
        bool sizeIncreased = history.size() > lastHistorySize;
        rebuildRows(history);
        lastHistorySize = history.size();
        
        if (sizeIncreased) {
            // Auto-scroll to bottom
            scrollOffset = maxScroll;
        }
    }
}

void MoveListPanel::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
            sf::Vector2i mousePosPixel(event.mouseWheelScroll.x, event.mouseWheelScroll.y);
            sf::Vector2f mousePosCoord = window.mapPixelToCoords(mousePosPixel);
            if (bounds.contains(mousePosCoord.x, mousePosCoord.y)) {
                // Scroll speed: ~20px per wheel click
                scrollOffset -= event.mouseWheelScroll.delta * 20.f;
                if (scrollOffset < 0.f) scrollOffset = 0.f;
                if (scrollOffset > maxScroll) scrollOffset = maxScroll;
            }
        }
    }
}

void MoveListPanel::render(sf::RenderTarget& target) {
    // 1. Draw background panel
    target.draw(backgroundRect);
    
    // 2. Draw scrollable content using sf::View clipping/viewport
    sf::View oldView = target.getView();
    
    // Calculate normalized viewport using current view mapping to support window resizing/DPI
    float headerH = dark ? 0.f : 30.f;
    sf::Vector2i pixelTopLeft = target.mapCoordsToPixel(sf::Vector2f(bounds.left, bounds.top + headerH), oldView);
    sf::Vector2i pixelBottomRight = target.mapCoordsToPixel(sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height), oldView);

    sf::Vector2u windowSize = target.getSize();
    if (windowSize.x > 0 && windowSize.y > 0) {
        float vpLeft = static_cast<float>(pixelTopLeft.x) / windowSize.x;
        float vpTop = static_cast<float>(pixelTopLeft.y) / windowSize.y;
        float vpWidth = static_cast<float>(pixelBottomRight.x - pixelTopLeft.x) / windowSize.x;
        float vpHeight = static_cast<float>(pixelBottomRight.y - pixelTopLeft.y) / windowSize.y;

        sf::View scrollView;
        scrollView.setViewport(sf::FloatRect(vpLeft, vpTop, vpWidth, vpHeight));
        scrollView.setSize(bounds.width, bounds.height - headerH);
        scrollView.setCenter(bounds.width / 2.f, scrollOffset + (bounds.height - headerH) / 2.f);
        
        target.setView(scrollView);
        
        // Draw rows
        for (const auto& row : rows) {
            target.draw(row.rowBg);
            if (row.hasHighlight) {
                target.draw(row.highlightRect);
            }
            target.draw(row.noText);
            target.draw(row.whiteText);
            target.draw(row.blackText);
        }
        
        target.setView(oldView);
    }
    
    // 3. Draw header (drawn outside the scroll view so it stays fixed).
    //    Tema oscuro: sin cabecera (el diseño muestra las filas directamente).
    if (!dark) {
        target.draw(headerBg);
        target.draw(headerTextNo);
        target.draw(headerTextWhite);
        target.draw(headerTextBlack);
        target.draw(headerBorder);
    }
}
