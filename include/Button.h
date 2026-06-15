#pragma once

#include <SFML/Graphics.hpp>
#include <cstring>

// Estados del botón
enum ButtonStates{BTN_IDL = 0, BTN_HOVER, BTN_ACTIVE};
   
class Button
{
private:
	// Variables
	short unsigned buttonStates;
	sf::Sprite shape;
	sf::Font* font;
	sf::Text text;

	float width;
	float height;

	/* Colores dependiento del estado del botón */
	sf::Color textIdleColor;
	sf::Color textHoverColor;
	sf::Color textActiveColor;

	void centerText();
	


public:
	// Constructor & Destructor
	Button(float x, float y, float width, float height, 
		sf::Font* font, std::string text, unsigned characterSize,
		sf::Color textIdleColor, sf::Color textHoverColor, sf::Color textActiveColor,
		sf::Texture& textureSheet);
	virtual ~Button();

	// Acción del boton
	const bool isPressed() const;

	// Funciones

	void scaleX(float x);
	void scaleY(float y);
	void scale(float x, float y);

	void update(const sf::Vector2i& mousePos);
	void render(sf::RenderTarget& target);


};

