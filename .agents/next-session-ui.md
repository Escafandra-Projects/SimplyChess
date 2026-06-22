# Próxima sesión: homogeneizar y pulir UI

## Estado actual (rama `feat/menu-dark-wood-redesign`)

El **menú principal** está muy bien — sirve como referencia de calidad estética:
- Fondo madera oscura con viñeta radial
- Panel central con borde dorado, sombra, marco interior
- Logo (peón negro) + título con letra-spacing amplio
- Separador ornamental con diamante dorado
- Botones `MenuButton` con bisel 3D, gradiente madera, hover/active
- Escafandra pequeña + crédito "Made by Escafandra Projects" abajo derecha
- Fade-in escalonado del panel y botones

El **Game Screen** no alcanzó ese nivel. Necesita otro pase completo.

---

## Referencias de diseño

El proyecto de diseño está en **Claude Design**, accesible via MCP (`DesignSync`):
- **Project ID:** `77d770d3-87a3-46bb-9823-62558b8daec4`
- **Archivo menú:** `Menu Screen.dc.html` ← referencia de calidad ya implementada
- **Archivo partida:** `Game Screen.dc.html` ← pendiente de aplicar bien

Para importarlos: usar `DesignSync` con `method: get_file` y el `projectId` y `path` indicados.

---

## Pendiente: Game Screen

El diseño original (`Game Screen.dc.html`) define:

### Layout
- Columna izquierda (55% ancho): tablero centrado + label "NEGRAS" arriba / "BLANCAS" abajo
- Columna derecha (45% ancho): panel de madera igual que el menú

### Panel derecho (de arriba a abajo)
1. **Fila jugador negro** — fondo oscuro semi-transparente, caja con ♚, nombre, estado, timer
2. **Piezas capturadas por negras** — símbolos unicode de piezas + ventaja (+N)
3. **Lista de movimientos** — fondo transparente, 3 columnas (nº, blancas, negras), scroll
4. **Piezas capturadas por blancas** — igual que negras
5. **Fila jugador blanco** — borde izquierdo dorado cuando activo, ♔, timer con glow pulsante
6. **Botones acción** — RENDIRSE y OFRECIR TABLAS (mismo estilo `MenuButton`)

### Colores clave (copiar del diseño, no inventar)
- Panel madera: gradiente `#A88060 → #8A6240 → #987050` (igual que menú)
- Borde panel: `#3C1E0C`
- Marco interior: `rgba(208,158,82,0.18)`
- Jugador activo bg: `rgba(200,168,90,0.10)` + borde `rgba(202,168,90,0.28)`
- Barra activo: gradiente `#CAA448 → #A07830` (3px izquierda)
- Timer activo: `#CAAA58` con animación glow
- Timer inactivo: `rgba(210,175,110,0.48)`
- Líneas separadoras: `rgba(200,148,70,0.10)`
- Texto movimiento blancas: `rgba(235,215,170,0.75)`
- Texto movimiento negras: `rgba(200,175,130,0.62)`
- Nº movimiento: `rgba(200,160,100,0.36)`

### Problemas conocidos de la implementación actual
- El tablero (800px) y la ventana (1280×820) dejan poco espacio lateral — revisar si ajustar proporciones
- Las piezas capturadas no se rastrean en `Board` — actualmente solo hay puntos numéricos (`points1`, `points2`); para mostrar símbolos de piezas habría que añadir tracking en `Board::capturePiece`
- `OFRECIR TABLAS` sin funcionalidad real aún
- La lista de movimientos (`MoveListPanel`) necesita mejor integración visual dentro del panel

---

## Pendiente: Settings Screen y otros

- Settings screen: sin diseño en Claude Design, pero debería seguir el mismo tema madera
- Pause menu: actualmente usa fondo semi-transparente blanco — adaptar al tema oscuro
- MessageBox (game over): misma adaptación pendiente
- Promotion menu: posiblemente necesita actualización

---

## Estándar de calidad — usar el menú como referencia

Al implementar cualquier pantalla nueva:
1. Usar `Gameplay.ttf` (pixel font) para todo el texto del tema oscuro
2. Fondos con `sf::VertexArray` TriangleFan para viñeta radial
3. Paneles como `sf::RectangleShape` con `sf::Color(140,102,68)` y borde `sf::Color(60,30,12)` 3px
4. Marco interior dorado: `rgba(208,158,78,0.15)` 1px, inset 7px
5. Botones con clase `MenuButton` (bisel 3D, gradiente madera)
6. Letter-spacing amplio en todos los textos del panel
7. Animaciones de fade-in donde sea apropiado

---

## Rama de trabajo
`feat/menu-dark-wood-redesign` — commits hasta aquí:
- `feat(menu)`: rediseño completo menú
- `feat(game)`: primer pase Game Screen (incompleto)
- `feat(menu)`: crédito Escafandra Projects
