#include "states/SettingsState.h"
#include <stdexcept>
#include <iostream>

void SettingsState::initVariables() {
	this->currentBaseTimeIdx = 3; // Default: 300s
	this->currentIncrementIdx = 0; // Default: 0s
	this->keytimeMax = 30.0f; // Aumentar delay de botones para que no cambien tan rápido
}

void SettingsState::loadSettings() {
	std::ifstream ifs("config/game.ini");
	if (ifs.is_open()) {
		ifs >> this->baseTime >> this->increment;
		ifs.close();

		for (size_t i = 0; i < baseTimeOptions.size(); i++) {
			if (baseTimeOptions[i] == this->baseTime) {
				currentBaseTimeIdx = i;
				break;
			}
		}
		for (size_t i = 0; i < incrementOptions.size(); i++) {
			if (incrementOptions[i] == this->increment) {
				currentIncrementIdx = i;
				break;
			}
		}
	} else {
		this->baseTime = 300;
		this->increment = 0;
	}
}

void SettingsState::saveSettings() {
	std::ofstream ofs("config/game.ini");
	if (ofs.is_open()) {
		ofs << this->baseTimeOptions[currentBaseTimeIdx] << " " << this->incrementOptions[currentIncrementIdx] << "\n";
		ofs.close();
	}
}

void SettingsState::initTextures() {
	if (!this->textures["BACKGROUND"].loadFromFile("resources/images/menu/background.png")) {
		throw std::runtime_error("ERROR::SETTINGS_STATE::FAILED TO LOAD BACKGROUND");
	}
	this->background.setTexture(this->textures["BACKGROUND"]);
	this->background.scale(4.49f, 4.1f);

	if (!this->textures["BUTTONS"].loadFromFile("resources/images/interface/buttons.png")) {
		throw std::runtime_error("ERROR::SETTINGS_STATE::FAILED TO LOAD BUTTONS");
	}
}

void SettingsState::initKeybinds() {
}

void SettingsState::initFonts() {
	if (!this->font.loadFromFile("resources/fonts/Factory LJDS.ttf")) {
		throw std::runtime_error("ERROR::SETTINGS_STATE::COULD NOT LOAD FONT");
	}
}

void SettingsState::initText() {
	this->titleText.setFont(this->font);
	this->titleText.setCharacterSize(80);
	this->titleText.setString("Settings");
	// Centrar el título sobre el banner (el banner de fondo tiene su propio diseño, ajustamos acorde)
	this->titleText.setPosition(420.f, 130.f);

	this->timeLabel.setFont(this->font);
	this->timeLabel.setCharacterSize(40);
	this->timeLabel.setString("Base Time:");
	this->timeLabel.setPosition(250.f, 300.f);

	this->incrementLabel.setFont(this->font);
	this->incrementLabel.setCharacterSize(40);
	this->incrementLabel.setString("Increment:");
	this->incrementLabel.setPosition(250.f, 450.f);
}

void SettingsState::initButtons() {
	// Aumentamos la escala para que el texto de las opciones quepa correctamente sin distorsionarse
	this->buttons["TIME_CYCLE"] = std::make_unique<Button>(550.0f, 280.0f, 100.f, 61.0f, 
		&this->font, "5 min", 30,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		this->textures["BUTTONS"]);
	this->buttons["TIME_CYCLE"]->scale(2.5f, 2.5f);

	this->buttons["INC_CYCLE"] = std::make_unique<Button>(550.0f, 430.0f, 100.f, 61.0f, 
		&this->font, "0 sec", 30,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		this->textures["BUTTONS"]);
	this->buttons["INC_CYCLE"]->scale(2.5f, 2.5f);

	this->buttons["CANCEL"] = std::make_unique<Button>(200.0f, 600.0f, 100.0f, 61.0f,
		&this->font, "Cancel", 40,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		this->textures["BUTTONS"]);
	this->buttons["CANCEL"]->scale(3.0f, 3.0f);

	this->buttons["CONFIRM"] = std::make_unique<Button>(600.0f, 600.0f, 100.0f, 61.0f,
		&this->font, "Confirm", 40,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		this->textures["BUTTONS"]);
	this->buttons["CONFIRM"]->scale(3.0f, 3.0f);
}

void SettingsState::updateButtonTexts() {
	int bt = baseTimeOptions[currentBaseTimeIdx];
	if (bt == 0) {
		// The font doesn't support the infinity symbol, so we use "Infinite"
		this->buttons["TIME_CYCLE"]->setText("Infinite");
	} else {
		std::string btStr = std::to_string(bt / 60) + " min";
		this->buttons["TIME_CYCLE"]->setText(btStr);
	}

	if (bt == 0) {
		this->buttons["INC_CYCLE"]->setText("Off");
	} else {
		int inc = incrementOptions[currentIncrementIdx];
		std::string incStr = std::to_string(inc) + " sec";
		this->buttons["INC_CYCLE"]->setText(incStr);
	}
}

SettingsState::SettingsState(sf::RenderWindow* window, std::map<std::string, int>* supportedKeys, std::stack<std::unique_ptr<State>>* states) 
	: State(window, supportedKeys, states) 
{
	this->initVariables();
	this->loadSettings();
	this->initTextures();
	this->initFonts();
	this->initKeybinds();
	this->initText();
	this->initButtons();
	this->updateButtonTexts();
}

SettingsState::~SettingsState() {
}

void SettingsState::updateInput(float /*dt*/) {
}

void SettingsState::updateButtons() {
	for (auto& [name, button] : this->buttons) {
		button->update(this->mousePosWindow);
	}

	if (this->buttons["TIME_CYCLE"]->isPressed() && this->getKeytime()) {
		currentBaseTimeIdx = (currentBaseTimeIdx + 1) % baseTimeOptions.size();
		updateButtonTexts();
	}

	if (this->buttons["INC_CYCLE"]->isPressed() && this->getKeytime()) {
		if (baseTimeOptions[currentBaseTimeIdx] != 0) {
			currentIncrementIdx = (currentIncrementIdx + 1) % incrementOptions.size();
			updateButtonTexts();
		}
	}

	if (this->buttons["CANCEL"]->isPressed() && this->getKeytime()) {
		this->quit = true;
	}

	if (this->buttons["CONFIRM"]->isPressed() && this->getKeytime()) {
		this->saveSettings();
		this->quit = true;
	}
}

void SettingsState::update(float dt) {
	this->updateMousePositions();
	this->updateKeytime(dt);
	this->updateInput(dt);
	this->updateButtons();
}

void SettingsState::renderButtons(sf::RenderTarget& target) {
	for (auto& [name, button] : this->buttons) {
		button->render(target);
	}
}

void SettingsState::render(sf::RenderTarget* target) {
	if (!target) {
		target = this->window;
	}
	target->draw(this->background);
	target->draw(this->titleText);
	target->draw(this->timeLabel);
	target->draw(this->incrementLabel);

	this->renderButtons(*target);
}
