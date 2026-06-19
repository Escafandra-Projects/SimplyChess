#pragma once

#include <iostream>
#include <fstream>
#include <stack>
#include <memory>
#include <map>

#include <SFML/Graphics.hpp>


/// Clase base abstracta de los estados del juego (menú, partida...).
/// Define el contrato común de actualización y renderizado.
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
	/// Carga la asignación de teclas del estado (cada estado la define).
	virtual void initKeybinds() = 0;


public:
	// Constructor y destructor
	State(sf::RenderWindow* window, std::map<std::string, int>* supportedKeys, std::stack<std::unique_ptr<State>>* states);
	virtual ~State();

	// Métodos de consulta
	/// Indica si el estado ha solicitado finalizar.
	bool getQuit() const;
	/// Devuelve true si ha pasado suficiente tiempo desde la última pulsación (antirrebote).
	bool getKeytime();

	// Funciones públicas
	/// Marca el estado para que sea eliminado de la pila.
	void endState();
	/// Pausa el estado.
	void pauseState();
	/// Reanuda el estado.
	void unpauseState();


	// Funciones virtuales
	/// Procesa eventos discretos (teclado, etc.).
	virtual void handleEvent(const sf::Event& event) {}
	/// Actualiza las posiciones del ratón (pantalla, ventana y vista).
	virtual void updateMousePositions();
	/// Incrementa el contador antirrebote en función del delta-time.
	virtual void updateKeytime(float dt);
	/// Procesa la entrada del usuario.
	virtual void updateInput(float dt) = 0;
	/// Actualiza la lógica del estado.
	virtual void update(float dt) = 0;
	/// Dibuja el estado; si target es nullptr usa la ventana.
	virtual void render(sf::RenderTarget* target = nullptr) = 0;
};
