#pragma once


#include "states/State.h"
#include <memory>


/// Clase central de la aplicación: crea la ventana, gestiona la pila de
/// estados y ejecuta el bucle principal del juego.
class Game
{
private:
	// Variables
	/* Ventana y entrada */
	std::unique_ptr<sf::RenderWindow> window;
	sf::Event sfEvent;
	std::map<std::string, int> supportedKeys;

	/* Núcleo */
	sf::Clock dtClock;
	float dt;
	std::stack<std::unique_ptr<State>> states;

	/* Ajustes */
	std::vector<sf::VideoMode> videoModes;
	sf::ContextSettings windowSettings;
	bool fullscreen;

	// Inicialización
	/// Establece los valores por defecto de las variables.
	void initVariables();
	/// Crea la ventana leyendo config/window.ini.
	void initWindow();
	/// Carga las teclas soportadas desde config/supported_keys.ini.
	void initKeys();
	/// Apila el estado inicial (menú principal).
	void initStates();

public:
	// Constructor y destructor
	Game();
	virtual ~Game();

	// Funciones

	/* Actualización */
	/// Calcula el delta-time (segundos) transcurrido desde el frame anterior.
	void updateDT();
	/// Procesa los eventos de la ventana (cierre, etc.).
	void updateSFMLEvents();
	/// Actualiza el estado en la cima de la pila y lo elimina si ha terminado.
	void update();

	/* Renderizado */
	/// Dibuja el estado actual en la ventana.
	void render();

	/* Núcleo */
	/// Bucle principal: actualiza y renderiza mientras la ventana esté abierta.
	void run();
	/// Tareas de cierre de la aplicación.
	void endApplication();

};
