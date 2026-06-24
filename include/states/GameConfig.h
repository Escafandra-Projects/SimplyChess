#pragma once

#include <string>

/// Configuración con la que se inicia una partida. La rellena GameSetupState
/// (nombres, rival, color elegido) y la consume GameState.
struct GameConfig {
	std::string whiteName = "White";
	std::string blackName = "Black";

	bool aiMode = true;       // true = contra la IA (Escafandrín); false = local 2 jugadores
	int aiDifficulty = 4;     // Profundidad máxima de la búsqueda

	float baseTime = 300.f;
	float increment = 0.f;

	// Solo relevante en modo IA. Define la orientación del tablero y el color que
	// juega la IA: si el jugador lleva blancas, la IA lleva negras y viceversa.
	bool playerIsWhite = true;
};
