#include "states/State.h"

// Constructor y destructor
State::State(sf::RenderWindow* window, std::map<std::string, int>* supportedKeys, std::stack<std::unique_ptr<State>>* states) {
	this->window = window;
	this->supportedKeys = supportedKeys;
	this->states = states;
	this->quit = false;
	this->paused = false;
	this->keytime = 0.0f;
	this->keytimeMax = 30.0f;
}

State::~State() {

}

// Métodos de consulta
bool State::getQuit() const {
	return this->quit;
}

bool State::getKeytime() {
	if (this->keytime >= this->keytimeMax) {
		this->keytime = 0.0f;
		return true;
	}
	return false;
}

// Funciones públicas
void State::endState() {
	this->quit = true;
}

void State::pauseState() {
	this->paused = true;
}

void State::unpauseState() {
	this->paused = false;
}

// Funciones virtuales
void State::updateMousePositions() {
	this->mousePosScreen = sf::Mouse::getPosition(); 
	this->mousePosWindow = sf::Mouse::getPosition(*this->window);
	this->mousePosView = this->window->mapPixelToCoords(sf::Mouse::getPosition(*this->window));
}

void State::updateKeytime(float dt) {
	if (this->keytime < this->keytimeMax) {
		this->keytime += 100.0f * dt;
	}
}
