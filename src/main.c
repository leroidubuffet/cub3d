/* ************************************************************************** */
/*                                                                            */
/*   main.c                                                                     */
/*                                                                            */
/*   Description:                                                             */
/*     This module serves as the entry point for the cub3D application. It    */
/*     is responsible for initializing the game state, including parsing the  */
/*     scene file, setting up the MiniLibX environment, loading textures,     */
/*     and starting the game loop. It also handles overall error management   */
/*     and resource cleanup.                                                  */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <stdio.h>

/**
 * @brief Handles errors that occur during the initialization phase (e.g., MLX setup).
 *
 * This function is called if a critical error occurs after the game configuration
 * has been successfully parsed but before the game loop starts (e.g., if MiniLibX
 * initialization fails). Its primary role is to ensure that any resources
 * allocated up to that point (like the parsed configuration data) are freed
 * before exiting the application.
 *
 * @param game Pointer to the t_game_data struct. Although MLX components might be
 *             partially initialized or NULL, game->config is expected to be valid
 *             if parsing was successful.
 */
void	handle_init_error(t_game_data *game)
{
	fprintf(stderr, "Error during initialization. Cleaning up.\n");
	// Free the parsed configuration data, as this is definitely allocated if
	// parsing was successful before MLX initialization failed.
	free_config(&game->config);
	// Note: More specific MLX cleanup for mlx_ptr (if partially initialized)
	// might be needed depending on the MLX version and failure point.
	// For example, mlx_destroy_display(game->mlx_ptr) on Linux if mlx_ptr
	// was set but mlx_new_window failed. cleanup_mlx should be robust.
	exit(1); // Exit the application with an error status.
}

/**
 * @brief Main function for the cub3D application.
 *
 * Orchestrates the entire lifecycle of the game:
 * 1. Validates command-line arguments.
 * 2. Parses the scene description file (.cub).
 * 3. Initializes the player's state (position, direction).
 * 4. Initializes the MiniLibX graphics system.
 * 5. Loads wall textures.
 * 6. Sets up event hooks (keyboard, window close, render loop).
 * 7. Starts the MiniLibX event loop.
 * 8. Performs cleanup of all resources upon exiting the loop.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings. Expected: program name and path to .cub file.
 * @return Returns 0 on successful execution and normal exit, 1 on any error.
 */
int	main(int argc, char **argv)
{
	t_game_data	game; // Main struct to hold all game-related data.

	// Initialize critical pointers in t_game_data to NULL.
	// This is important for ensuring that cleanup functions (e.g., cleanup_mlx,
	// free_all_textures, free_config) can safely check for NULL before trying
	// to free or destroy resources, especially if an error occurs early.
	game.mlx_ptr = NULL;
	game.win_ptr = NULL;
	// game.config is a struct, not a pointer. Its members, including any
	// internal pointers, are initialized by init_config() which is called
	// at the beginning of parse_cub_file().

	// Validate command-line arguments: exactly one argument (the map file) is expected.
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <map_file.cub>\n", argv[0]);
		return (1);
	}

	// Parse the .cub file. This populates game.config.
	// parse_cub_file calls init_config internally and free_config on error.
	if (!parse_cub_file(argv[1], &game.config))
	{
		// Error messages are printed by the parser functions.
		// No need to call free_config here as parse_cub_file handles it on failure.
		return (1);
	}

	// --- Debug: Print parsed configuration (can be removed or conditionalized for final version) ---
	printf("Parsing successful!\n");
	printf("North Texture: %s\n", game.config.north_texture_path ? game.config.north_texture_path : "(null)");
	printf("South Texture: %s\n", game.config.south_texture_path ? game.config.south_texture_path : "(null)");
	printf("West Texture: %s\n", game.config.west_texture_path ? game.config.west_texture_path : "(null)");
	printf("East Texture: %s\n", game.config.east_texture_path ? game.config.east_texture_path : "(null)");
	printf("Floor Color: R=%d, G=%d, B=%d\n", game.config.floor_color_r, game.config.floor_color_g, game.config.floor_color_b);
	printf("Ceiling Color: R=%d, G=%d, B=%d\n", game.config.ceiling_color_r, game.config.ceiling_color_g, game.config.ceiling_color_b);
	if (game.config.map_data) // Ensure map_data is not NULL before accessing player info from it
	{
		printf("Player Start in config: X=%.2f, Y=%.2f, Orientation=%c\n", \
			game.config.player_start_x, game.config.player_start_y, game.config.player_orientation);
	}
	// --- End Debug ---

	// Initialize player's position, direction, and camera plane based on parsed config.
	init_player_state(&game);
	// --- Debug: Print player initial state ---
	printf("Player Initial State:\n");
	printf("  Position: (X=%.2f, Y=%.2f)\n", game.player_x, game.player_y);
	printf("  Direction: (dir_X=%.2f, dir_Y=%.2f)\n", game.dir_x, game.dir_y);
	printf("  Camera Plane: (plane_X=%.2f, plane_Y=%.2f)\n", game.plane_x, game.plane_y);
	// --- End Debug ---

	// Initialize MiniLibX (graphics library). This sets up mlx_ptr, win_ptr, and screen_buffer.
	// It also calls setup_hooks() internally on success.
	if (!init_mlx(&game))
	{
		// init_mlx prints specific errors.
		// If init_mlx fails, it attempts some internal cleanup.
		// We must ensure game.config is freed as parsing was successful.
		handle_init_error(&game); // Calls free_config(&game.config) and exits.
	}
	printf("MiniLibX initialized. Window created. Hooks set up.\n");

	// Load wall textures from paths specified in game.config into MLX images.
	if (!load_all_textures(&game))
	{
		// load_all_textures prints specific errors and should free any partially loaded textures.
		fprintf(stderr, "Error: Failed to load one or more textures. Exiting.\n");
		// Perform full cleanup before exiting:
		cleanup_mlx(&game);       // Frees MLX window, screen buffer, and display connection.
		free_config(&game.config); // Frees paths and map data from config.
		exit(1);
	}
	printf("Textures loaded successfully.\n");

	// Start the MiniLibX event loop. This function blocks until the game exits
	// (e.g., by pressing ESC or closing the window).
	// Event handling (keypresses, rendering new frames) is managed by hooks
	// set up in setup_hooks (called by init_mlx).
	printf("Starting MLX event loop.\n");
	mlx_loop(game.mlx_ptr);

	// The code below is reached when mlx_loop exits.
	// This typically happens if the exit sequence (e.g., triggered by ESC key)
	// explicitly calls functions to free resources and then exits the loop
	// or the program. This section acts as a final safeguard for cleanup.
	printf("MLX loop finished. Performing final cleanup.\n");
	free_all_textures(&game); // Free texture MLX images.
	cleanup_mlx(&game);       // Free MLX window, screen buffer, and display.
	free_config(&game.config); // Free parsed configuration data (paths, map).

	return (0); // Successful exit.
}
