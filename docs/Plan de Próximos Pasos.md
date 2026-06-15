
Este documento detalla las posibles direcciones y características a desarrollar para avanzar el proyecto **Simply Chess** a sus siguientes fases. Se agrupan en distintas áreas clave.

## 1. 🧠 Core Lógico del Ajedrez
Gran parte de las reglas especiales del ajedrez ya están implementadas en `Board` y `Piece` (como el Enroque, la Captura al Paso y la Promoción). El foco de desarrollo en el *core* recae en el estado avanzado de la partida:

*   **Estado de la Partida (Urgente):**
    *   **Jaque (Check):** Terminar de conectar e implementar la lógica de `isMenaced` y `jaque[]` para impedir que un jugador realice un movimiento que deje o mantenga a su propio rey bajo ataque.
    *   **Jaque Mate (Checkmate):** Crear el algoritmo que verifique si el jugador en turno está en jaque y no tiene *ningún* movimiento legal disponible para salvar al rey, declarando el final de la partida.
*   **Reglas de Tablas (Draws):**
    *   **Ahogado (Stalemate):** Condición de empate cuando el jugador en turno no está en jaque pero tampoco tiene movimientos legales posibles.
    *   **Otras reglas de tablas:** Implementar la regla de los 50 movimientos y la triple repetición de posición.

## 2. 🤖 Inteligencia Artificial (Modo Solitario)
Añadir la capacidad de jugar contra el ordenador incrementará enormemente el valor del juego.

*   **Motor Básico (Minimax):** Implementar el algoritmo Minimax con evaluación heurística del tablero (asignando valores a las piezas: Peón=1, Caballo=3, etc. y bonificaciones por posiciones centrales).
*   **Optimización (Poda Alfa-Beta):** Reducir el número de nodos evaluados en el árbol de búsqueda para permitir al bot "pensar" con varios movimientos de profundidad en un tiempo razonable.
*   **Integración UCI (Alternativa Avanzada):** En lugar de crear un motor propio desde cero, preparar a `Simply Chess` para comunicarse vía consola con motores open source de alto nivel como **Stockfish**, usando el protocolo UCI (Universal Chess Interface).

## 3. 🌐 Multijugador (Red)
Dado que el proyecto utiliza SFML, la biblioteca `sfml-network` permite expandir el juego al terreno online.

*   **Partidas en Red Local (LAN):** Implementar un modelo Cliente-Servidor donde un jugador hace de anfitrión (Host) y el otro se conecta mediante una dirección IP.
*   **Protocolo de Comunicación:** Definir los paquetes de red (ej. `<MovePiece startX="2" startY="1" endX="2" endY="3">`) para mantener los tableros sincronizados.

## 4. 🎨 Experiencia de Usuario y UI (UX/UI Polish)
Hacer que el juego se sienta más moderno, responsivo y dinámico.

*   **Animaciones:** Migrar del movimiento instantáneo (teletransporte) a animaciones fluidas interpolando las posiciones de las piezas entre su origen y su destino.
*   **Asistencia Visual:** 
    *   Al hacer clic en una pieza, iluminar todas las casillas a las que puede moverse legalmente.
    *   Resaltar el origen y el destino del último movimiento realizado.
*   **Efectos de Sonido:** Integrar `sfml-audio` para añadir efectos sonoros sutiles para movimientos, capturas, enroque, jaque y victoria.

## 5. 💾 Gestión de Partidas
*   **Historial de Movimientos:** Mostrar en pantalla la notación algebraica de la partida en curso (ej. e4, Nf3...).
*   **Sistema de Guardado (PGN):** Permitir exportar y cargar partidas en el formato estándar *Portable Game Notation* (.pgn).
*   **Deshacer/Rehacer (Undo/Redo):** Guardar el estado de la partida en una pila (Stack) permitiendo a los jugadores retroceder movimientos para análisis o partidas de práctica.

---

> [!TIP]
> **Recomendación Inicial:** El punto más crítico para priorizar suele ser **1. Core Lógico del Ajedrez** (especialmente Jaque/Jaque mate y En passant) para asegurar que el juego cumple todas las reglas oficiales antes de expandirse a otras áreas.

- podemos crear una serie de tableros virtuales para que se puedan realizar las pruebas automáticamente? que haga las pruebas un script o si no gemini

- podemos añadir un módulo de puzzles o de cargar partidas