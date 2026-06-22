#include "ui/PauseMenu.h"


PauseMenu::PauseMenu(sf::Font& font) : font(font) {
	// Fondo   
	this->container.setSize(sf::Vector2f(300.f, 430.f));
	this->container.setPosition(490.f, 195.f);
	this->container.setFillColor(sf::Color(140, 102, 68));
	this->container.setOutlineColor(sf::Color(60, 30, 12));
	this->container.setOutlineThickness(3.f);
	
	// Golden inner frame
	this->innerFrame.setSize(sf::Vector2f(286.f, 416.f));
	this->innerFrame.setPosition(497.f, 202.f);
	this->innerFrame.setFillColor(sf::Color::Transparent);
	this->innerFrame.setOutlineColor(sf::Color(208, 158, 78, 38));
	this->innerFrame.setOutlineThickness(1.f);

	// Titulo
	this->menuText.setFont(font);
	this->menuText.setCharacterSize(24);   
	this->menuText.setFillColor(sf::Color(60, 30, 12));
	this->menuText.setString("PAUSA");
	
	auto lb = this->menuText.getLocalBounds();
	this->menuText.setPosition(
		this->container.getPosition().x + (this->container.getSize().x - lb.width) / 2.f - lb.left,
		this->container.getPosition().y + 25.f - lb.top
	);
}

PauseMenu::~PauseMenu() {
}

bool PauseMenu::isButtonPressed(std::string key) {
	return this->buttons[key]->isPressed();
}

void PauseMenu::addButton(std::string key, float x, float y, std::string text) {
	this->buttons[key] = std::make_unique<MenuButton>(
		this->container.getPosition().x + x,
		this->container.getPosition().y + y - 75.f,
		200.f,
		40.f,
		&this->font,
		text,
		15
	);
}

void PauseMenu::update(sf::Vector2i& mousePosWindow) {
	for (auto& [key, button] : this->buttons) {
		button->update(mousePosWindow);
	}
}

void PauseMenu::render(sf::RenderTarget& target) {
	target.draw(this->container);
	target.draw(this->innerFrame);

	for (auto& [key, button] : this->buttons) {
		button->render(target);
	}
	target.draw(this->menuText);
}
