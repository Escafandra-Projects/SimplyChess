#pragma once

#include "states/State.h"
#include "ui/Button.h"
#include <memory>

/// Estado del menú principal: título y botones para iniciar partida,
/// ajustes y salir.
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
	/// Configura el texto del título.
	void initVariables();
	/// Carga las texturas de fondo y botones.
	void initTextures();
	/// Carga la asignación de teclas desde config/mainmenustate_keybinds.ini.
	void initKeybinds();
	/// Carga la fuente de texto.
	void initFonts();
	/// Crea los botones del menú.
	void initButtons();

public:
	// Constructor y destructor
	MainMenuState(sf::RenderWindow* window, std::map<std::string, int>* supportedKeys, std::stack<std::unique_ptr<State>>* states);
	virtual ~MainMenuState();

	// Funciones
	/// Procesa la entrada del usuario (sin uso por ahora).
	void updateInput(float dt) override;
	/// Actualiza los botones y ejecuta su acción al pulsarlos.
	void updateButtons();
	/// Actualiza la entrada, el ratón y los botones.
	void update(float dt) override;
	/// Dibuja todos los botones.
	void renderButtons(sf::RenderTarget& target);
	/// Dibuja el fondo, el título y los botones.
	void render(sf::RenderTarget* target = nullptr) override;
};
