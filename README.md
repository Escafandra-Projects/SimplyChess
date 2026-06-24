# SimplyChess

> Versión **1.0.0**

Ajedrez escrito en **C++20** con **SFML 2.6** para jugar partidas locales (dos jugadores en el mismo teclado/ratón) o enfrentarse contra la IA **Escafandrín** (basada en Minimax con poda Alfa-Beta y tres niveles de dificultad).

Implementa las reglas completas del ajedrez —movimiento de todas las piezas, enroque, captura al paso, coronación, jaque y jaque mate— y todas las condiciones de tablas (ahogado, regla de los 50 movimientos, triple repetición y acuerdo de tablas). Incluye un reloj de ajedrez configurable (tiempo base, incremento y modo infinito), soporte para deshacer/rehacer jugadas (undos/redos) y un panel lateral con historial de jugadas en notación PGN simplificada, ventaja material y captura de piezas.

## Requisitos

- Un compilador con soporte de **C++20**: GCC ≥ 10, Clang ≥ 12 o MSVC 2019/2022.
- **CMake ≥ 3.14** y un generador de compilación (Make, Ninja o Visual Studio).
- Conexión a internet en la **primera** compilación: SFML 2.6.1 se descarga y compila
  automáticamente mediante `FetchContent` (no hace falta instalarlo a mano).
- En **Linux** hacen falta las dependencias de desarrollo de SFML (ver más abajo).

## Compilación y ejecución

En las tres plataformas la compilación es la misma (desde la raíz del repositorio):

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Se genera el ejecutable y una copia de `resources/` y `config/` dentro de `build/`. El juego
carga sus recursos con **rutas relativas**, así que debe ejecutarse desde el directorio que
contiene `resources/` y `config/`.

### Linux

Instala antes el toolchain y las dependencias de desarrollo de SFML:

```bash
# Arch
sudo pacman -S --needed base-devel cmake libx11 libxrandr libxcursor libxi mesa freetype2

# Debian / Ubuntu
sudo apt install build-essential cmake libx11-dev libxrandr-dev libxcursor-dev \
  libxi-dev libgl1-mesa-dev libfreetype-dev
```

Compila con los comandos de arriba y ejecuta:

```bash
cd build && ./SimplyChess
```

### macOS

Necesitas las *Command Line Tools* de Xcode y CMake (`brew install cmake`). Compila con los
comandos de arriba y ejecuta:

```bash
cd build && ./SimplyChess
```

### Windows

Usa **Visual Studio 2019/2022** o las *Build Tools for Visual Studio* (incluyen CMake). Tras
compilar, el ejecutable queda en `build\Release\SimplyChess.exe`. Como los recursos se copian
a `build\` (no a `build\Release\`), ejecútalo con `build\` como directorio de trabajo:

```powershell
cd build
.\Release\SimplyChess.exe
```

## Controles

- **Clic izquierdo**: seleccionar una pieza y, con el segundo clic, su casilla destino.
- **Clic derecho**: cancelar la selección actual de pieza.
- **Flecha izquierda / Ctrl + Z**: deshacer la última jugada (solo disponible en partidas sin límite de tiempo).
- **Flecha derecha / Ctrl + Y**: rehacer la última jugada deshecha (solo disponible en partidas sin límite de tiempo).
- **Esc**: pausar / reanudar la partida y ver opciones (Guardar PGN, Salir).
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
├── ESCAFANDRIN.md        ← Guía de contexto para asistentes IA
├── README.md             ← Presentación pública del proyecto
├── CHANGELOG.md          ← Registro de cambios por versión
├── CMakeLists.txt        ← Configuración de compilación
├── .gitignore
│
├── include/              ← Cabeceras (.h)
│   ├── core/             ← Game — bucle principal, ventana, pila de estados
│   ├── states/           ← State (abstracta), MainMenuState, GameState
│   ├── ui/               ← Button, MessageBox, PauseMenu, PromotionMenu
│   └── chess/            ← Board, Piece — lógica del ajedrez
│
├── src/                  ← Implementaciones (.cpp), misma estructura que include/
│   ├── core/
│   ├── states/
│   ├── ui/
│   ├── chess/
│   └── main.cpp          ← Punto de entrada
│
├── config/               ← Archivos de configuración (.ini)
│   ├── window.ini
│   ├── supported_keys.ini
│   ├── gamestate_keybinds.ini
│   └── mainmenustate_keybinds.ini
│
├── resources/            ← Recursos gráficos
│   ├── fonts/
│   └── images/
│       ├── Tablero.png
│       ├── pieces/       ← 12 PNGs (6 tipos × 2 colores)
│       ├── interface/    ← Botones, fondo del panel, menú de coronación
│       └── menu/         ← Fondo y logo del menú principal
│
└── docs/                 ← Documentación extendida
    ├── architecture.md   ← Arquitectura técnica y diagramas
    ├── conventions.md    ← Convenciones de código, commits y flujo Git
    ├── roadmap.md        ← Hitos y tareas priorizadas
    └── ideas.md          ← Ideas exploratorias a futuro
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
