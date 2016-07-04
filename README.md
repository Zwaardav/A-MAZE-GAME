# A-MAZE-GAME
A maze game with enemies, barriers and warps for TI-83+/TI-84+ in assembly

## Files
File | Description
--- | ---
AMAZEGME.8XP | An assembled binary that can be run using the `Asm(` from the catalog
AMAZEGME.z80 | The main source file
i/amaze/moveplayer.inc | Handles player movement and collision detection
i/amaze/enemies.inc | Handles enemy movement and display
i/amaze/codeinput.inc | Handles continue passwords
i/bytemod.inc | Used for modulo calculations

## Level editor
The level editor is built for the Love2d framework, and requires either Love2d 0.9.x or 0.10.x to work.
