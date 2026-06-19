# Changelog

Todos los cambios relevantes del proyecto se documentan en este archivo.

El formato sigue [Keep a Changelog](https://keepachangelog.com/es/1.1.0/) y el proyecto
usa [Semantic Versioning](https://semver.org/lang/es/).

---

## [Sin publicar]

### Añadido
- **Iconos de Aplicación**: Se han incorporado los iconos nativos para los ejecutables de release (icono de peón `PeonN`).
  - En **Windows** (`.ico`), el icono se incrusta en el `.exe` mediante el compilador de recursos `windres` en CMake.
  - En **macOS** (`.icns`), la aplicación se genera ahora como un **App Bundle** nativo (`SimplyChess.app`), lo que habilita la asignación del icono en el sistema y soluciona problemas de permisos de teclado.
- Soporte para eventos de teclado basados en cola de eventos (`handleEvent`) en la arquitectura de estados, mejorando la compatibilidad con macOS.

### Corregido
- Arreglada la distribución en macOS: el ejecutable ahora resuelve las rutas de los recursos correctamente independientemente del directorio de ejecución, y el paquete de release es más ligero al incluir solo el framework necesario (`freetype`).
- La tecla **Escape** para pausar/reanudar la partida ahora funciona correctamente en macOS gracias a la refactorización de la lectura de teclado (usando la cola de eventos en lugar de la consulta de estado global, la cual fallaba por restricciones de permisos en los App Bundles).

---

## [1.0.0] — 2026-06-18

Primera versión estable: el juego implementa todas las reglas oficiales del ajedrez,
incluyendo las condiciones de tablas, además de un reloj de ajedrez configurable.

### Añadido
- **Sonido al coronar**: reproduce el efecto de jaque o de fin de partida cuando la
  pieza resultante de una coronación da jaque, jaque mate o provoca ahogado.
- **Mejoras visuales (Hito 2)**:
  - **Animaciones de movimiento**: Las piezas se desplazan de forma suave (interpolación lineal) en lugar de teletransportarse instantáneamente.
  - **Efectos de sonido**: Añadida la biblioteca `sfml-audio` y un `AudioSystem` para reproducir sonidos al mover, capturar, dar jaque o finalizar la partida.
  - **Mover arrastrando (drag & drop)**: además del modo de dos clics, ahora se puede mover una pieza arrastrándola con el ratón y soltándola en la casilla destino.
  - **Resaltado de movimientos legales**: Al seleccionar una pieza, muestra las casillas disponibles con un punto gris oscuro y las capturas con un aro alrededor de la pieza enemiga (incluyendo captura al paso).
  - **Resaltado del último movimiento**: Marca las casillas de origen y destino del último movimiento con un fondo sutil.
- **Coordenadas en el tablero**: nueva imagen de tablero con los números (1-8) a la
  izquierda y las letras (A-H) abajo.
- **Reglas de tablas (Hito 1 completado):**
  - Detección de **ahogado (stalemate)**: tablas cuando el jugador en turno no está en
    jaque pero no tiene ningún movimiento legal (antes el juego se bloqueaba).
  - **Regla de los 50 movimientos**: tablas tras 50 movimientos consecutivos sin captura
    ni movimiento de peón.
  - **Triple repetición de posición**: tablas cuando la misma posición se repite tres veces.
- **Reloj de Ajedrez**: Lógica completa para tiempo base e incremento por turno, mostrándose la cuenta atrás y provocando el fin de la partida por "Time Out" (`TIMEOUT`).
- **Menú de Ajustes**: Nueva ventana (accesible desde el Menú Principal) que permite configurar el tiempo base (ej. 3 min, 5 min, 10 min, etc.) y el incremento (ej. 0 sec, 2 sec, 5 sec, etc.).
- Funcionalidad para pausar automáticamente el tiempo del jugador activo mientras éste se encuentra moviendo la pieza seleccionada o en el proceso de elegir una pieza de coronación.
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
- Mensaje de fin de partida centrado en pantalla.
- Ventana centrada al iniciar y título de la ventana actualizado.
- Homogeneizado el tiempo de respuesta (debounce) de la UI para todos los botones en 30.0f (menús más suaves y controlados).

### Corregido
- Solucionado un error que impedía detectar el jaque (y el jaque mate / ahogado) cuando lo provocaba la pieza resultante de coronar un peón.
- El botón **Back** de Ajustes ya no inicia la partida al volver: el menú principal solo
  actúa ante un clic nuevo (flanco de pulsación), evitando que el clic mantenido se cuele
  en el botón *Start* que queda debajo.
- El **reloj** ya no empieza a descontar antes del primer movimiento: arranca cuando las
  blancas completan su jugada inicial.
- El **reloj** ya no se pausa al seleccionar/mover una pieza; sigue corriendo mientras el
  jugador piensa su jugada (solo se detiene durante la coronación).
- **Compilación con MSVC/Windows**: los flags de warnings de GCC/Clang (`-Wextra`,
  `-Wpedantic`) ya no abortan el build con Visual Studio; en MSVC se usa `/W4`.

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
