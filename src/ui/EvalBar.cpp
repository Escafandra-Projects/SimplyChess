#include "ui/EvalBar.h"
#include <cmath>

EvalBar::EvalBar(float x, float y, float width, float height, const sf::Font& font)
    : barX(x), barY(y), barWidth(width), barHeight(height),
      currentFillRatio(0.5f), targetFillRatio(0.5f),
      currentScore(0.0f), targetScore(0.0f), flipped(false) {
	
	// Configurar borde
	barBorder.setPosition(x, y);
	barBorder.setSize(sf::Vector2f(width, height));
	barBorder.setFillColor(sf::Color::Transparent);
	barBorder.setOutlineColor(sf::Color(60, 30, 12));
	barBorder.setOutlineThickness(3.0f);
	
	// Marco interior
	innerFrame.setPosition(x, y);
	innerFrame.setSize(sf::Vector2f(width, height));
	innerFrame.setFillColor(sf::Color::Transparent);
	innerFrame.setOutlineColor(sf::Color(208, 158, 78, 40));
	innerFrame.setOutlineThickness(1.0f);

	// Fondos (Blanco/Negro)
	barBackground.setFillColor(sf::Color(42, 26, 12)); // Negro por defecto
	barForeground.setFillColor(sf::Color(200, 184, 136)); // Blanco por defecto

	// Texto
	evalText.setFont(font);
	evalText.setCharacterSize(11);
	evalText.setFillColor(sf::Color(242, 228, 194)); // Más claro para destacar
	
	// Fondo del texto
	textBg.setFillColor(sf::Color(26, 12, 6, 220)); // Casi opaco
	textBg.setOutlineColor(sf::Color(200, 148, 70, 80)); // Borde sutil
	textBg.setOutlineThickness(1.0f);
}

void EvalBar::setEvaluation(float score) {
	targetScore = score;
	// formula tanh
	targetFillRatio = 0.5f + 0.5f * std::tanh(score / MAX_EVAL * 2.0f);
}

void EvalBar::setFlipped(bool flipped) {
	this->flipped = flipped;
}

void EvalBar::update(float dt) {
	// Interpolación suave
	currentFillRatio += (targetFillRatio - currentFillRatio) * ANIMATION_SPEED * dt;
	currentScore += (targetScore - currentScore) * ANIMATION_SPEED * dt;

	// Actualizar visual de la barra (blanco abajo por defecto)
	float whiteHeight = barHeight * currentFillRatio;
	float blackHeight = barHeight - whiteHeight;

	if (!flipped) {
		// Blanco abajo
		barBackground.setPosition(barX, barY);
		barBackground.setSize(sf::Vector2f(barWidth, blackHeight));
		barForeground.setPosition(barX, barY + blackHeight);
		barForeground.setSize(sf::Vector2f(barWidth, whiteHeight));
	} else {
		// Negro abajo
		barBackground.setPosition(barX, barY + whiteHeight);
		barBackground.setSize(sf::Vector2f(barWidth, blackHeight));
		barForeground.setPosition(barX, barY);
		barForeground.setSize(sf::Vector2f(barWidth, whiteHeight));
	}

	// Formatear texto (+0.0)
	std::stringstream ss;
	if (std::abs(currentScore) > 9.9f) {
		ss << (currentScore > 0 ? "+" : "") << std::fixed << std::setprecision(0) << currentScore;
	} else {
		ss << (currentScore > 0 ? "+" : "") << std::fixed << std::setprecision(1) << currentScore;
	}
	evalText.setString(ss.str());

	// Centrar texto
	sf::FloatRect textBounds = evalText.getLocalBounds();
	float textX = barX + (barWidth - textBounds.width) / 2.0f - textBounds.left;
	float textY = barY + (barHeight - textBounds.height) / 2.0f - textBounds.top;
	evalText.setPosition(textX, textY);
	
	// Fondo del texto
	float paddingX = 4.0f;
	float paddingY = 4.0f;
	textBg.setSize(sf::Vector2f(textBounds.width + paddingX * 2.0f, textBounds.height + paddingY * 2.0f));
	// Ajustamos la posición restando un poco más para que quede bien centrado visualmente
	textBg.setPosition(barX + (barWidth - textBg.getSize().x) / 2.0f, textY + textBounds.top - paddingY);
}

void EvalBar::render(sf::RenderTarget& target) {
	target.draw(barBackground);
	target.draw(barForeground);
	target.draw(barBorder);
	target.draw(innerFrame);
	target.draw(textBg);
	target.draw(evalText);
}
