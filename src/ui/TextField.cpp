#include "ui/TextField.h"

// Constructor y destructor
TextField::TextField(float x, float y, float width, float height,
	sf::Font* font, unsigned characterSize, const std::string& placeholder,
	std::size_t maxLength) {

	this->font = font;
	this->content = "";
	this->focused = false;
	this->maxLength = maxLength;
	this->caretTimer = 0.f;
	this->caretVisible = true;

	// Caja con borde.
	this->box.setPosition(x, y);
	this->box.setSize(sf::Vector2f(width, height));
	this->box.setFillColor(sf::Color(245, 240, 235, 255));
	this->box.setOutlineThickness(3.f);
	this->box.setOutlineColor(sf::Color(120, 90, 80, 255));

	// Texto introducido.
	this->text.setFont(*this->font);
	this->text.setCharacterSize(characterSize);
	this->text.setFillColor(sf::Color(60, 45, 40, 255));

	// Placeholder atenuado.
	this->placeholder.setFont(*this->font);
	this->placeholder.setCharacterSize(characterSize);
	this->placeholder.setString(placeholder);
	this->placeholder.setFillColor(sf::Color(150, 140, 135, 255));

	this->layoutText();
}

TextField::~TextField() {
}

void TextField::layoutText() {
	// Alinea el texto verticalmente centrado, con un pequeño margen a la izquierda.
	const float padX = 12.f;
	float boxX = this->box.getPosition().x;
	float boxY = this->box.getPosition().y;
	float boxH = this->box.getSize().y;

	float textY = boxY + (boxH - static_cast<float>(this->text.getCharacterSize())) / 2.f - 6.f;
	this->text.setPosition(boxX + padX, textY);
	this->placeholder.setPosition(boxX + padX, textY);
}

// Métodos de consulta
std::string TextField::getText() const {
	return this->content;
}

bool TextField::isFocused() const {
	return this->focused;
}

bool TextField::isMouseOver(const sf::Vector2i& mousePos) const {
	return this->box.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
}

// Funciones
void TextField::setText(const std::string& value) {
	this->content = value;
	this->text.setString(this->content);
}

void TextField::setFocused(bool value) {
	this->focused = value;
	if (this->focused) {
		// Reinicia el parpadeo para que el cursor aparezca de inmediato.
		this->caretTimer = 0.f;
		this->caretVisible = true;
	}
}

void TextField::handleEvent(const sf::Event& event) {
	if (!this->focused) return;
	if (event.type != sf::Event::TextEntered) return;

	sf::Uint32 unicode = event.text.unicode;

	if (unicode == 8) { // Retroceso
		if (!this->content.empty()) {
			this->content.pop_back();
		}
	} else if (unicode == 13 || unicode == 10) {
		// Intro: no inserta nada (el estado contenedor gestiona confirmar/saltar de campo).
		return;
	} else if (unicode >= 32 && unicode < 127) { // ASCII imprimible
		if (this->content.size() < this->maxLength) {
			this->content += static_cast<char>(unicode);
		}
	}

	this->text.setString(this->content);
}

void TextField::update(float dt) {
	// Borde resaltado cuando está enfocado.
	this->box.setOutlineColor(this->focused ? sf::Color(180, 120, 60, 255)
	                                         : sf::Color(120, 90, 80, 255));

	// Parpadeo del cursor (solo enfocado).
	if (this->focused) {
		this->caretTimer += dt;
		if (this->caretTimer >= 0.5f) {
			this->caretTimer = 0.f;
			this->caretVisible = !this->caretVisible;
		}
	}
}

void TextField::render(sf::RenderTarget& target) {
	target.draw(this->box);

	if (this->content.empty() && !this->focused) {
		target.draw(this->placeholder);
	} else {
		target.draw(this->text);
	}

	// Cursor: una barra vertical justo tras el texto.
	if (this->focused && this->caretVisible) {
		float caretX = this->text.getPosition().x + this->text.getGlobalBounds().width + 3.f;
		float caretY = this->box.getPosition().y + 10.f;
		sf::RectangleShape caret(sf::Vector2f(2.f, this->box.getSize().y - 20.f));
		caret.setPosition(caretX, caretY);
		caret.setFillColor(sf::Color(60, 45, 40, 255));
		target.draw(caret);
	}
}
