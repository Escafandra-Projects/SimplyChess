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
	// 1. Dark background overlay (covers 1280x820 window)
	this->bgRect.setSize({1280.f, 820.f});
	this->bgRect.setFillColor(sf::Color(26, 12, 6)); // Dark brown canvas

	// 2. Central wooden panel
	this->panel.setSize({900.f, 700.f});
	this->panel.setPosition(190.f, 60.f); // Centered
	this->panel.setFillColor(sf::Color(140, 102, 68)); // Wooden fill
	this->panel.setOutlineColor(sf::Color(60, 30, 12)); // Border color
	this->panel.setOutlineThickness(3.f);

	// 3. Golden inner frame
	this->panelInnerFrame.setSize({900.f - 14.f, 700.f - 14.f});
	this->panelInnerFrame.setPosition(190.f + 7.f, 60.f + 7.f);
	this->panelInnerFrame.setFillColor(sf::Color::Transparent);
	this->panelInnerFrame.setOutlineColor(sf::Color(208, 158, 78, 38)); // Gold with ~15% alpha
	this->panelInnerFrame.setOutlineThickness(1.f);
}

void SettingsState::initKeybinds() {
}

void SettingsState::initFonts() {
	if (!this->font.loadFromFile("resources/fonts/Gameplay.ttf")) {
		throw std::runtime_error("ERROR::SETTINGS_STATE::COULD NOT LOAD FONT");
	}
}

void SettingsState::initText() {
	this->titleText.setFont(this->font);
	this->titleText.setCharacterSize(50);
	this->titleText.setString("Settings");
	this->titleText.setFillColor(sf::Color(238, 224, 194));
	
	// Center the title horizontally
	auto titleBounds = this->titleText.getLocalBounds();
	this->titleText.setOrigin(titleBounds.left + titleBounds.width / 2.0f, titleBounds.top + titleBounds.height / 2.0f);
	this->titleText.setPosition(1280.f / 2.0f, 115.f);

	this->modeLabel.setFont(this->font);
	this->modeLabel.setCharacterSize(28);
	this->modeLabel.setString("Opponent:");
	this->modeLabel.setFillColor(sf::Color(238, 224, 194));
	this->modeLabel.setPosition(350.f, 300.f);

	this->diffLabel.setFont(this->font);
	this->diffLabel.setCharacterSize(28);
	this->diffLabel.setString("Difficulty:");
	this->diffLabel.setFillColor(sf::Color(238, 224, 194));
	this->diffLabel.setPosition(350.f, 410.f);

	this->timeLabel.setFont(this->font);
	this->timeLabel.setCharacterSize(28);
	this->timeLabel.setString("Base Time:");
	this->timeLabel.setFillColor(sf::Color(238, 224, 194));
	this->timeLabel.setPosition(350.f, 300.f);

	this->incrementLabel.setFont(this->font);
	this->incrementLabel.setCharacterSize(28);
	this->incrementLabel.setString("Increment:");
	this->incrementLabel.setFillColor(sf::Color(238, 224, 194));
	this->incrementLabel.setPosition(350.f, 410.f);
}

void SettingsState::initButtons() {
	// Pestañas
	this->buttons["TAB_BOT"] = std::make_unique<MenuButton>(350.f, 180.f, 200.f, 50.f, &this->font, "Bot", 24);
	this->buttons["TAB_TIME"] = std::make_unique<MenuButton>(650.f, 180.f, 200.f, 50.f, &this->font, "Time", 24);

	// Opciones
	this->buttons["MODE_CYCLE"] = std::make_unique<MenuButton>(650.f, 290.f, 250.f, 50.f, &this->font, "Escafandrin", 24);
	this->buttons["DIFF_CYCLE"] = std::make_unique<MenuButton>(650.f, 400.f, 250.f, 50.f, &this->font, "Normal", 24);
	this->buttons["TIME_CYCLE"] = std::make_unique<MenuButton>(650.f, 290.f, 250.f, 50.f, &this->font, "5 min", 24);
	this->buttons["INC_CYCLE"] = std::make_unique<MenuButton>(650.f, 400.f, 250.f, 50.f, &this->font, "0 sec", 24);

	// Controles
	this->buttons["CANCEL"] = std::make_unique<MenuButton>(350.f, 560.f, 200.f, 50.f, &this->font, "Cancel", 28);
	this->buttons["CONFIRM"] = std::make_unique<MenuButton>(650.f, 560.f, 200.f, 50.f, &this->font, "Confirm", 28);
}

void SettingsState::updateButtonTexts() {
	this->buttons["MODE_CYCLE"]->setText(modeOptions[currentModeIdx]);
	this->buttons["DIFF_CYCLE"]->setText(diffNames[currentDiffIdx]);

	if (currentModeIdx == 0) { // Local 2P
		this->buttons["DIFF_CYCLE"]->setText("N/A");
	}

	int bt = baseTimeOptions[currentBaseTimeIdx];
	if (bt == 0) {
		this->buttons["TIME_CYCLE"]->setText("INF");
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
	target->draw(this->bgRect);
	target->draw(this->panel);
	target->draw(this->panelInnerFrame);
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
