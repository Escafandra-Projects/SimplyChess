#include "Button.h"

// Constructor & Destructor
Button::Button(float x, float y, float width, float height, 
	sf::Font* font, std::string text, unsigned characterSize,
	sf::Color textIdleColor, sf::Color textHoverColor, sf::Color textActiveColor,
	sf::Texture& textureSheet) {

	// Al crearse, el estado actual es idle
	this->buttonStates = BTN_IDL;
	  
	// Establece tamaño, posición y color
	this->shape.setPosition(sf::Vector2f(x, y));
	this->height = height;
	this->width = width;
	

	// Establece fuente, texto y propiedades
	this->font = font;
	this->text.setFont(*this->font);
	this->text.setString(text);
	this->text.setFillColor(textIdleColor);
	this->text.setCharacterSize(characterSize);


	// Establece todos los colores según el estado
	this->textIdleColor = textIdleColor;
	this->textHoverColor = textHoverColor;
	this->textActiveColor = textActiveColor;

	
	
	// Establecemos textura
	this->shape.setTexture(textureSheet);
	this->shape.setTextureRect(sf::IntRect(0, 0, width, height));

	this->shape.scale(1.f, 0.5f);

	// Centra texto  
	this->text.setPosition(this->shape.getPosition().x + (this->shape.getGlobalBounds().width / 2.0f) - this->text.getGlobalBounds().width / 2.0f,
		this->shape.getPosition().y + (this->shape.getGlobalBounds().height / 2.0f) - this->text.getGlobalBounds().height / 1.5f );

	
}

Button::~Button() {
	
}

void Button::centerText() {
	this->text.setPosition(this->shape.getPosition().x + (this->shape.getGlobalBounds().width / 2.0f) - this->text.getGlobalBounds().width / 2.0f,
		this->shape.getPosition().y + (this->shape.getGlobalBounds().height / 2.0f) - this->text.getGlobalBounds().height / 1.5f);

}

// Acción del botón
const bool Button::isPressed() const
{
	if (this->buttonStates == BTN_ACTIVE) return true;
	else return false;
}

void Button::scaleX(float x) {
	this->shape.scale(x, 1.f);
	this->centerText();
}

void Button::scaleY(float y) {
	this->shape.scale(1.f, y);
	this->centerText();
}

void Button::scale(float x, float y) {
	this->shape.scale(x, y);
	this->centerText();
}

// Funciones
void Button::update(const sf::Vector2i& mousePos) {
	
	// Idle
	this->buttonStates = BTN_IDL;

	// Hover
	if (this->shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
		this->buttonStates = BTN_HOVER;

		// Active
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			this->buttonStates = BTN_ACTIVE;
		}
	}

	// Gestiona colores del botón en función del estado
	switch (this->buttonStates)
	{
	case BTN_IDL:
		this->shape.setTextureRect(sf::IntRect(0, 0, width, height));
		this->text.setFillColor(this->textIdleColor);
		break;
	case BTN_HOVER:
		
		this->text.setFillColor(this->textHoverColor);
		this->shape.setTextureRect(sf::IntRect(width, 0 , width, height));
		break;

	case BTN_ACTIVE:
		
		this->text.setFillColor(this->textActiveColor);
		this->shape.setTextureRect(sf::IntRect(width*2, 0 , width, height));
		break;


	default:
		this->text.setFillColor(sf::Color::Blue);
		break;
	}
}

void Button::render(sf::RenderTarget& target) {
	target.draw(this->shape);
	target.draw(this->text);
}

