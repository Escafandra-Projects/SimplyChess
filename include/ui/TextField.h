#pragma once

#include <SFML/Graphics.hpp>
#include <string>

/// Campo de entrada de texto por teclado. Captura eventos sf::Event::TextEntered
/// cuando está enfocado, muestra un placeholder cuando está vacío y dibuja un
/// cursor parpadeante. El estado contenedor decide qué campo está enfocado.
class TextField {
private:
	// Variables
	sf::RectangleShape box;
	sf::Text text;
	sf::Text placeholder;
	sf::Font* font;

	std::string content;
	bool focused;
	std::size_t maxLength;

	// Cursor parpadeante
	float caretTimer;
	bool caretVisible;

	/// Recoloca el texto (y el placeholder) dentro de la caja.
	void layoutText();

public:
	// Constructor y destructor
	TextField(float x, float y, float width, float height,
		sf::Font* font, unsigned characterSize, const std::string& placeholder = "",
		std::size_t maxLength = 16);
	virtual ~TextField();

	// Métodos de consulta
	/// Devuelve el texto introducido.
	std::string getText() const;
	/// Indica si el campo está enfocado.
	bool isFocused() const;
	/// Indica si el ratón está sobre la caja.
	bool isMouseOver(const sf::Vector2i& mousePos) const;

	// Funciones
	/// Fija el texto del campo.
	void setText(const std::string& value);
	/// Enfoca o desenfoca el campo.
	void setFocused(bool value);
	/// Procesa la entrada de teclado (solo surte efecto si está enfocado).
	void handleEvent(const sf::Event& event);
	/// Actualiza el resaltado y el parpadeo del cursor.
	void update(float dt);
	/// Dibuja la caja, el texto y el cursor.
	void render(sf::RenderTarget& target);
};
