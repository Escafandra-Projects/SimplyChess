#pragma once

#include <SFML/Graphics.hpp>
#include <string>

// Estados del botón
enum class ButtonStates { BTN_IDL = 0, BTN_HOVER, BTN_ACTIVE };

/// Botón de interfaz con texto, textura por estado (reposo/hover/pulsado)
/// y detección de pulsación.
class Button
{
private:
	// Variables
	ButtonStates buttonStates;
	sf::Sprite shape;
	sf::Font* font;
	sf::Text text;

	float width;
	float height;

	/* Colores dependiendo del estado del botón */
	sf::Color textIdleColor;
	sf::Color textHoverColor;
	sf::Color textActiveColor;

	/// Centra el texto dentro del botón.
	void centerText();



public:
	// Constructor y destructor
	Button(float x, float y, float width, float height,
		sf::Font* font, std::string text, unsigned characterSize,
		sf::Color textIdleColor, sf::Color textHoverColor, sf::Color textActiveColor,
		sf::Texture& textureSheet);
	virtual ~Button();

	/// Devuelve true si el botón está pulsado.
	bool isPressed() const;

	// Funciones
	/// Escala el botón y recentra su texto.
	void scale(float x, float y);

	/// Actualiza el estado del botón según la posición del ratón.
	void update(const sf::Vector2i& mousePos);
	/// Dibuja el botón.
	void render(sf::RenderTarget& target);


};
