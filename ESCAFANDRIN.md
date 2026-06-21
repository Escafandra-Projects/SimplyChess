# 🤿 Escafandrín — Guía de contexto para asistentes IA

> Este archivo es la referencia principal para **Claude Code**, **Gemini CLI (Antigravity)**
> y cualquier otro asistente de IA que trabaje en el proyecto SimplyChess.
> Léelo completo antes de realizar cualquier tarea.

---

## Sobre el proyecto

**SimplyChess** es un juego de ajedrez para dos jugadores locales (mismo ratón) desarrollado
por **Escafandra Projects**.

| Dato | Valor |
|---|---|
| Lenguaje | C++20 |
| Biblioteca gráfica | SFML 2.6.1 (descargada automáticamente vía `FetchContent`) |
| Sistema de compilación | CMake ≥ 3.14 |
| Versión actual | v1.0.0 |
| Plataformas objetivo | macOS, Linux, Windows |
| Repositorio | `Escafandra-Projects/SimplyChess` |

---

## Arquitectura — Resumen rápido

La aplicación usa una **pila de estados** (`std::stack<std::unique_ptr<State>>`):

- `Game` crea la ventana y ejecuta el bucle principal (`update` → `render`).
- Solo el estado en la **cima** de la pila se actualiza y renderiza.
- `MainMenuState` → menú con botones (Start, Settings, Exit).
- `GameState` → partida activa, contiene el `Board`.

El tablero (`Board`) mantiene **dos representaciones** sincronizadas manualmente:
1. **`BoardGrid`** — array 8×8 de strings de 2 caracteres (`"PB"` = peón blanco, `"+"` = casilla vacía blanca, `"-"` = casilla vacía negra).
2. **`pieces[16][2]`** — 32 objetos `Piece` con posición, tipo, color y sprite.

**Flujo de movimiento:**
1. Clic → `startMove()`: selecciona la pieza si es del turno correcto.
2. Clic → `endMove()`: valida en una **copia** del tablero (`testBoard`), comprueba jaque, y si es legal aplica al tablero real.

> Para detalles completos, ver `docs/architecture.md`.

---

## Estado actual del proyecto

### ✅ Implementado
- Movimiento de todas las piezas con validación completa
- Enroque corto y largo (ambos colores)
- Captura al paso (en passant)
- Coronación de peones (menú gráfico de selección)
- Detección de jaque con resaltado visual
- Detección de jaque mate (fin de partida)
- Detección de ahogado (stalemate)
- Regla de los 50 movimientos
- Triple repetición de posición
- Animaciones de movimiento (interpolación) y mover arrastrando (drag & drop)
- Resaltado de movimientos legales y del último movimiento
- Efectos de sonido (mover, capturar, jaque, fin de partida)
- Coordenadas en el tablero (números 1-8 y letras A-H)
- Menú principal con botones
- Menú de pausa (Esc)
- Menú de Ajustes (tiempo base e incremento)
- Reloj de ajedrez configurable, con opción de tiempo infinito (símbolo ∞)
- Sistema de puntos por capturas
- Configuración de ventana vía archivos `.ini`

### ❌ Pendiente (ver `docs/roadmap.md`)
- Historial de movimientos en pantalla (notación algebraica)
- Exportar e importar partidas en PGN
- Deshacer/rehacer movimientos (undo/redo)

---

## Convenciones — Resumen para IAs

> Para la referencia completa, ver `docs/conventions.md`.

### Idioma
- **Comentarios del código**: siempre en **español**.
- **Documentación** (README, docs/): en **español**.
- **Variables, clases, métodos**: mezcla de español (dominio del ajedrez: `peonPaso`,
  `jaque`, `enroque`) e inglés (conceptos de programación: `update`, `render`, `isActive`).
- **Mensajes de error en excepciones**: en inglés (`"ERROR::GAME_STATE::FAILED TO LOAD"`).
- **Texto de la UI**: en inglés (`"Start"`, `"Pause"`, `"Exit"`).

### Estilo de comentarios
- **Headers (`.h`)**: comentarios `///` estilo Doxygen antes de cada método y clase.
- **Fuentes (`.cpp`)**: comentarios `//` inline explicando la lógica.
- **Secciones** dentro de cada clase, en este orden:
  ```
  // Variables
  // Inicialización
  // Constructor y destructor
  // Funciones (o Métodos de consulta)
  ```

### Commits (Conventional Commits)
```
tipo(scope): descripción en español
```

**Tipos válidos:** `feat`, `fix`, `refactor`, `docs`, `style`, `chore`

**Scopes válidos:**

| Scope | Área |
|---|---|
| `board` | Lógica del tablero (`chess/Board`) |
| `piece` | Lógica de piezas (`chess/Piece`) |
| `ui` | Interfaz (botones, menús) |
| `game` | Bucle principal, ventana (`core/Game`) |
| `states` | Estados del juego (`states/`) |
| `config` | Archivos de configuración |
| `docs` | Documentación |
| `build` | CMake, compilación |
| `resources` | Imágenes, fuentes |

**Ejemplos:**
```
feat(board): añadir detección de ahogado
fix(piece): corregir validación del enroque largo
refactor(ui): extraer lógica de centrado de texto
docs(docs): actualizar roadmap con hito 2
chore(build): actualizar versión de SFML a 2.6.2
```

### Ramas
```
tipo/descripcion-corta
```
Ejemplos: `feat/deteccion-ahogado`, `fix/enroque-largo`, `docs/arquitectura`

### Pull Requests
- **Estrategia de merge:** Squash & Merge.
- **Aprobadores:** mínimo 1 (distinto al autor del PR).
- **Título del PR:** sigue el formato de commit (será el mensaje que quede en `main`).

### Versionado
- **Semantic Versioning** (`MAJOR.MINOR.PATCH`).
- Mantenemos `v0.x` hasta que todas las reglas del ajedrez estén implementadas.
- `v1.0.0` se publicará cuando el juego tenga reglas completas (jaque mate, ahogado, tablas).

---

## Cómo trabajar en este proyecto

### Al recibir una tarea
1. **Lee este archivo completo** si no lo has hecho ya.
2. **Consulta `docs/architecture.md`** para entender el componente que vas a modificar.
3. **Consulta `docs/conventions.md`** para seguir las convenciones establecidas.
4. **Consulta `docs/roadmap.md`** para entender el contexto y las prioridades.

### Al escribir código
- Asegurarse de que estamos en la rama main, y hacer fetch y pull primero antes de nada.
- Crear rama para trabajar en github, con las conveciones del proyecto.
- 
- Comenta en **español**, siguiendo el estilo Doxygen (`///`) en headers.
- Mantén la estructura de secciones (`// Variables`, `// Inicialización`, etc.).
- Si creas un archivo nuevo en `include/` o `src/`, hazlo en la subcarpeta correcta
  (`core/`, `states/`, `ui/`, `chess/`) y usa `#pragma once` en los headers.
- CMake detecta nuevos `.cpp` automáticamente gracias a `GLOB_RECURSE`.
- Al modificar la representación del tablero, **actualiza ambas estructuras** (grid + objetos Piece).

### Al hacer commits
- Usa Conventional Commits: `tipo(scope): descripción en español`.
- Un commit por cambio lógico.
- Las ramas se nombran `tipo/descripcion-corta`.

### Al documentar cambios
- Registra todo cambio relevante en `CHANGELOG.md`, bajo la sección `[Sin publicar]`.
- Sigue el formato [Keep a Changelog](https://keepachangelog.com/es/1.1.0/):
  agrupa las entradas en `Añadido`, `Cambiado`, `Eliminado`, etc.
- **Al completar una tarea de un `feat` o de un hito**, actualiza `docs/roadmap.md`
  (marca la casilla correspondiente) y el «Estado actual» de este archivo para que
  ambos reflejen lo que ya está implementado.

### Al compilar
- Compila con `cmake -S . -B build && cmake --build build`.
- Los warnings están habilitados: `-Wall -Wextra -Wpedantic`. **No introduzcas warnings nuevos.**
- SFML se descarga sola la primera vez; después se cachea en `build/_deps/`.

---

## Documentación disponible

| Documento    | Ruta                   | Contenido                                                |
| --------------| ------------------------| ----------------------------------------------------------|
| README       | `README.md`            | Presentación, compilación, controles, estructura         |
| Arquitectura | `docs/architecture.md` | Diagramas, módulos, flujo de movimiento, modelo de datos |
| Convenciones | `docs/conventions.md`  | Commits, ramas, PRs, estilo de código                    |
| Roadmap      | `docs/roadmap.md`      | Hitos priorizados con tareas concretas                   |
| Ideas        | `docs/ideas.md`        | Ideas exploratorias sin comprometer                      |
| Changelog    | `CHANGELOG.md`         | Registro de cambios por versión                          |
