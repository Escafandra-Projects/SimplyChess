# Ideas a futuro

Propuestas exploratorias que ampliarían significativamente el alcance del proyecto. Estas
ideas **no están comprometidas** en el desarrollo actual y se evaluarán cuando los hitos
del [roadmap](roadmap.md) estén completados.

---

## 🤖 Inteligencia Artificial (modo solitario)

Añadir la capacidad de jugar contra el ordenador.

### Motor propio (Minimax)
- Implementar el algoritmo **Minimax** con evaluación heurística del tablero.
- Evaluación básica: asignar valores a las piezas (Peón=1, Caballo/Alfil=3, Torre=5,
  Dama=9) con bonificaciones por posiciones centrales y control del tablero.
- Optimización con **poda alfa-beta** para reducir el número de nodos evaluados y
  permitir mayor profundidad de búsqueda en tiempo razonable.

### Integración UCI (alternativa avanzada)
- En lugar de crear un motor desde cero, preparar SimplyChess para comunicarse con
  motores open source de alto nivel como **Stockfish**.
- Usar el protocolo **UCI** (*Universal Chess Interface*) para enviar/recibir
  movimientos vía entrada/salida estándar.
- Permitiría distintos niveles de dificultad configurando la profundidad del motor.

---

## 🌐 Multijugador en red

Expandir el juego al terreno online usando `sfml-network`.

### Partidas en red local (LAN)
- Implementar un modelo **cliente-servidor** donde un jugador hace de anfitrión (host)
  y el otro se conecta mediante dirección IP.

### Protocolo de comunicación
- Definir los paquetes de red para mantener los tableros sincronizados.
- Ejemplo de paquete:
  ```
  <MovePiece startX="2" startY="1" endX="2" endY="3">
  ```
- Gestionar la sincronización de turnos, desconexiones y reconexiones.

---

## 🧩 Módulo de puzzles

- Cargar posiciones predefinidas (FEN o formato propio) para que el jugador resuelva
  problemas de ajedrez (mate en 1, mate en 2, etc.).
- Base de datos de puzzles que se puedan navegar por dificultad.
- Validar la solución del jugador y mostrar retroalimentación.

---

## 🧪 Testing automatizado

- Crear tableros virtuales (posiciones de prueba) que se puedan cargar
  programáticamente para validar la lógica de movimiento.
- Automatizar la verificación de reglas especiales: enroque, al paso, coronación,
  jaque, jaque mate, ahogado.
- Opciones: scripts externos que interactúen con el motor, o un modo headless
  (sin ventana gráfica) para ejecutar suites de pruebas.

---

## 📖 Documentación de características

- Crear documentación detallada de todas las funcionalidades del proyecto,
  incluyendo guías de uso, capturas de pantalla y explicaciones de cada pantalla.
- Documentar el formato de los archivos de configuración con todos los valores
  posibles y sus efectos.
- Añadir una sección de FAQ o solución de problemas comunes.
