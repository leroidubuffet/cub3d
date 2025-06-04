/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: your_login <your_login@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 10:00:00 by your_login          #+#    #+#             */
/*   Updated: 2023/10/28 10:00:00 by your_login         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H

# include <stdlib.h> // For malloc, free
# include <stdio.h>  // For printf (for testing) and perror
# include <fcntl.h>  // For open
# include <unistd.h> // For read, close (used by GNL)
# include "mlx.h"   // MiniLibX header
# include "libft.h" // Libft header

// string.h is no longer needed if all string ops are via libft

// --- Constants ---
// BUFFER_SIZE for GNL is now defined in libft.h
# define SCREEN_WIDTH 1024 // Default screen width in pixels
# define SCREEN_HEIGHT 768 // Default screen height in pixels

// Keycodes for keyboard input handling. Differentiated for macOS and Linux.
# ifdef __APPLE__ // macOS keycodes
#  define KEY_ESC 53
#  define KEY_W 13
#  define KEY_A 0
#  define KEY_S 1
#  define KEY_D 2
#  define KEY_LEFT_ARROW 123
#  define KEY_RIGHT_ARROW 124
# else // Linux keycodes (X11)
#  define KEY_ESC 65307
#  define KEY_W 119
#  define KEY_A 97
#  define KEY_S 115
#  define KEY_D 100
#  define KEY_LEFT_ARROW 65361
#  define KEY_RIGHT_ARROW 65363
# endif

// Movement and Rotation Speeds for player control.
# define MOVE_SPEED 0.1    // Player movement speed in units per frame.
# define ROT_SPEED 0.05     // Player rotation speed in radians per frame.

// --- Data Structures ---

/**
 * @brief Holds the configuration data parsed from the .cub file.
 * This includes texture paths, colors, map layout, and player start info.
 */
typedef struct s_config
{
	char	*north_texture_path;  // Path to the north wall texture file.
	char	*south_texture_path;  // Path to the south wall texture file.
	char	*west_texture_path;   // Path to the west wall texture file.
	char	*east_texture_path;   // Path to the east wall texture file.
	int		floor_color_r;        // Red component of the floor color.
	int		floor_color_g;        // Green component of the floor color.
	int		floor_color_b;        // Blue component of the floor color.
	int		ceiling_color_r;      // Red component of the ceiling color.
	int		ceiling_color_g;      // Green component of the ceiling color.
	int		ceiling_color_b;      // Blue component of the ceiling color.
	char	**map_data;           // 2D array representing the game map layout.
	int		map_height;           // Height of the map grid (number of rows).
	int		map_width;            // Width of the map grid (number of columns).
	double	player_start_x;       // Player's starting X position on the map.
	double	player_start_y;       // Player's starting Y position on the map.
	char	player_orientation;   // Player's starting orientation ('N', 'S', 'E', 'W').
	int		player_found;         // Flag: 1 if player start found in map, 0 otherwise.
	int		north_texture_set;    // Flag: 1 if North texture path is parsed, 0 otherwise.
	int		south_texture_set;    // Flag: 1 if South texture path is parsed, 0 otherwise.
	int		west_texture_set;     // Flag: 1 if West texture path is parsed, 0 otherwise.
	int		east_texture_set;     // Flag: 1 if East texture path is parsed, 0 otherwise.
	int		floor_color_set;      // Flag: 1 if Floor color is parsed, 0 otherwise.
	int		ceiling_color_set;    // Flag: 1 if Ceiling color is parsed, 0 otherwise.
}	t_config;

/**
 * @brief Represents an image in MiniLibX.
 * Contains image pointer, pixel data address, and image properties.
 */
typedef struct s_img
{
	void	*img_ptr;         // Pointer to the MLX image structure.
	char	*addr;            // Pointer to the raw pixel data of the image.
	int		bits_per_pixel;   // Number of bits used to represent one pixel.
	int		line_length;      // Size of one horizontal line of the image in bytes.
	int		endian;           // Endianness of the pixel data (0 for little, 1 for big).
	int		width;            // Width of the image in pixels.
	int		height;           // Height of the image in pixels.
}	t_img;

/**
 * @brief Main game data structure.
 * Holds MLX pointers, game configuration, player state, and texture images.
 */
typedef struct s_game_data
{
	void		*mlx_ptr;         // Pointer to the MLX instance.
	void		*win_ptr;         // Pointer to the MLX window.
	t_config	config;           // Parsed game configuration from .cub file.
	t_img		screen_buffer;    // Image structure for the main screen buffer to draw on.
	int			screen_width;     // Current screen width (may differ from config if resized).
	int			screen_height;    // Current screen height.
	double		player_x;         // Player's current X position.
	double		player_y;         // Player's current Y position.
	double		dir_x;            // X component of the player's direction vector.
	double		dir_y;            // Y component of the player's direction vector.
	double		plane_x;          // X component of the camera plane vector (for raycasting).
	double		plane_y;          // Y component of the camera plane vector.
	// Wall textures loaded into t_img structures.
	t_img		north_texture;    // Loaded north wall texture.
	t_img		south_texture;    // Loaded south wall texture.
	t_img		east_texture;     // Loaded east wall texture.
	t_img		west_texture;     // Loaded west wall texture.
}	t_game_data;

// --- Function Prototypes ---

// --- Parser (parser.c) ---

/**
 * @brief Parses the .cub scene description file and populates the t_config struct.
 * @param filename Path to the .cub file.
 * @param config Pointer to a t_config struct to be filled with parsed data.
 * @return Returns 1 on successful parsing and validation, 0 on any error.
 *         Error messages are typically printed to stderr by the function.
 */
int		parse_cub_file(const char *filename, t_config *config);

/**
 * @brief Initializes a t_config struct to default values.
 * Sets pointers to NULL, numeric values to 0 or -1, and flags to 0.
 * @param config Pointer to the t_config struct to initialize.
 */
void	init_config(t_config *config);

/**
 * @brief Frees dynamically allocated memory within a t_config struct.
 * This includes texture paths and the map_data array. Sets freed pointers to NULL.
 * @param config Pointer to the t_config struct to be freed.
 */
void	free_config(t_config *config);

/**
 * @brief Validates the parsed map data stored in t_config.
 * Checks for invalid characters, ensures the map is closed, and a player start exists.
 * @param config Pointer to the t_config struct containing the map to validate.
 * @return Returns 1 if the map is valid, 0 otherwise.
 *         Error messages are printed to stderr for specific validation failures.
 */
int		validate_map(t_config *config);


// --- Map reading helpers (typically in parser.c or map_utils.c) ---

/**
 * @brief Node for a linked list used temporarily to store map lines during parsing.
 */
typedef struct s_map_line_node
{
	char					*line; // A single line of the map.
	struct s_map_line_node	*next; // Pointer to the next map line node.
}	t_map_line_node;

/**
 * @brief Adds a new line to the linked list of map lines.
 * Used during the initial parsing phase before converting to a 2D array.
 * @param head Pointer to the head of the map line linked list.
 * @param line Content of the line to add (will be duplicated).
 */
void	add_map_line(t_map_line_node **head, char *line);

/**
 * @brief Frees all nodes and their line content in a map line linked list.
 * @param head Pointer to the head of the map line linked list.
 */
void	free_map_lines(t_map_line_node *head);

/**
 * @brief Converts the linked list of map lines into a 2D char array (char**) in t_config.
 * Also calculates map_width and map_height. Lines are padded with spaces to ensure rectangularity.
 * @param head Pointer to the head of the map line linked list.
 * @param config Pointer to the t_config struct where the map array and dimensions will be stored.
 * @return Returns 1 on success, 0 on memory allocation failure.
 */
int		convert_map_lines_to_array(t_map_line_node *head, t_config *config);


// --- MLX and Hooks (mlx_setup.c) ---

/**
 * @brief Initializes the MiniLibX instance, window, and screen buffer image.
 * @param game Pointer to the main t_game_data struct.
 * @return Returns 1 on success, 0 on failure to initialize MLX or create window/image.
 */
int		init_mlx(t_game_data *game);

/**
 * @brief Cleans up MLX resources: destroys images, window, and display connection.
 * @param game Pointer to the t_game_data struct containing MLX resources.
 */
void	cleanup_mlx(t_game_data *game);

/**
 * @brief Handles key press events.
 * Triggers actions like player movement, rotation, or exiting the game.
 * @param keycode The code of the key that was pressed.
 * @param game Pointer to the t_game_data struct.
 * @return Returns 0 (standard for MLX hook functions).
 */
int		handle_keypress(int keycode, t_game_data *game);

/**
 * @brief Handles the window close event (e.g., clicking the window's 'X' button).
 * @param game Pointer to the t_game_data struct.
 * @return Returns 0, and typically triggers game exit.
 */
int		handle_window_close(t_game_data *game);

/**
 * @brief Sets up MLX event hooks for key presses, window close, and the main game loop.
 * @param game Pointer to the t_game_data struct.
 */
void	setup_hooks(t_game_data *game);

// --- Player Setup (player_setup.c) ---

/**
 * @brief Initializes the player's state based on parsed config.
 * Sets player position (player_x, player_y), direction vector (dir_x, dir_y),
 * and camera plane (plane_x, plane_y) according to player_start_x, player_start_y,
 * and player_orientation from t_config.
 * @param game Pointer to the t_game_data struct.
 */
void	init_player_state(t_game_data *game);

// --- Player Movement (player_movement.c) ---

/**
 * @brief Moves the player forward or backward based on their current direction.
 * Collision detection with walls is handled.
 * @param game Pointer to the t_game_data struct.
 * @param move_dir Direction multiplier: 1 for forward, -1 for backward.
 */
void	move_forward_backward(t_game_data *game, double move_dir);

/**
 * @brief Strafes the player left or right relative to their current direction.
 * Collision detection with walls is handled.
 * @param game Pointer to the t_game_data struct.
 * @param strafe_dir Direction multiplier: 1 for right, -1 for left.
 */
void	strafe_left_right(t_game_data *game, double strafe_dir);

/**
 * @brief Rotates the player (and their view) left or right.
 * Updates the direction vector and camera plane.
 * @param game Pointer to the t_game_data struct.
 * @param rot_angle_multiplier Rotation direction: 1 for right, -1 for left.
 *        Actual angle is rot_angle_multiplier * ROT_SPEED.
 */
void	rotate_player(t_game_data *game, double rot_angle_multiplier);

// --- Texture Loading (texture_loader.c) ---

/**
 * @brief Loads all wall textures (North, South, East, West) from paths in t_config.
 * Uses MLX's xpm_file_to_image and populates the t_img structures in t_game_data.
 * @param game Pointer to the t_game_data struct.
 * @return Returns 1 on success, 0 if any texture fails to load.
 */
int		load_all_textures(t_game_data *game);

/**
 * @brief Frees all loaded wall textures.
 * Destroys the MLX images for North, South, East, and West textures.
 * @param game Pointer to the t_game_data struct.
 */
void	free_all_textures(t_game_data *game);

// --- Raycasting (raycaster.c) ---

/**
 * @brief Performs raycasting for each vertical stripe of the screen to render the 3D view.
 * Calculates distances to walls and determines texture coordinates for drawing.
 * Results are typically drawn directly to the screen_buffer.
 * @param game Pointer to the t_game_data struct.
 */
void	cast_all_rays(t_game_data *game);

// --- Rendering (render.c) ---

/**
 * @brief Puts a pixel of a specified color at (x,y) coordinates in an t_img structure.
 * This is a utility function to draw directly into an image's pixel data.
 * @param img Pointer to the t_img structure (e.g., screen_buffer).
 * @param x X-coordinate of the pixel.
 * @param y Y-coordinate of the pixel.
 * @param color The color of the pixel (integer representation, e.g., 0x00RRGGBB).
 */
void	my_mlx_pixel_put(t_img *img, int x, int y, int color);

/**
 * @brief Converts RGB color components into a single integer representation.
 * @param r Red component (0-255).
 * @param g Green component (0-255).
 * @param b Blue component (0-255).
 * @return Integer representation of the color.
 */
int		rgb_to_int(int r, int g, int b);

/**
 * @brief Draws the floor and ceiling on the screen buffer.
 * Fills the top half of the buffer with the ceiling color and the bottom half
 * with the floor color, both specified in t_config.
 * @param game Pointer to the t_game_data struct.
 */
void	draw_floor_and_ceiling(t_game_data *game);

/**
 * @brief Main rendering loop function, called by MLX for each frame.
 * Typically, this function will call raycasting logic, draw elements,
 * and then put the completed image (screen_buffer) to the window.
 * @param game Pointer to the t_game_data struct.
 * @return Returns 0 (standard for MLX loop hook functions).
 */
int		render_next_frame(t_game_data *game);

#endif
