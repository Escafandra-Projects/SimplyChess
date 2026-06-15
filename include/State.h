#pragma once

#include <iostream>
#include <fstream>
#include <stack>
#include <memory>
#include <map>

#include <SFML/Graphics.hpp>

  
class State
{
private:

protected:
	// Variables
	std::stack<std::unique_ptr<State>>* states;
	sf::RenderWindow* window;
	std::map<std::string, int>* supportedKeys;
	std::map<std::string, int> keybinds;
	bool quit;
	bool paused;
	float keytime;
	float keytimeMax;

	sf::Vector2i mousePosScreen;
	sf::Vector2i mousePosWindow;
	sf::Vector2f mousePosView;

	/* Texturas */
	std::map<std::string, sf::Texture> textures;

	// Funciones privadas
	virtual void initKeybinds() = 0;
	

public:
	// Constructor & Destructor
	State(sf::RenderWindow* window, std::map<std::string, int>* supportedKeys, std::stack<std::unique_ptr<State>>* states);
	virtual ~State();

	// Getters
	const bool& getQuit() const;
	const bool getKeytime();

	// Funciones públicas
	void endState();
	void pauseState();
	void unpauseState();


	// Funciones virtuales
	virtual void updateMousePositions();
	virtual void updateKeytime(const float& dt);
	virtual void updateInput(const float& dt) = 0;
	virtual void update(const float& dt) = 0;
	virtual void render(sf::RenderTarget* target = nullptr) = 0;
};

