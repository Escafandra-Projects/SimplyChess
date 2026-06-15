#include "ui/MessageBox.h"

void MessageBox::centerText() {
	this->messageText.setPosition(this->container.getPosition().x + (this->container.getGlobalBounds().width / 2.0f) - this->container.getGlobalBounds().width / 2.5f,
		this->container.getPosition().y + (this->container.getGlobalBounds().height / 2.0f) - this->messageText.getGlobalBounds().height);
}
  
MessageBox::MessageBox(sf::Font& font, std::string message, std::string buttonText, sf::Texture& buttonTexture) : font(font) {
	// Fondo (en el panel derecho, fuera del tablero de 800x800)
	this->container.setSize(sf::Vector2f(300.f, 300.f));
	this->container.setPosition(810.f, 460.f);
	this->container.setFillColor(sf::Color(255, 255, 255, 150));
	this->container.setOutlineThickness(2.f);

	// Titulo
	this->messageText.setFont(font);
	this->messageText.setCharacterSize(50);
	this->messageText.setFillColor(sf::Color(75, 53, 47, 255));
	this->messageText.setPosition(840.f, 490.f);
	this->messageText.setString(message);

	// Botón
	this->button = std::make_unique<Button>(860.f, 650.f, 100.f, 61.0f,
		&this->font, buttonText, 35,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		buttonTexture); 
	this->button->scale(2, 2);
}

MessageBox::~MessageBox() {
}

bool MessageBox::isButtonPressed()
{
	return this->button->isPressed();
}

void MessageBox::setText(std::string text)
{
	this->messageText.setString(text);
	this->centerText();
}

void MessageBox::update(sf::Vector2i& mousePosWindow)
{
	this->button->update(mousePosWindow);
}

void MessageBox::render(sf::RenderTarget& target)
{
	target.draw(this->container);
	target.draw(this->messageText);
	this->button->render(target);
}
