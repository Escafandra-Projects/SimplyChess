#pragma once

#include "State.h"
#include "Button.h"
#include <memory>

class MainMenuState :  
    public State    
{
private:
	// Variables
	/* Fondo */
	sf::Sprite background;
	/* Fuente */
	sf::Font font;
	/* Botones */
	std::map<std::string, std::unique_ptr<Button>> buttons;
	/* Titulo */
	sf::Text titleText;

	// Inicialización
	void initVariables();
	void initTextures();
	void initKeybinds();
	void initFonts();
	void initButtons();

public:
	// Constructor & Destructor
	MainMenuState(sf::RenderWindow* window, std::map<std::string, int>* supportedKeys, std::stack<std::unique_ptr<State>>* states);
	virtual ~MainMenuState();

	// Funciones
	void updateInput(const float& dt);
	void updateButtons();
	void update(const float& dt);
	void renderButtons(sf::RenderTarget& target);
	void render(sf::RenderTarget* target = nullptr);
};

