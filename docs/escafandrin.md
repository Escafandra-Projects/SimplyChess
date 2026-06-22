# Escafandrín: Motor de IA de SimplyChess

**Escafandrín** es el motor de ajedrez propietario integrado en SimplyChess, diseñado para proporcionar un desafío configurable y orgánico en las partidas para un jugador.

## Arquitectura del Motor

El motor se asienta sobre tres pilares de optimización que le permiten analizar miles de posiciones por segundo sin bloquear la interfaz de usuario:

### 1. FastBoard
Para maximizar la velocidad de cálculo, la IA no utiliza las clases visuales de SFML (`Board`, `Piece`). En su lugar, emplea `FastBoard`, una representación en memoria ultracompacta que guarda las casillas como enteros de 8 bits (`uint8_t`), además de los derechos de enroque y peón al paso.
Esto permite "clonar" tableros en microsegundos para simular jugadas a mucha profundidad.

### 2. MoveGen
El generador de movimientos puro evalúa `FastBoard` para generar todas las opciones legales. Calcula directamente jaques, enroques y colisiones de forma matemática, garantizando el respeto de las reglas oficiales a máxima velocidad.

### 3. Minimax con Poda Alfa-Beta
Escafandrín utiliza el algoritmo **Minimax** para buscar en el árbol de posibles movimientos futuros. La optimización principal es la **Poda Alfa-Beta**, que permite descartar rápidamente "ramas" enteras del árbol de decisiones (movimientos objetivamente peores) para ahorrar tiempo y calcular con mayor profundidad en el mismo plazo.

## Evaluación Heurística
Para decidir qué posición es mejor, el motor utiliza:
1. **Valor Material:** Dama=900, Torre=500, Alfil=330, Caballo=320, Peón=100.
2. **Tablas Piece-Square (PST):** Un sistema de bonificaciones y penalizaciones por casilla. Por ejemplo, premia a los peones por avanzar al centro y penaliza a los caballos por estar en las esquinas.
3. **Penalización por mate lejano:** Fomenta dar jaque mate en el menor número de movimientos posibles.

## Comportamiento "Humano" (Niveles de Dificultad)
Escafandrín incorpora "emoción" para evitar ser un ente puramente calculador en niveles bajos:
- **Modo Easy (Profundidad 2):** Escoge aleatoriamente uno de los mejores movimientos detectados siempre que la diferencia de puntos no sea catastrófica. Ocasionalmente arriesgará o variará sus aperturas.
- **Modo Normal (Profundidad 4):** Un nivel más equilibrado. En un 20% de las ocasiones no escogerá el movimiento teóricamente óptimo, dejándose "sorprender" ligeramente, favoreciendo tácticas menos ortodoxas.
- **Modo Hard (Profundidad 5+):** Implacable. Jugará siempre el mejor movimiento encontrado por el Minimax en su máxima profundidad, ideal para jugadores avanzados.

## Multithreading
Para evitar la congelación de los gráficos y animaciones (bloqueo del hilo principal), el análisis de Escafandrín se lanza utilizando `std::async`. El hilo principal consulta continuamente un objeto `std::future`, y en el momento en el que Escafandrín termina de pensar, su jugada se integra en la partida a través del sistema de animaciones.
