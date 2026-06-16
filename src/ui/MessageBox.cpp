#include "ui/MessageBox.h"

void MessageBox::centerText() {
	this->titleText.setPosition(this->container.getPosition().x + (this->container.getGlobalBounds().width / 2.0f) - (this->titleText.getGlobalBounds().width / 2.0f),
		this->container.getPosition().y + 30.f);
	this->bodyText.setPosition(this->container.getPosition().x + (this->container.getGlobalBounds().width / 2.0f) - (this->bodyText.getGlobalBounds().width / 2.0f),
		this->container.getPosition().y + 100.f);
}
  
MessageBox::MessageBox(sf::Font& font, std::string message, std::string buttonText, sf::Texture& buttonTexture) : font(font) {
	// Fondo centrado en la pantalla (1120x820)
	this->container.setSize(sf::Vector2f(600.f, 300.f));
	this->container.setPosition(260.f, 260.f);
	this->container.setFillColor(sf::Color(250, 240, 230, 240));
	this->container.setOutlineThickness(4.f);
	this->container.setOutlineColor(sf::Color(75, 53, 47, 255));

	// Textos
	this->titleText.setFont(font);
	this->titleText.setCharacterSize(50);
	this->titleText.setFillColor(sf::Color(75, 53, 47, 255));
	
	this->bodyText.setFont(font);
	this->bodyText.setCharacterSize(45);
	this->bodyText.setFillColor(sf::Color(75, 53, 47, 255));

	this->setText(message);

	// Botón
	this->button = std::make_unique<Button>(460.f, 430.f, 100.f, 61.0f,
		&this->font, buttonText, 35,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		buttonTexture); 
	this->button->scale(2.f, 2.f);
}

MessageBox::~MessageBox() {
}

bool MessageBox::isButtonPressed()
{
	return this->button->isPressed();
}

void MessageBox::setText(std::string text)
{
	size_t newlinePos = text.find('\n');
	if (newlinePos != std::string::npos) {
		this->titleText.setString(text.substr(0, newlinePos));
		this->bodyText.setString(text.substr(newlinePos + 1));
	} else {
		this->titleText.setString(text);
		this->bodyText.setString("");
	}
	this->centerText();
}

void MessageBox::update(sf::Vector2i& mousePosWindow)
{
	this->button->update(mousePosWindow);
}

void MessageBox::render(sf::RenderTarget& target)
{
	target.draw(this->container);
	target.draw(this->titleText);
	target.draw(this->bodyText);
	this->button->render(target);
}
