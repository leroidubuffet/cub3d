# cub3D Design Overview

## 1. Introduction

This document provides a high-level overview of the cub3D project's architecture, its core components, and the fundamental logic behind its operation. It is intended to help developers understand the overall design and how different parts of the program interact.

## 2. Program Flow

The execution of cub3D can be broken down into three main stages: Initialization, Game Loop, and Termination.

### 2.1. Initialization

This stage prepares everything needed to run the game:

1.  **Parsing the `.cub` File:**
    *   The program reads the scene description file (e.g., `map.cub`) provided as a command-line argument.
    *   It extracts paths to wall textures (North, South, East, West).
    *   It parses RGB values for floor and ceiling colors.
    *   It reads and validates the 2D map layout, identifying walls, empty spaces, and the player's starting position/orientation.
    *   All this data is stored in the `t_config` structure.
2.  **Initializing MiniLibX:**
    *   A connection to the display server is established using `mlx_init()`.
    *   A game window is created using `mlx_new_window()`.
    *   An image buffer (`screen_buffer`) is created using `mlx_new_image()` to serve as the canvas for off-screen rendering.
3.  **Loading Textures:**
    *   The texture paths obtained from the `.cub` file are used to load XPM images into `t_img` structures using `mlx_xpm_file_to_image()`.
    *   The data addresses of these texture images are retrieved for later use in rendering.
4.  **Setting Up Player State:**
    *   The player's initial position (x, y coordinates) and viewing direction (angle) are set based on the 'N', 'S', 'E', or 'W' character found in the map.
    *   The player's direction vector and camera plane vector are initialized.

### 2.2. Game Loop

Once initialization is complete, the program enters the main game loop, which continuously handles events, updates game state, and renders the scene:

1.  **Handling User Input:**
    *   Keyboard events (key presses) are captured via MLX hooks (e.g., `mlx_key_hook` or `mlx_hook` for KeyPress).
    *   Actions like moving forward/backward, strafing, rotating, or exiting the game are triggered based on the input.
2.  **Updating Player State:**
    *   Based on user input, the player's position (x, y) is updated. Collision detection with walls is performed to prevent movement into solid blocks.
    *   The player's viewing direction vector and camera plane vector are updated if rotation input is received.
3.  **Performing Raycasting:**
    *   For each vertical column of pixels on the screen, a ray is cast from the player's current position and direction.
    *   The raycasting algorithm determines which wall the ray hits, the distance to that wall, and the exact point of intersection.
4.  **Rendering the Scene:**
    *   The floor and ceiling are drawn with their configured solid colors.
    *   Based on the raycasting results for each screen column, a vertical slice of the appropriate wall texture is drawn. The height of this slice is inversely proportional to the corrected wall distance.
    *   All rendering is done to the off-screen image buffer (`screen_buffer`).
5.  **Displaying the Image:**
    *   After the entire scene for the current frame has been rendered to the `screen_buffer`, this image is pushed to the window using `mlx_put_image_to_window()`, making it visible to the player.

### 2.3. Termination

The game loop continues until an exit condition is met (e.g., player presses the ESC key or closes the window):

1.  **Cleaning Up Resources:**
    *   All dynamically allocated memory is freed. This includes:
        *   MLX images for textures and the screen buffer (using `mlx_destroy_image()`).
        *   The MLX window (using `mlx_destroy_window()`).
        *   The MLX display connection (on some systems, e.g., `mlx_destroy_display()` on Linux).
        *   Memory allocated for the map data (`map_data` in `t_config`).
        *   Memory allocated for texture paths in `t_config`.
    *   The program then exits.

## 3. Core Modules and Responsibilities

The project is divided into several modules (source files), each with specific responsibilities:

*   **`main.c`:** Contains the `main()` function, which serves as the entry point. It orchestrates the overall program flow, including initialization, starting the game loop, and triggering final cleanup.
*   **`parser.c`:** Responsible for reading the `.cub` scene file. It parses texture paths, color definitions, and the map layout, validating the input against specified rules.
*   **`mlx_setup.c`:** Manages all interactions with the MiniLibX library. This includes initializing MLX, creating the game window, setting up event hooks (for keyboard input, window close events, and the main render loop), and cleaning up MLX resources.
*   **`player_setup.c`:** Initializes the player's attributes (position, initial direction vector, camera plane vector) based on the data parsed from the map in the `.cub` file.
*   **`player_movement.c`:** Implements the logic for player movement (forward, backward, strafing) and rotation in response to keyboard input. It also includes basic collision detection with walls.
*   **`texture_loader.c`:** Handles the loading of XPM texture files specified in the `.cub` file. It converts these files into MLX image structures that can be used for rendering.
*   **`raycaster.c`:** Implements the core raycasting algorithm. It takes the player's state and the map to calculate what is visible on the screen, determining wall distances and texture coordinates for each screen column.
*   **`render.c`:** Responsible for drawing the game scene. This includes drawing the floor and ceiling, and using the raycasting results to draw textured wall slices onto an image buffer. It also handles putting the final rendered image to the window.

## 4. Main Data Structures

Key data structures are defined in `includes/cub3d.h` to manage game state and configuration:

*   **`t_config`:**
    *   **Purpose:** Stores all the configuration data parsed from the `.cub` file.
    *   **Key Fields:**
        *   `north_texture_path`, `south_texture_path`, `west_texture_path`, `east_texture_path`: Strings holding paths to wall texture files.
        *   `floor_color_r, _g, _b`, `ceiling_color_r, _g, _b`: Integer components for floor and ceiling RGB colors.
        *   `map_data`: A 2D array of characters (`char**`) representing the game map layout.
        *   `map_width`, `map_height`: Dimensions of the map grid.
        *   `player_start_x`, `player_start_y`, `player_orientation`: Initial player position and view direction.
        *   Flags like `north_texture_set`, `floor_color_set` to track if elements were parsed.

*   **`t_img`:**
    *   **Purpose:** Represents an image or texture within the MiniLibX context. It holds the MLX image pointer, metadata, and a pointer to the raw pixel data.
    *   **Key Fields:**
        *   `img_ptr`: Void pointer to the MLX image object.
        *   `addr`: Char pointer to the raw pixel data of the image.
        *   `bits_per_pixel`: Number of bits used to represent one pixel.
        *   `line_length`: The size (in bytes) of one horizontal line of the image.
        *   `endian`: The endianness of the pixel data.
        *   `width`, `height`: Dimensions of the image in pixels.

*   **`t_game_data`:**
    *   **Purpose:** The central structure that holds all runtime game state. It acts as a container for MLX pointers, the parsed configuration, current player data, and loaded textures. It's typically passed around to most game functions.
    *   **Key Fields:**
        *   `mlx_ptr`, `win_ptr`: Pointers for the MLX instance and game window.
        *   `config`: An instance of `t_config` holding the parsed scene configuration.
        *   `screen_buffer`: A `t_img` structure for the off-screen buffer where frames are rendered before display.
        *   `screen_width`, `screen_height`: Dimensions of the game window.
        *   `player_x`, `player_y`: Player's current floating-point coordinates.
        *   `dir_x`, `dir_y`: Components of the player's current direction vector.
        *   `plane_x`, `plane_y`: Components of the player's camera plane vector.
        *   `north_texture`, `south_texture`, `west_texture`, `east_texture`: `t_img` structures for the loaded wall textures.

## 5. Raycasting Logic Overview

The raycasting algorithm creates a pseudo-3D view from a 2D map. The process, simplified, is as follows:

1.  **Iterate Through Screen Columns:** For each vertical column `x` of pixels on the screen (from 0 to `screen_width - 1`):
2.  **Calculate Ray Direction:**
    *   Determine the `camera_x` coordinate, which represents the x-position on the camera plane corresponding to the current screen column `x`. This ranges from -1 (left edge of view) to 1 (right edge).
    *   The ray's direction vector is calculated by adding the player's direction vector to the camera plane vector scaled by `camera_x`. `ray_dir = player_dir + camera_plane * camera_x`.
3.  **DDA (Digital Differential Analysis):**
    *   The ray starts at the player's map grid cell.
    *   The DDA algorithm iteratively steps the ray from one grid line to the next (horizontal or vertical).
    *   In each step, it checks if the current grid cell the ray is in is a wall (`'1'`).
    *   This continues until a wall is hit. The algorithm also tracks which side of the wall was hit (e.g., North/South face vs. East/West face).
4.  **Calculate Wall Distance:**
    *   Once a wall is hit, the perpendicular distance from the player to the wall is calculated. This is important to prevent "fisheye" distortion that would result from using the Euclidean distance.
5.  **Determine Wall Slice Height:**
    *   The height of the vertical wall slice to be drawn on the screen for the current column `x` is inversely proportional to the `perp_wall_dist`. Closer walls appear taller.
6.  **Texture Mapping:**
    *   Identify which wall texture to use (North, South, East, or West) based on the ray's direction and which side of the wall block was hit by the DDA.
    *   Calculate the exact horizontal coordinate (`wall_x`) where the ray intersected the wall block (a value between 0.0 and 1.0).
    *   This `wall_x` is then mapped to a specific vertical column (`texture_x`) of the chosen texture image.
7.  **Draw the Wall Slice:**
    *   Draw the calculated vertical slice of the texture onto the screen column `x` in the off-screen buffer. The pixels are scaled vertically based on the calculated `line_height`.

## 6. Rendering Process

Rendering a single frame involves these steps:

1.  **Draw Floor and Ceiling:** The top half of the `screen_buffer` is filled with the specified ceiling color, and the bottom half is filled with the floor color. This also serves to clear the previous frame's wall renderings.
2.  **Cast Rays and Draw Walls:** The `cast_all_rays()` function is called. This function executes the raycasting logic described above for each screen column. As each ray determines a wall hit, the corresponding textured vertical wall slice is drawn directly into the `screen_buffer`.
3.  **Display Frame:** Once the floor, ceiling, and all wall slices have been drawn to the `screen_buffer`, the entire contents of this off-screen buffer are pushed to the game window using `mlx_put_image_to_window()`, making the rendered frame visible to the player. This process is repeated for every frame, creating the illusion of movement and a 3D environment.
