#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <iomanip>
#include <sstream>

class EvalBar {
private:
	// Dimensiones y posición
	sf::RectangleShape barBackground;  // Porción de un color (ej: negro)
	sf::RectangleShape barForeground;  // Porción del otro color (ej: blanco)
	sf::RectangleShape barBorder;      // Borde exterior
	sf::RectangleShape innerFrame;     // Marco interior sutil
	sf::RectangleShape textBg;         // Fondo opaco para el texto
	sf::Text evalText;                 // Valor numérico (+2.3)
	
	float barX, barY, barWidth, barHeight;
	float currentFillRatio;    // 0.0 = todo negro, 1.0 = todo blanco
	float targetFillRatio;     // Objetivo de la animación
	float currentScore;        // Score actual mostrado
	float targetScore;         // Score objetivo
	bool flipped;              // Orientación según perspectiva
	
	static constexpr float ANIMATION_SPEED = 4.0f;  // Velocidad de interpolación
	static constexpr float MAX_EVAL = 15.0f;        // Score máximo para escala

public:
	EvalBar(float x, float y, float width, float height, const sf::Font& font);
	
	/// Actualiza la evaluación objetivo (con animación suave).
	void setEvaluation(float score);
	
	/// Establece la orientación (sigue perspectiva del tablero).
	void setFlipped(bool flipped);
	
	/// Actualiza la animación de interpolación.
	void update(float dt);
	
	/// Dibuja la barra.
	void render(sf::RenderTarget& target);
};
