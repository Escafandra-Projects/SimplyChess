# Changelog

Todos los cambios relevantes del proyecto se documentan en este archivo.

El formato sigue [Keep a Changelog](https://keepachangelog.com/es/1.1.0/) y el proyecto
usa [Semantic Versioning](https://semver.org/lang/es/).

---

## [Sin publicar]

### Añadido
- **Coordenadas en el tablero**: nueva imagen de tablero con los números (1-8) a la
  izquierda y las letras (A-H) abajo.
- Documentación completa del proyecto:
  - `ESCAFANDRIN.md` — guía de contexto para asistentes IA.
  - `docs/architecture.md` — documentación técnica de arquitectura.
  - `docs/conventions.md` — convenciones de código, commits, ramas y PRs.
  - `docs/roadmap.md` — hitos priorizados con tareas concretas.
  - `docs/ideas.md` — ideas exploratorias a futuro.
  - `CHANGELOG.md` — este archivo.

### Cambiado
- La rejilla jugable se posiciona mediante un offset (`BOARD_OFFSET_X/Y`) para que
  piezas, resaltados, menú de coronación y detección de clics queden alineados con la
  nueva franja de coordenadas.

### Eliminado
- `docs/Plan de Próximos Pasos.md` — reemplazado por `docs/roadmap.md` y `docs/ideas.md`.

---

## [v0.7.0] — 2026-06-15

Refactor de la estructura del proyecto y consolidación de las reglas del ajedrez.

### Añadido
- Detección de **jaque** con resaltado visual del rey amenazado.
- Detección de **jaque mate** con mensaje de fin de partida y cuadro de diálogo.
- Validación de **jaque propio**: impedir movimientos que dejen al rey en jaque.
- Verificación de legalidad del **enroque** (rey no en jaque ni pasando por casilla amenazada).
- Menú de **pausa** (Esc) con botones Continue y Exit.
- Cuadro de **fin de partida** (`MessageBox`) con protección anti-clic accidental.
- Flags de compilación con warnings: `-Wall -Wextra -Wpedantic`.

### Cambiado
- **Reestructuración completa** de carpetas del proyecto:
  - Código fuente organizado en subcarpetas: `core/`, `states/`, `ui/`, `chess/`.
  - `include/` y `src/` comparten la misma estructura.
  - Recursos movidos a `resources/` (minúsculas) con subcarpetas `fonts/`, `images/`.
  - Configuración movida a `config/` (minúsculas).
- CMakeLists.txt actualizado con `GLOB_RECURSE` para detectar automáticamente nuevos `.cpp`.
- Mejoras en la documentación Doxygen (`///`) en todos los headers.

---

## [v0.5.0] — 2020-09-03

Primera versión funcional del juego.

### Añadido
- Tablero de ajedrez 8×8 renderizado con SFML.
- 32 piezas con sprites individuales (6 tipos × 2 colores).
- Movimiento de todas las piezas con validación geométrica completa.
- **Enroque** corto y largo (ambos colores) con verificación de camino libre.
- **Captura al paso** (en passant) con flags de elegibilidad por columna.
- **Coronación de peones** con menú gráfico de selección (dama, caballo, alfil, torre).
- Menú principal con botones Start, Settings y Exit.
- Sistema de puntos por capturas (panel lateral con marcador).
- Configuración de ventana mediante archivos `.ini` (resolución, FPS, V-Sync, antialiasing).
- Sistema de keybindings configurable por estado.
