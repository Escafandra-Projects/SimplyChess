#include "ui/MessageBox.h"

namespace {
	constexpr float CX = 340.f, CY = 260.f, CW = 600.f, CH = 300.f;
}

void MessageBox::centerText() {
	auto titleBounds = this->titleText.getLocalBounds();
	this->titleText.setPosition(
		CX + (CW - titleBounds.width) / 2.f - titleBounds.left,
		CY + 30.f - titleBounds.top
	);

	auto bodyBounds = this->bodyText.getLocalBounds();
	this->bodyText.setPosition(
		CX + (CW - bodyBounds.width) / 2.f - bodyBounds.left,
		CY + 100.f - bodyBounds.top
	);
}

MessageBox::MessageBox(sf::Font& font, std::string message, std::string buttonText) : font(font) {
	// Panel de madera (gradiente + veta + marco dorado)
	this->container.setBounds({CX, CY, CW, CH});

	// Textos (crema sobre madera, con sombra ligera)
	this->titleText.setFont(font);
	this->titleText.setCharacterSize(24);
	this->titleText.setLetterSpacing(2.f);
	this->titleText.setFillColor(sf::Color(242, 226, 192));
	this->titleText.setOutlineColor(sf::Color(40, 20, 8, 180));
	this->titleText.setOutlineThickness(1.f);

	this->bodyText.setFont(font);
	this->bodyText.setCharacterSize(18);
	this->bodyText.setFillColor(sf::Color(238, 224, 194));

	this->setText(message);

	// Botón
	this->button = std::make_unique<MenuButton>(
		CX + 200.f,
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
	this->container.render(target);
	target.draw(this->titleText);
	target.draw(this->bodyText);
	this->button->render(target);
}
