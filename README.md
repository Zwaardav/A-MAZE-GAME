# A-MAZE-GAME
A maze game with enemies, barriers and warps for TI-83+/TI-84+ in assembly

## Files
File | Description
--- | ---
AMAZEGME.8XP | An assembled binary that can be run using `Asm(` from the catalog
AMAZEGME.z80 | The main source file
i/amaze/moveplayer.inc | Handles player movement and collision detection
i/amaze/enemies.inc | Handles enemy movement and display
i/amaze/codeinput.inc | Handles continue passwords
i/bytemod.inc | Used for modulo calculations

## How to run
After sending AMAZEGME.8XP to your calculator, do the following:

1. Press 2nd+0 to open the catalog
2. Scroll to `Asm(` and select it
3. Press PRGM and select AMAZEGME, then press enter again

## Level editor
The level editor is built for the Love2d framework, and requires either Love2d 0.9.x or 0.10.x to work.
