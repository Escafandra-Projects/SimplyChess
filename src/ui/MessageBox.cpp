#include "ui/MessageBox.h"

void MessageBox::centerText() {
	auto titleBounds = this->titleText.getLocalBounds();
	this->titleText.setPosition(
		this->container.getPosition().x + (this->container.getSize().x - titleBounds.width) / 2.f - titleBounds.left,
		this->container.getPosition().y + 30.f - titleBounds.top
	);
	
	auto bodyBounds = this->bodyText.getLocalBounds();
	this->bodyText.setPosition(
		this->container.getPosition().x + (this->container.getSize().x - bodyBounds.width) / 2.f - bodyBounds.left,
		this->container.getPosition().y + 100.f - bodyBounds.top
	);
}
  
MessageBox::MessageBox(sf::Font& font, std::string message, std::string buttonText) : font(font) {
	// Fondo centrado en la pantalla (1280x820)
	this->container.setSize(sf::Vector2f(600.f, 300.f));
	this->container.setPosition(340.f, 260.f);
	this->container.setFillColor(sf::Color(140, 102, 68));
	this->container.setOutlineThickness(4.f);
	this->container.setOutlineColor(sf::Color(60, 30, 12));

	// Golden inner frame
	this->innerFrame.setSize(sf::Vector2f(586.f, 286.f));
	this->innerFrame.setPosition(347.f, 267.f);
	this->innerFrame.setFillColor(sf::Color::Transparent);
	this->innerFrame.setOutlineColor(sf::Color(208, 158, 78, 38));
	this->innerFrame.setOutlineThickness(1.f);

	// Textos
	this->titleText.setFont(font);
	this->titleText.setCharacterSize(24);
	this->titleText.setFillColor(sf::Color(60, 30, 12));
	
	this->bodyText.setFont(font);
	this->bodyText.setCharacterSize(18);
	this->bodyText.setFillColor(sf::Color(60, 30, 12));

	this->setText(message);

	// Botón
	this->button = std::make_unique<MenuButton>(
		this->container.getPosition().x + 200.f, 
		430.f, 
		200.f, 
		40.f,
		&this->font, 
		buttonText, 
		15
	); 
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
	target.draw(this->innerFrame);
	target.draw(this->titleText);
	target.draw(this->bodyText);
	this->button->render(target);
}
