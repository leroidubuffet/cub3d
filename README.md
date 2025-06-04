# cub3D

## Overview

cub3D is a 3D graphics project that emulates the raycasting rendering technique popularized by the classic game Wolfenstein 3D. The player navigates a 2D map, and the game renders a pseudo-3D perspective of the environment. This project is a common assignment within the 42 school curriculum, designed to teach students about graphics programming, raycasting, texture mapping, and parsing complex configuration files.

## Features

*   **Raycasting Engine:** Renders a 3D perspective from a 2D map.
*   **Texture Mapping:** Walls are rendered with textures loaded from XPM files.
*   **`.cub` File Parser:** Parses scene descriptions, including textures, colors, and map layouts from custom `.cub` files.
*   **Player Movement:** Supports forward/backward movement, left/right strafing, and rotation.
*   **Collision Detection:** Basic collision detection with walls.
*   **Sprite Rendering (Not Implemented):** The original Wolfenstein 3D also featured sprites for objects and enemies, which is a common extension but not part of the base project requirements typically.

## Compilation

To compile the project, navigate to the project's root directory and run:

```bash
make
```
This will compile the `libft` library, then the main game sources, and create the `cub3D` executable.

## Running the Game

After successful compilation, run the game using the following command, providing a path to a valid `.cub` map file:

```bash
./cub3D maps/your_map.cub
```

For example, to run with a test map (if available, e.g., `maps/test.cub` or one of the maps in `maps/test_files/`):

```bash
./cub3D maps/test_files/valid_complex.cub
```

## Controls

Player movement and game controls are handled via keyboard input:

-   **W:** Move forward
-   **S:** Move backward
-   **A:** Strafe left
-   **D:** Strafe right
-   **Left Arrow:** Rotate view left
-   **Right Arrow:** Rotate view right
-   **ESC:** Exit the game

## `.cub` File Format

The scene is described in a `.cub` file, which has a specific format for defining textures, colors, and the map layout. The file is typically parsed from top to bottom. All configuration elements (textures and colors) must appear before the map layout.

### Texture Identifiers

Wall textures are specified using identifiers followed by the path to an XPM (`.xpm`) file. Paths are typically relative to the project root or where the executable is run.

-   `NO ./path_to_north_texture.xpm`: North wall texture.
-   `SO ./path_to_south_texture.xpm`: South wall texture.
-   `WE ./path_to_west_texture.xpm`: West wall texture.
-   `EA ./path_to_east_texture.xpm`: East wall texture.

Each texture identifier must appear exactly once.

### Color Identifiers

Floor and ceiling colors are specified using identifiers followed by RGB color values.

-   `F R,G,B`: Floor color.
-   `C R,G,B`: Ceiling color.

`R`, `G`, `B` are integer values for Red, Green, and Blue components, respectively. Each component must be in the range of 0-255. Example: `F 220,100,0`.
Each color identifier must appear exactly once.

### Map Layout

The map layout is the last element in the file. It is a grid composed of the following characters:

-   `0`: Empty, walkable space.
-   `1`: A wall block.
-   `N`, `S`, `E`, `W`: Player's starting position and initial viewing direction (North, South, East, West).

**Map Rules:**

-   The map must be the last section of the `.cub` file, following all texture and color definitions.
-   The map must be "closed" or "surrounded" by walls (`1`). This means a player starting within the map should not be able to reach an "empty" space that is outside the defined map grid by only moving through `0`s or player start positions.
-   Only the characters `0`, `1`, `N`, `S`, `E`, `W`, and space (` `) are allowed in the map section. Spaces are often used for padding irregular map shapes but should be treated as "outside" or potentially walls during validation if not part of a closed area.
-   There must be exactly one player starting position (`N`, `S`, `E`, or `W`) on the map.

### Example `.cub` File

```cub
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm

F 100,100,100
C 50,50,50

1111111111
1000100001
10N0100001
1000100001
1111111111
```

## Running Tests

The project includes parser tests to verify the `.cub` file parsing logic. To compile and run these tests:

```bash
make test
```
This command typically compiles a separate test executable (e.g., `test_cub3d`) and runs it.

## Project Structure

A brief overview of the main directories:

-   `src/`: Contains the C source files for the game logic (main, parser, raycaster, player movement, MLX setup, rendering, etc.).
-   `includes/`: Header files (`.h`), including `cub3d.h` (main project header) and `mlx.h` (MiniLibX header).
-   `libft/`: Contains the custom `libft` library, a collection of utility functions.
-   `maps/`: Directory for `.cub` scene files.
    -   `maps/test_files/`: Contains specific maps for parser testing.
-   `textures/`: Directory for XPM texture files.
-   `tests/`: Contains source files for unit tests (e.g., `test_parser.c`).
-   `minilibx/`: Contains the MiniLibX library files or source.
-   `Makefile`: Used to compile the project and tests.
-   `README.md`: This file.
```
