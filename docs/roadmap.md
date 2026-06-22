# Roadmap

Tareas concretas y priorizadas organizadas en hitos. Cada hito agrupa funcionalidades
relacionadas que se desarrollarán de forma incremental.

> Consulta [`ideas.md`](ideas.md) para propuestas exploratorias que aún no están
> comprometidas en el roadmap.

---

## ✅ Completado

Funcionalidades ya implementadas y funcionando:

- [x] Movimiento de todas las piezas con validación completa
- [x] Enroque corto y largo (ambos colores)
- [x] Captura al paso (en passant)
- [x] Coronación de peones con menú gráfico de selección
- [x] Detección de jaque con resaltado visual del rey
- [x] Detección de jaque mate (fin de partida con mensaje)
- [x] Menú principal con botones (Start, Settings, Exit)
- [x] Menú de pausa (Esc) con botones Continue y Exit
- [x] Sistema de puntos por capturas
- [x] Panel lateral con marcador (nombres y puntos)
- [x] Configuración de ventana vía archivos `.ini`
- [x] Refactor de estructura de carpetas por módulo (`core/`, `states/`, `ui/`, `chess/`)

---

## 🟢 Hito 1 — Reglas completas del ajedrez

**Prioridad:** Alta
**Objetivo:** El juego debe cumplir todas las reglas oficiales del ajedrez antes de
expandirse a otras áreas.

- [x] **Detección de ahogado (stalemate)**
  - Si el jugador en turno no está en jaque pero no tiene ningún movimiento legal,
    declarar empate.
  - Actualmente el juego se **bloquea** en esta situación.
  - Se puede reutilizar la lógica de `isCheckmate()` comprobando que `!isInCheck()`.

- [x] **Regla de los 50 movimientos**
  - Si se realizan 50 movimientos consecutivos sin captura ni movimiento de peón,
    declarar empate.
  - Requiere un contador que se reinicie en cada captura o movimiento de peón.

- [x] **Triple repetición de posición**
  - Si la misma posición del tablero se repite 3 veces (mismas piezas, mismo turno,
    mismos derechos de enroque y al paso), declarar empate.
  - Requiere almacenar un historial de posiciones con un hash o comparación directa.

---

## 🟡 Hito 2 — Pulido de UX

**Prioridad:** Media
**Objetivo:** Hacer que el juego se sienta más moderno, responsivo y dinámico.

- [x] **Añadir la lemniscata**
  - Cargar una fuente que soporte el símbolo de infinito o usar una textura para mostrar la lemniscata en la opción de tiempo infinito de los ajustes.

- [x] **Animaciones de movimiento**
  - Reemplazar el movimiento instantáneo (teletransporte) por interpolación fluida
    entre origen y destino.
  - Opciones: interpolación lineal (lerp) o easing (ease-in-out) usando delta-time.

- [x] **Resaltado de movimientos legales**
  - Al seleccionar una pieza, iluminar todas las casillas a las que puede moverse
    legalmente.
  - Requiere calcular y filtrar todos los destinos válidos (geometría + jaque).

- [x] **Resaltado del último movimiento**
  - Marcar visualmente las casillas de origen y destino del último movimiento realizado.
  - Almacenar las coordenadas del último movimiento completado.

- [ ] **Efectos de sonido**
  - [x] Integrar `sfml-audio` y añadir a `target_link_libraries`.
  - [x] Efectos de movimiento normal, captura y enroque.
  - [ ] Reemplazar los sonidos temporales (placeholders) de:
    - Jaque
    - Jaque mate / fin de partida

- [x] **Coordenadas en el tablero**
  - Mostrar los números (1-8) a la izquierda y las letras (A-H) en la parte inferior para ayudar a la lectura.

- [x] **Menú de Ajustes**
  - Dar funcionalidad al botón *Settings* del menú principal.
  - Permitir cambiar parámetros como los de tiempo base e incremento desde la interfaz.

- [ ] **Piezas Capturadas en Panel Lateral**
  - Mostrar visualmente en la barra de la derecha (donde el marcador) los iconos de las piezas capturadas por cada jugador.
  - Calcular la ventaja material y mostrarla numéricamente (ej: +2).

---

## 🟡 Hito 3 — Gestión de partidas

**Prioridad:** Media
**Objetivo:** Permitir el seguimiento, guardado y análisis de partidas.

- [x] **Historial de movimientos en pantalla**
  - Mostrar la notación algebraica estándar de la partida en curso
    (ej: `1. e4 e5 2. Nf3 Nc6`).
  - Requiere un registro de movimientos con formato de notación.

- [ ] **Exportar e importar partidas en PGN**
  - [x] Guardar partidas en el formato estándar *Portable Game Notation* (`.pgn`).
  - [ ] Cargar archivos `.pgn` para reproducir o continuar partidas.

- [x] **Deshacer/Rehacer movimientos (Undo/Redo)**
  - Guardar el estado completo de la partida en una pila (`std::stack`) tras cada
    movimiento.
  - Permitir retroceder y avanzar movimientos para análisis o práctica.
  - Debe guardar: `BoardGrid`, posiciones de piezas, flags de enroque, al paso, turno y puntos.

- [x] **Reloj de ajedrez (Tiempo)**
  - Añadir soporte para partidas con límite de tiempo por jugador (ej: 5 min, 10 min).
  - Implementar cuenta regresiva y fin de partida por tiempo agotado.

---

## 🟢 Hito 4 — IA y Juego contra Bot (Minimax)

**Prioridad:** Alta
**Objetivo:** Permitir al usuario jugar contra la computadora usando un motor de inteligencia artificial local.

- [x] **Motor de IA basado en Minimax**
  - Implementar el algoritmo Minimax con poda Alfa-Beta para la búsqueda del mejor movimiento.
  - Generación rápida de movimientos y evaluación heurística basada en tablas de piezas (Piece-Square Tables).
  - Estructuras de datos optimizadas (`FastBoard`, `FastMove`) para realizar cálculos rápidos e independientes del renderizado.

- [x] **Integración y Multithreading**
  - Desacoplar el cálculo de la IA de la interfaz gráfica usando `std::async` para evitar que el juego se congele.
  - Inyección del movimiento de la IA directamente al estado del tablero.

- [x] **Ajuste de dificultad**
  - Permitir escoger distintos niveles de profundidad de búsqueda (Depth) desde el menú de inicio para hacer la IA más o menos agresiva.
