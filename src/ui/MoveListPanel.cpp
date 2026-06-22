#include "ui/MoveListPanel.h"
#include <algorithm>

MoveListPanel::MoveListPanel(const sf::Font& font, sf::FloatRect bounds)
    : font(font), bounds(bounds), scrollOffset(0.f), maxScroll(0.f), lastHistorySize(-1) {
    
    // Background Rect
    backgroundRect.setSize(sf::Vector2f(bounds.width, bounds.height));
    backgroundRect.setPosition(bounds.left, bounds.top);
    backgroundRect.setFillColor(sf::Color(250, 240, 230, 200)); // semi-transparent light cream
    backgroundRect.setOutlineThickness(2.f);
    backgroundRect.setOutlineColor(sf::Color(75, 53, 47, 255)); // dark brown border
    
    // Header Bg
    headerBg.setSize(sf::Vector2f(bounds.width, 30.f));
    headerBg.setPosition(bounds.left, bounds.top);
    headerBg.setFillColor(sf::Color(75, 53, 47, 255)); // solid dark brown
    
    // Header Text No
    headerTextNo.setFont(font);
    headerTextNo.setCharacterSize(18);
    headerTextNo.setFillColor(sf::Color::White);
    headerTextNo.setString("No.");
    sf::FloatRect noBounds = headerTextNo.getLocalBounds();
    headerTextNo.setPosition(
        bounds.left + (colWidthNo / 2.f) - (noBounds.width / 2.f) - noBounds.left,
        bounds.top + (30.f / 2.f) - (noBounds.height / 2.f) - noBounds.top
    );
    
    // Header Text White
    headerTextWhite.setFont(font);
    headerTextWhite.setCharacterSize(18);
    headerTextWhite.setFillColor(sf::Color::White);
    headerTextWhite.setString("White");
    sf::FloatRect whiteBounds = headerTextWhite.getLocalBounds();
    headerTextWhite.setPosition(
        bounds.left + colWidthNo + (colWidthWhite / 2.f) - (whiteBounds.width / 2.f) - whiteBounds.left,
        bounds.top + (30.f / 2.f) - (whiteBounds.height / 2.f) - whiteBounds.top
    );
    
    // Header Text Black
    headerTextBlack.setFont(font);
    headerTextBlack.setCharacterSize(18);
    headerTextBlack.setFillColor(sf::Color::White);
    headerTextBlack.setString("Black");
    sf::FloatRect blackBounds = headerTextBlack.getLocalBounds();
    headerTextBlack.setPosition(
        bounds.left + colWidthNo + colWidthWhite + (colWidthBlack / 2.f) - (blackBounds.width / 2.f) - blackBounds.left,
        bounds.top + (30.f / 2.f) - (blackBounds.height / 2.f) - blackBounds.top
    );

    // Header border line to separate header from scrollable content
    headerBorder.setSize(sf::Vector2f(bounds.width, 2.f));
    headerBorder.setPosition(bounds.left, bounds.top + 30.f);
    headerBorder.setFillColor(sf::Color(75, 53, 47, 255));
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
        if (i % 2 == 0) {
            row.rowBg.setFillColor(sf::Color(240, 230, 220, 180)); // slightly darker cream
        } else {
            row.rowBg.setFillColor(sf::Color(250, 240, 230, 180)); // standard cream
        }
        
        // No text
        row.noText.setFont(font);
        row.noText.setCharacterSize(18);
        row.noText.setFillColor(sf::Color(75, 53, 47));
        row.noText.setString(std::to_string(i + 1) + ".");
        sf::FloatRect noBounds = row.noText.getLocalBounds();
        row.noText.setPosition(
            (colWidthNo / 2.f) - (noBounds.width / 2.f) - noBounds.left,
            i * rowHeight + (rowHeight / 2.f) - (noBounds.height / 2.f) - noBounds.top
        );
        
        // White text
        int whiteMoveIdx = i * 2;
        row.whiteText.setFont(font);
        row.whiteText.setCharacterSize(18);
        row.whiteText.setFillColor(sf::Color(75, 53, 47));
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
        row.blackText.setCharacterSize(18);
        row.blackText.setFillColor(sf::Color(75, 53, 47));
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
            row.highlightRect.setFillColor(sf::Color(255, 215, 0, 120)); // Gold highlight
        }
        
        rows.push_back(row);
    }
    
    // Recalculate maxScroll
    float totalHeight = numRows * rowHeight;
    float visibleHeight = bounds.height - 30.f; // 30px for header
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
    sf::Vector2i pixelTopLeft = target.mapCoordsToPixel(sf::Vector2f(bounds.left, bounds.top + 30.f), oldView);
    sf::Vector2i pixelBottomRight = target.mapCoordsToPixel(sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height), oldView);
    
    sf::Vector2u windowSize = target.getSize();
    if (windowSize.x > 0 && windowSize.y > 0) {
        float vpLeft = static_cast<float>(pixelTopLeft.x) / windowSize.x;
        float vpTop = static_cast<float>(pixelTopLeft.y) / windowSize.y;
        float vpWidth = static_cast<float>(pixelBottomRight.x - pixelTopLeft.x) / windowSize.x;
        float vpHeight = static_cast<float>(pixelBottomRight.y - pixelTopLeft.y) / windowSize.y;
        
        sf::View scrollView;
        scrollView.setViewport(sf::FloatRect(vpLeft, vpTop, vpWidth, vpHeight));
        scrollView.setSize(bounds.width, bounds.height - 30.f);
        scrollView.setCenter(bounds.width / 2.f, scrollOffset + (bounds.height - 30.f) / 2.f);
        
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
    
    // 3. Draw header (drawn outside the scroll view so it stays fixed)
    target.draw(headerBg);
    target.draw(headerTextNo);
    target.draw(headerTextWhite);
    target.draw(headerTextBlack);
    target.draw(headerBorder);
}
