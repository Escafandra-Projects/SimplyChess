#include "ui/PauseMenu.h"


PauseMenu::PauseMenu(sf::Font& font) : font(font) {
	// Fondo   
	this->container.setSize(sf::Vector2f(300.f, 430.f));
	this->container.setPosition(490.f, 195.f);
	this->container.setFillColor(sf::Color(255, 255, 255, 150));
	this->container.setOutlineThickness(2.f);
	

	// Titulo
	this->menuText.setFont(font);
	this->menuText.setCharacterSize(50);   
	this->menuText.setFillColor(sf::Color(75, 53, 47, 255));
	this->menuText.setPosition(this->container.getPosition().x + 100.f, this->container.getPosition().y + 15.f);
	this->menuText.setString("Pause");

}

PauseMenu::~PauseMenu() {
}

bool PauseMenu::isButtonPressed(std::string key) {
	return this->buttons[key]->isPressed();
}

void PauseMenu::addButton(std::string key, float x, float y, std::string text, sf::Texture& buttonTexture) {

	this->buttons[key] = std::make_unique<Button>(this->container.getPosition().x + x, this->container.getPosition().y + y - 75.f, 100.f, 61.0f,
		&this->font, text, 35,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		buttonTexture);
	this->buttons[key]->scale(2, 2);
}

void PauseMenu::update(sf::Vector2i& mousePosWindow) {
	for (auto& [key, button] : this->buttons) {
		button->update(mousePosWindow);
	}
}

void PauseMenu::render(sf::RenderTarget& target) {
	target.draw(this->container);

	for (auto& [key, button] : this->buttons) {
		button->render(target);
	}
	target.draw(this->menuText);

}
