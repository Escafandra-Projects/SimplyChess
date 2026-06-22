#include "states/SettingsState.h"
#include <stdexcept>
#include <iostream>

void SettingsState::initVariables() {
	this->currentBaseTimeIdx = 3; // Default: 300s
	this->currentIncrementIdx = 0; // Default: 0s
	this->currentModeIdx = 1; // Default: Escafandrin
	this->currentDiffIdx = 1; // Default: Normal
	this->currentTab = TAB_BOT; // Empezamos en ajustes del bot
	this->keytimeMax = 30.0f; // Aumentar delay de botones para que no cambien tan rápido
}

void SettingsState::loadSettings() {
	std::ifstream ifs("config/game.ini");
	if (ifs.is_open()) {
		ifs >> this->baseTime >> this->increment;
		if (!(ifs >> this->aiMode)) this->aiMode = true;
		if (!(ifs >> this->aiDepth)) this->aiDepth = 4;
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
		this->currentModeIdx = this->aiMode ? 1 : 0;
		for (size_t i = 0; i < diffOptions.size(); i++) {
			if (diffOptions[i] == this->aiDepth) {
				currentDiffIdx = i;
				break;
			}
		}
	} else {
		this->baseTime = 300;
		this->increment = 0;
		this->aiMode = true;
		this->aiDepth = 4;
	}
}

void SettingsState::saveSettings() {
	std::ofstream ofs("config/game.ini");
	if (ofs.is_open()) {
		ofs << this->baseTimeOptions[currentBaseTimeIdx] << " " 
			<< this->incrementOptions[currentIncrementIdx] << " "
			<< (this->currentModeIdx == 1) << " "
			<< this->diffOptions[currentDiffIdx] << "\n";
		ofs.close();
	}
}

void SettingsState::initTextures() {
	if (!this->textures["BACKGROUND"].loadFromFile("resources/images/menu/background.png")) {
		throw std::runtime_error("ERROR::SETTINGS_STATE::FAILED TO LOAD BACKGROUND");
	}
	this->background.setTexture(this->textures["BACKGROUND"]);
	this->background.scale(5.12f, 4.1f);

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
	this->titleText.setPosition(500.f, 130.f);

	this->modeLabel.setFont(this->font);
	this->modeLabel.setCharacterSize(40);
	this->modeLabel.setString("Opponent:");
	this->modeLabel.setPosition(330.f, 420.f);

	this->diffLabel.setFont(this->font);
	this->diffLabel.setCharacterSize(40);
	this->diffLabel.setString("Difficulty:");
	this->diffLabel.setPosition(330.f, 530.f);

	this->timeLabel.setFont(this->font);
	this->timeLabel.setCharacterSize(40);
	this->timeLabel.setString("Base Time:");
	this->timeLabel.setPosition(330.f, 420.f);

	this->incrementLabel.setFont(this->font);
	this->incrementLabel.setCharacterSize(40);
	this->incrementLabel.setString("Increment:");
	this->incrementLabel.setPosition(330.f, 530.f);
}

void SettingsState::initButtons() {
	// Pestañas
	this->buttons["TAB_BOT"] = std::make_unique<Button>(350.0f, 285.0f, 100.f, 61.0f, 
		&this->font, "Bot", 30,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		this->textures["BUTTONS"]);
	this->buttons["TAB_BOT"]->scale(2.0f, 2.0f);

	this->buttons["TAB_TIME"] = std::make_unique<Button>(650.0f, 285.0f, 100.f, 61.0f, 
		&this->font, "Time", 30,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		this->textures["BUTTONS"]);
	this->buttons["TAB_TIME"]->scale(2.0f, 2.0f);

	// Aumentamos la escala para que el texto de las opciones quepa correctamente sin distorsionarse
	this->buttons["MODE_CYCLE"] = std::make_unique<Button>(630.0f, 395.0f, 100.f, 61.0f, 
		&this->font, "Escafandrin", 30,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		this->textures["BUTTONS"]);
	this->buttons["MODE_CYCLE"]->scale(2.5f, 2.5f);

	this->buttons["DIFF_CYCLE"] = std::make_unique<Button>(630.0f, 505.0f, 100.f, 61.0f, 
		&this->font, "Normal", 30,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		this->textures["BUTTONS"]);
	this->buttons["DIFF_CYCLE"]->scale(2.5f, 2.5f);

	this->buttons["TIME_CYCLE"] = std::make_unique<Button>(630.0f, 395.0f, 100.f, 61.0f, 
		&this->font, "5 min", 30,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		this->textures["BUTTONS"]);
	this->buttons["TIME_CYCLE"]->scale(2.5f, 2.5f);

	this->buttons["INC_CYCLE"] = std::make_unique<Button>(630.0f, 505.0f, 100.f, 61.0f, 
		&this->font, "0 sec", 30,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		this->textures["BUTTONS"]);
	this->buttons["INC_CYCLE"]->scale(2.5f, 2.5f);

	this->buttons["CANCEL"] = std::make_unique<Button>(280.0f, 620.0f, 100.0f, 61.0f, 
		&this->font, "Cancel", 40,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		this->textures["BUTTONS"]);
	this->buttons["CANCEL"]->scale(2.5f, 2.5f);

	this->buttons["CONFIRM"] = std::make_unique<Button>(780.0f, 620.0f, 100.0f, 61.0f, 
		&this->font, "Confirm", 40,
		sf::Color::White, sf::Color(200, 200, 200, 255), sf::Color::White,
		this->textures["BUTTONS"]);
	this->buttons["CONFIRM"]->scale(3.0f, 3.0f);
}

void SettingsState::updateButtonTexts() {
	this->buttons["MODE_CYCLE"]->setText(modeOptions[currentModeIdx]);
	this->buttons["DIFF_CYCLE"]->setText(diffNames[currentDiffIdx]);

	if (currentModeIdx == 0) { // Local 2P
		this->buttons["DIFF_CYCLE"]->setText("N/A");
	}

	int bt = baseTimeOptions[currentBaseTimeIdx];
	if (bt == 0) {
		// Tiempo infinito: la fuente Factory LJDS incluye el glifo ∞ (U+221E). Se construye
		// como sf::String UTF-32 para que SFML no decodifique los bytes UTF-8 como Latin-1.
		this->buttons["TIME_CYCLE"]->setText(sf::String(static_cast<sf::Uint32>(0x221E)));
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
	// Actualizar solo botones relevantes
	this->buttons["TAB_BOT"]->update(this->mousePosWindow);
	this->buttons["TAB_TIME"]->update(this->mousePosWindow);
	this->buttons["CANCEL"]->update(this->mousePosWindow);
	this->buttons["CONFIRM"]->update(this->mousePosWindow);

	if (currentTab == TAB_BOT) {
		this->buttons["MODE_CYCLE"]->update(this->mousePosWindow);
		this->buttons["DIFF_CYCLE"]->update(this->mousePosWindow);
	} else if (currentTab == TAB_TIME) {
		this->buttons["TIME_CYCLE"]->update(this->mousePosWindow);
		this->buttons["INC_CYCLE"]->update(this->mousePosWindow);
	}

	if (this->buttons["TAB_BOT"]->isPressed() && this->getKeytime()) {
		this->currentTab = TAB_BOT;
	}

	if (this->buttons["TAB_TIME"]->isPressed() && this->getKeytime()) {
		this->currentTab = TAB_TIME;
	}

	if (currentTab == TAB_BOT) {
		if (this->buttons["MODE_CYCLE"]->isPressed() && this->getKeytime()) {
			currentModeIdx = (currentModeIdx + 1) % modeOptions.size();
			updateButtonTexts();
		}

		if (this->buttons["DIFF_CYCLE"]->isPressed() && this->getKeytime()) {
			if (currentModeIdx == 1) { // Only change diff if Bot mode is selected
				currentDiffIdx = (currentDiffIdx + 1) % diffOptions.size();
				updateButtonTexts();
			}
		}
	} else if (currentTab == TAB_TIME) {
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
	this->buttons["TAB_BOT"]->render(target);
	this->buttons["TAB_TIME"]->render(target);
	this->buttons["CANCEL"]->render(target);
	this->buttons["CONFIRM"]->render(target);

	if (currentTab == TAB_BOT) {
		this->buttons["MODE_CYCLE"]->render(target);
		this->buttons["DIFF_CYCLE"]->render(target);
	} else if (currentTab == TAB_TIME) {
		this->buttons["TIME_CYCLE"]->render(target);
		this->buttons["INC_CYCLE"]->render(target);
	}
}

void SettingsState::render(sf::RenderTarget* target) {
	if (!target) {
		target = this->window;
	}
	target->draw(this->background);
	target->draw(this->titleText);

	if (currentTab == TAB_BOT) {
		target->draw(this->modeLabel);
		target->draw(this->diffLabel);
	} else if (currentTab == TAB_TIME) {
		target->draw(this->timeLabel);
		target->draw(this->incrementLabel);
	}

	this->renderButtons(*target);
}
