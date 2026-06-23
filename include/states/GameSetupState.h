#pragma once

#include "states/State.h"
#include "states/GameConfig.h"
#include "ui/Button.h"
#include "ui/TextField.h"

#include <map>
#include <memory>

/// Pantalla previa a la partida: pide los nombres de los jugadores, permite
/// elegir el rival (IA / local 2 jugadores) y, en modo IA, el color con el que
/// juega el usuario. Al confirmar, crea el GameState con la configuración.
class GameSetupState : public State {
private:
	// Variables
	sf::Font font;
	sf::Sprite background;
	sf::Text titleText;
	sf::Text modeLabel;
	sf::Text colorLabel;
	sf::Text whiteLabel;
	sf::Text blackLabel;
	sf::Text nameLabel;

	std::map<std::string, std::unique_ptr<Button>> buttons;
	std::unique_ptr<TextField> field1; // Nombre del jugador (IA) o de blancas (2P)
	std::unique_ptr<TextField> field2; // Nombre de negras (solo 2P)

	// Opciones seleccionadas
	int currentMode;   // 0 = Local 2 jugadores, 1 = IA (Escafandrín)
	int currentColor;  // 0 = Blancas, 1 = Negras, 2 = Aleatorio

	// Valores heredados de los ajustes (config/game.ini)
	float baseTime;
	float increment;
	int aiDifficulty;

	bool mousePressedLastFrame;

	// Inicialización
	void initVariables();
	void initKeybinds() override;
	void initFonts();
	void initTextures();
	void initText();
	void initButtons();
	void initFields();

	/// Lee config/game.ini para heredar modo, dificultad y tiempo por defecto.
	void loadDefaults();
	/// Actualiza los textos de los botones cíclicos (modo, color).
	void updateButtonTexts();
	/// Enfoca el campo indicado y desenfoca el resto.
	void focusField(TextField* target);
	/// Construye la GameConfig y lanza la partida.
	void startGame();

public:
	// Constructor y destructor
	GameSetupState(sf::RenderWindow* window, std::map<std::string, int>* supportedKeys, std::stack<std::unique_ptr<State>>* states);
	virtual ~GameSetupState();

	// Funciones
	void handleEvent(const sf::Event& event) override;
	void updateInput(float dt) override;
	void updateButtons();
	void update(float dt) override;
	void render(sf::RenderTarget* target = nullptr) override;
};
