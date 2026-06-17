#pragma once

#include "states/State.h"
#include "ui/Button.h"
#include <map>
#include <string>
#include <vector>

/// Estado de Ajustes: permite configurar el reloj de ajedrez
class SettingsState : public State {
private:
	// Variables
	sf::Font font;
	sf::Sprite background;
	sf::Text titleText;
	sf::Text timeLabel;
	sf::Text incrementLabel;
	
	std::map<std::string, std::unique_ptr<Button>> buttons;

	// Configuraciones
	int baseTime;
	int increment;

	// Opciones de configuración
	const std::vector<int> baseTimeOptions = {60, 180, 300, 600};
	const std::vector<int> incrementOptions = {0, 1, 2, 3, 5};
	int currentBaseTimeIdx;
	int currentIncrementIdx;

	// Inicialización
	void initVariables();
	void initTextures();
	void initKeybinds();
	void initFonts();
	void initText();
	void initButtons();

	void loadSettings();
	void saveSettings();
	void updateButtonTexts();

public:
	// Constructor y destructor
	SettingsState(sf::RenderWindow* window, std::map<std::string, int>* supportedKeys, std::stack<std::unique_ptr<State>>* states);
	virtual ~SettingsState();

	// Funciones
	void updateInput(float dt) override;
	void updateButtons();
	void update(float dt) override;
	void renderButtons(sf::RenderTarget& target);
	void render(sf::RenderTarget* target = nullptr) override;
};
