# SimplyChess

Ajedrez para dos jugadores (local, mismo teclado/ratón) escrito en **C++20** con **SFML 2.6**.

Implementa las reglas completas del ajedrez: movimiento de todas las piezas, enroque corto
y largo, captura al paso, coronación de peones (con menú de selección de pieza) y detección
de jaque y jaque mate.

## Requisitos

- Un compilador con soporte de **C++20** (GCC, Clang o MSVC recientes).
- **CMake ≥ 3.14**.
- Conexión a internet en la **primera** compilación: SFML 2.6.1 se descarga y compila
  automáticamente mediante `FetchContent` (no hace falta instalarlo a mano).

## Compilación

```bash
cmake -S . -B build
cmake --build build
```

El ejecutable y una copia de las carpetas `resources/` y `config/` se generan dentro de
`build/`.

## Ejecución

```bash
cd build
./SimplyChess
```

> Los recursos se cargan con rutas relativas (`resources/...`, `config/...`), por lo que el
> juego debe lanzarse desde el directorio donde están esas carpetas (por defecto, `build/`).

## Controles

- **Clic izquierdo**: seleccionar una pieza y, con el segundo clic, su casilla destino.
- **Clic derecho**: cancelar la selección actual.
- **Esc**: pausar / reanudar la partida.
- Al coronar un peón aparece un menú; haz clic en la pieza deseada (dama, caballo, alfil o torre).

## Configuración

La carpeta `config/` contiene archivos `.ini` editables:

- `window.ini` — título, resolución, pantalla completa, límite de FPS, V-Sync y antialiasing.
- `supported_keys.ini` — mapa de teclas reconocidas.
- `gamestate_keybinds.ini` / `mainmenustate_keybinds.ini` — asignación de acciones a teclas.

## Estructura del proyecto

El código se agrupa por responsabilidad. `include/` (cabeceras) y `src/`
(implementaciones) comparten la misma estructura de subcarpetas:

```
SimplyChess/
├── include/        Cabeceras (.h)
│   ├── core/       Game — bucle principal, ventana y pila de estados
│   ├── states/     State, MainMenuState, GameState — pantallas del juego
│   ├── ui/         Button, MessageBox, PauseMenu, PromotionMenu — interfaz
│   └── chess/      Board, Piece — lógica del ajedrez
├── src/            Implementaciones (.cpp), con la misma estructura
│   └── main.cpp    Punto de entrada
├── resources/      Imágenes y fuentes
└── config/         Ficheros de configuración (.ini)
```

Las cabeceras se incluyen con su grupo, p. ej. `#include "ui/Button.h"`,
tomando `include/` como raíz de búsqueda. La arquitectura se basa en una pila
de estados (`State`): `MainMenuState` para el menú y `GameState` para la partida.

## Documentación

| Documento | Descripción |
|---|---|
| [`docs/architecture.md`](docs/architecture.md) | Arquitectura técnica, diagramas y modelo de datos |
| [`docs/conventions.md`](docs/conventions.md) | Convenciones de código, commits, ramas y PRs |
| [`docs/roadmap.md`](docs/roadmap.md) | Hitos priorizados con tareas concretas |
| [`docs/ideas.md`](docs/ideas.md) | Ideas exploratorias a futuro |
| [`CHANGELOG.md`](CHANGELOG.md) | Registro de cambios por versión |
