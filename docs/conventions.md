# Convenciones del proyecto

Este documento establece las convenciones de código, estilo, control de versiones y flujo
de trabajo del proyecto SimplyChess. Todos los contribuidores (humanos e IAs) deben
seguirlas.

---

## Idioma

| Contexto | Idioma | Ejemplo |
|---|---|---|
| Comentarios del código | Español | `/// Valida el movimiento del rey.` |
| Documentación (README, docs/) | Español | — |
| Variables y métodos de dominio | Español | `peonPaso`, `jaque`, `coronación` |
| Variables y métodos genéricos | Inglés | `update`, `render`, `isActive`, `getPosition` |
| Nombres de clases | Inglés | `Board`, `Piece`, `GameState`, `Button` |
| Enums | Español (dominio) / Inglés (UI) | `PieceType::CABALLO`, `ButtonStates::BTN_HOVER` |
| Mensajes de error (excepciones) | Inglés | `"ERROR::GAME_STATE::FAILED TO LOAD"` |
| Texto visible en la UI | Inglés | `"Start"`, `"Pause"`, `"Exit"` |
| Mensajes de commit | Español (descripción) | `feat(board): añadir detección de ahogado` |

---

## Estilo de código

### Comentarios

**En headers (`.h`)** — Usar `///` estilo Doxygen antes de cada clase, método público
y los privados relevantes:

```cpp
/// Valida si el movimiento es legal para el tipo de esta pieza.
bool checkMove(bool turn, sf::Vector2i startPos, sf::Vector2i desPos,
               BoardGrid& board, CastlingState& castling, EnPassantState& peonPaso);
```

**En fuentes (`.cpp`)** — Usar `//` inline para explicar la lógica:

```cpp
// Si el peón avanza dos casillas, activar elegibilidad de captura al paso
if (std::abs(desPos.y - startPos.y) == 2) {
    peonPaso[desPos.x][turn] = true;
}
```

### Organización de clases

Dentro de cada clase, seguir este orden de secciones:

```cpp
class MiClase {
private:
    // Variables
    /* Agrupación lógica */
    int miVariable;

    // Inicialización
    /// Descripción del método.
    void initVariables();

public:
    // Constructor y destructor
    MiClase();
    virtual ~MiClase();

    // Funciones (o Métodos de consulta)
    /// Descripción del método.
    void update();
    void render();
};
```

### Headers

- Usar `#pragma once` como guard de inclusión (no `#ifndef`).
- Incluir cabeceras del proyecto con ruta relativa al grupo:
  ```cpp
  #include "ui/Button.h"
  #include "chess/Piece.h"
  ```

### Archivos nuevos

Al crear archivos nuevos:
- Colocarlos en la subcarpeta correspondiente (`core/`, `states/`, `ui/`, `chess/`).
- Los `.h` van en `include/<subcarpeta>/` y los `.cpp` en `src/<subcarpeta>/`.
- CMake los detecta automáticamente gracias a `GLOB_RECURSE`.

### Compilación

- Los warnings están habilitados: `-Wall -Wextra -Wpedantic`.
- **No introducir warnings nuevos.** Todo código debe compilar limpio.

---

## Control de versiones (Git)

### Commits — Conventional Commits

Formato obligatorio:

```
tipo(scope): descripción en español
```

La descripción debe:
- Empezar en **minúscula**.
- Usar **infinitivo** (añadir, corregir, refactorizar — no "añadido" ni "añade").
- Ser concisa pero descriptiva.
- **No** terminar en punto.

#### Tipos válidos

| Tipo | Uso | Ejemplo |
|---|---|---|
| `feat` | Nueva funcionalidad | `feat(board): añadir detección de ahogado` |
| `fix` | Corrección de bug | `fix(piece): corregir validación del enroque largo` |
| `refactor` | Reestructuración sin cambio de comportamiento | `refactor(ui): extraer lógica de centrado de texto` |
| `docs` | Cambios en documentación | `docs(docs): actualizar roadmap con hito 2` |
| `style` | Formato, espaciado, sin cambios lógicos | `style(board): reformatear condicionales de checkMove` |
| `chore` | Mantenimiento (CMake, .gitignore, CI) | `chore(build): actualizar versión de SFML a 2.6.2` |

#### Scopes válidos

| Scope | Área del proyecto |
|---|---|
| `board` | Lógica del tablero (`chess/Board`) |
| `piece` | Lógica de piezas (`chess/Piece`) |
| `ui` | Interfaz — botones, menús (`ui/`) |
| `game` | Bucle principal, ventana (`core/Game`) |
| `states` | Estados del juego (`states/`) |
| `config` | Archivos de configuración (`config/`) |
| `docs` | Documentación (`docs/`, README, etc.) |
| `build` | CMake, compilación |
| `resources` | Imágenes, fuentes (`resources/`) |

### Ramas

Formato:

```
tipo/descripcion-corta
```

- Usar los mismos tipos que en los commits: `feat/`, `fix/`, `refactor/`, `docs/`, `style/`, `chore/`.
- Descripción en minúsculas, separada por guiones.
- Sin acentos ni caracteres especiales.

**Ejemplos:**

```
feat/deteccion-ahogado
fix/enroque-largo
refactor/representacion-tablero
docs/arquitectura
chore/actualizar-sfml
```

### Pull Requests

| Regla | Valor |
|---|---|
| Estrategia de merge | **Squash & Merge** |
| Aprobadores mínimos | **1 (distinto al autor)** |
| Formato del título | Sigue el formato de commit |

El título del PR se convierte en el commit que queda en `main`. Esto mantiene el
historial de la rama principal limpio, con un commit por PR.

**Ejemplo:**
- Rama: `feat/deteccion-ahogado`
- Título del PR: `feat(board): añadir detección de ahogado`
- Resultado en `main`: un único commit con ese mensaje.

---

## Versionado — Semantic Versioning

Formato: `MAJOR.MINOR.PATCH`

| Componente | Cuándo incrementar |
|---|---|
| `MAJOR` | Cambios incompatibles o hitos mayores del proyecto |
| `MINOR` | Nuevas funcionalidades compatibles |
| `PATCH` | Correcciones de bugs |

**Política actual:**
- El proyecto se mantiene en **`v0.x`** mientras las reglas del ajedrez no estén
  completas.
- **`v1.0.0`** se publicará cuando el juego implemente todas las reglas oficiales
  (jaque mate, ahogado, tablas por 50 movimientos y triple repetición).
- El registro de cambios se documenta en [`CHANGELOG.md`](../CHANGELOG.md).
