#include "ui/PauseMenu.h"

namespace {
    constexpr float PX = 490.f, PY = 195.f, PW = 300.f, PH = 430.f;
}

PauseMenu::PauseMenu(sf::Font& font) : font(font) {
	// Panel de madera (gradiente + veta + marco dorado)
	this->container.setBounds({PX, PY, PW, PH});

	// Titulo
	this->menuText.setFont(font);
	this->menuText.setCharacterSize(24);
	this->menuText.setLetterSpacing(2.f);
	this->menuText.setFillColor(sf::Color(242, 226, 192));
	this->menuText.setOutlineColor(sf::Color(40, 20, 8, 180));
	this->menuText.setOutlineThickness(1.f);
	this->menuText.setString("PAUSA");

	auto lb = this->menuText.getLocalBounds();
	this->menuText.setPosition(
		PX + (PW - lb.width) / 2.f - lb.left,
		PY + 25.f - lb.top
	);
}

PauseMenu::~PauseMenu() {
}

bool PauseMenu::isButtonPressed(std::string key) {
	return this->buttons[key]->isPressed();
}

void PauseMenu::addButton(std::string key, float x, float y, std::string text) {
	this->buttons[key] = std::make_unique<MenuButton>(
		PX + x,
		PY + y - 75.f,
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
	this->container.render(target);

	for (auto& [key, button] : this->buttons) {
		button->render(target);
	}
	target.draw(this->menuText);
}
