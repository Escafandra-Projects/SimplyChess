#pragma once


#include "State.h"
#include <memory>

    
class Game
{
private:    
	// Variables
	/* Ventana y entrada */
	std::unique_ptr<sf::RenderWindow> window;
	sf::Event sfEvent;
	std::map<std::string, int> supportedKeys;

	/* Cosas del core */
	sf::Clock dtClock;
	float dt;
	std::stack<std::unique_ptr<State>> states;

	/* Ajustes */
	std::vector<sf::VideoMode> videoModes;
	sf::ContextSettings windowSettings;
	bool fullscreen;

	// Inicialización
	void initVariables();
	void initWindow();
	void initKeys();
	void initStates();

public:
	// Constructor & Destructor
	Game();
	virtual ~Game();

	// Funciones

	/* Update */
	void updateDT();
	void updateSFMLEvents();
	void update();

	/* Render */
	void render();

	/* Core */
	void run();
	void endApplication();

};

