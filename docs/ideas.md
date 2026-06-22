# Ideas a futuro

Propuestas exploratorias que ampliarían significativamente el alcance del proyecto. Estas
ideas **no están comprometidas** en el desarrollo actual y se evaluarán cuando los hitos
del [roadmap](roadmap.md) estén completados.

---

## 🤖 Integración de Motores UCI (IA Avanzada)

- Actualmente contamos con nuestro propio motor "Escafandrín" basado en Minimax. 
- Como paso futuro, podríamos preparar SimplyChess para comunicarse con motores open source de alto nivel competitivo como **Stockfish**.
- Usar el protocolo **UCI** (*Universal Chess Interface*) para enviar/recibir movimientos vía entrada/salida estándar.

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

---

## 🎨 Personalización estética

- **Distintas skins:** Permitir cambiar el aspecto visual del tablero y las piezas mediante temas seleccionables (ej. estilo clásico, madera, minimalista, neón).
- **Logotipo personalizado:** Diseñar e incluir un logo único para el juego en el menú principal.

---

## 🌍 Internacionalización

- **Soporte multilenguaje:** Traducir los textos de la interfaz (menús, botones, mensajes de fin de partida) a múltiples idiomas, permitiendo seleccionar el idioma desde los Ajustes.

---

## 🎲 Modos de juego alternativos

- **Distribución inicial aleatoria (Chess960):** Añadir la opción de jugar con la variante Fischer Random, donde las piezas de la primera fila se colocan aleatoriamente siguiendo ciertas reglas.
- **Multijugador Cooperativo (Co-op):** Implementar un modo donde dos jugadores colaboran compartiendo piezas, o modos tipo "Pasapiezas" (Bughouse) si se decide expandir a más de un tablero simultáneo.

---

## 🖱️ Interfaz y Experiencia de Usuario (UX)

- **Entrada por teclado de los nombres:** Reemplazar los nombres fijos ("Player 1", "Player 2") por cuadros de texto que permitan a los jugadores escribir su propio nombre antes de empezar la partida.
