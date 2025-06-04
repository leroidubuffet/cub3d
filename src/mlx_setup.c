/* ************************************************************************** */
/*                                                                            */
/*   mlx_setup.c                                                              */
/*                                                                            */
/*   Description:                                                             */
/*     This module handles the setup, teardown, and event hooking related to  */
/*     the MiniLibX graphics library. It includes functions for initializing  */
/*     the MLX connection, creating the game window, managing the screen      */
/*     buffer image, setting up event listeners (keyboard, window close),     */
/*     and cleaning up MLX resources upon exit.                               */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * @brief Handles the window close event (e.g., clicking the window's 'X' button).
 *
 * This function is registered as an MLX hook for the window destroy notification.
 * It ensures a graceful shutdown of the application by freeing all allocated
 * resources in the correct order (textures, MLX components, parsed configuration)
 * before exiting.
 *
 * @param game Pointer to the t_game_data struct containing all game state and resources.
 * @return int This function should ideally not return as it calls exit().
 *             MLX typically expects 0 from such hooks if they were to return.
 */
int	handle_window_close(t_game_data *game)
{
	printf("Window close event triggered (e.g., red cross clicked).\n");
	// Order of cleanup:
	// 1. Game-specific resources (textures, map data from config)
	// 2. MLX resources (window, images, display connection)
	free_all_textures(game);  // Free MLX images for textures
	free_config(&game->config); // Free parsed config data (paths, map_data array)
	cleanup_mlx(game);        // Free MLX window, screen buffer, and display connection

	printf("All resources freed. Exiting application.\n");
	exit(0); // Terminate the application successfully.
	return (0); // Should not be reached due to exit().
}

/**
 * @brief Handles key press events.
 *
 * This function is registered as an MLX hook for key press events.
 * It checks the `keycode` to determine the action to perform:
 * - ESC: Initiates graceful game exit (freeing all resources).
 * - W, A, S, D: Trigger player movement (forward, strafe left, backward, strafe right).
 * - Left/Right Arrow: Trigger player rotation.
 * After processing a movement/rotation key, the game state is updated, and the
 * next call to `render_next_frame` (the main loop hook) will draw the new view.
 *
 * @param keycode The integer code of the key that was pressed.
 * @param game Pointer to the t_game_data struct.
 * @return int Always returns 0, as is standard for MLX key hook functions.
 */
int	handle_keypress(int keycode, t_game_data *game)
{
	// printf("Key pressed: %d\n", keycode); // For debugging keycodes
	if (keycode == KEY_ESC)
	{
		printf("ESC key pressed. Initiating game exit.\n");
		// Perform full cleanup similar to handle_window_close
		free_all_textures(game);
		free_config(&game->config);
		cleanup_mlx(game);
		printf("All resources freed. Exiting application.\n");
		exit(0);
	}
	// Player movement keys
	if (keycode == KEY_W)
		move_forward_backward(game, 1.0); // Move forward
	if (keycode == KEY_S)
		move_forward_backward(game, -1.0); // Move backward
	if (keycode == KEY_A)
		strafe_left_right(game, -1.0);    // Strafe left
	if (keycode == KEY_D)
		strafe_left_right(game, 1.0);     // Strafe right
	// Player rotation keys
	if (keycode == KEY_LEFT_ARROW)
		rotate_player(game, -1.0); // Rotate left
	if (keycode == KEY_RIGHT_ARROW)
		rotate_player(game, 1.0);  // Rotate right

	// Note: After player state changes, the screen is not re-rendered here directly.
	// Instead, the `render_next_frame` function, registered with `mlx_loop_hook`,
	// will handle drawing the updated scene in the next iteration of the MLX loop.
	return (0);
}

/**
 * @brief Sets up MLX event hooks for the game.
 *
 * This function registers handlers for various events:
 * - Key press events (`mlx_hook` with event code 2 for `KeyPress`):
 *   Calls `handle_keypress` to process keyboard input.
 * - Window close/destroy events (`mlx_hook` with event code 17 for `DestroyNotify`):
 *   Calls `handle_window_close` for graceful exit when the window is closed.
 * - Main game loop (`mlx_loop_hook`):
 *   Calls `render_next_frame` repeatedly to update and draw the game screen.
 *
 * @param game Pointer to the t_game_data struct, which contains the MLX window pointer
 *             (`win_ptr`) and MLX instance pointer (`mlx_ptr`) needed for hooks.
 */
void	setup_hooks(t_game_data *game)
{
	// Hook for KeyPress events.
	// Event code 2: KeyPress
	// Mask 1L<<0: KeyPressMask
	mlx_hook(game->win_ptr, 2, 1L << 0, handle_keypress, game);

	// Hook for Window close event (e.g., clicking the 'X' button).
	// Event code 17: DestroyNotify (window destruction)
	// Mask 1L<<17: StructureNotifyMask or a more specific mask for window close.
	// Standard practice is to use a mask that includes window close events.
	mlx_hook(game->win_ptr, 17, (1L << 17), handle_window_close, game);

	// Register the main rendering function to be called repeatedly by MLX.
	// This function will drive the game's graphics updates.
	mlx_loop_hook(game->mlx_ptr, render_next_frame, game);
}

/**
 * @brief Initializes the screen buffer image.
 *
 * Creates a new MLX image (`mlx_new_image`) to be used as the primary drawing surface.
 * Then, retrieves the data address (`mlx_get_data_addr`) of this image, which allows
 * direct pixel manipulation (e.g., via `my_mlx_pixel_put`).
 * Details like bits per pixel, line length, and endianness are stored in game->screen_buffer.
 *
 * @param game Pointer to the t_game_data struct. The `mlx_ptr`, `screen_width`,
 *             and `screen_height` fields must be initialized. The `screen_buffer`
 *             field will be populated by this function.
 * @return 1 on successful creation and data address retrieval, 0 on failure.
 */
static int	init_screen_buffer(t_game_data *game)
{
	// Create a new image for the screen buffer.
	game->screen_buffer.img_ptr = mlx_new_image(game->mlx_ptr, \
												game->screen_width, \
												game->screen_height);
	if (!game->screen_buffer.img_ptr)
	{
		fprintf(stderr, "Error: mlx_new_image() failed for screen buffer.\n");
		return (0); // Failed to create the image.
	}
	// Get the data address of the image, allowing raw pixel access.
	// This also populates bits_per_pixel, line_length, and endian fields.
	game->screen_buffer.addr = mlx_get_data_addr(game->screen_buffer.img_ptr, \
										&game->screen_buffer.bits_per_pixel, \
										&game->screen_buffer.line_length, \
										&game->screen_buffer.endian);
	if (!game->screen_buffer.addr)
	{
		fprintf(stderr, "Error: mlx_get_data_addr() failed for screen buffer.\n");
		// If getting data address fails, destroy the created image to prevent leaks.
		mlx_destroy_image(game->mlx_ptr, game->screen_buffer.img_ptr);
		game->screen_buffer.img_ptr = NULL; // Mark as NULL to avoid double free.
		return (0); // Failed to get image data address.
	}
	// Store width and height in the t_img struct as well for convenience.
	game->screen_buffer.width = game->screen_width;
	game->screen_buffer.height = game->screen_height;
	return (1); // Screen buffer initialized successfully.
}

/**
 * @brief Initializes the MiniLibX environment.
 *
 * This involves:
 * 1. Initializing the connection to the graphics system (`mlx_init`).
 * 2. Setting the game's screen dimensions.
 * 3. Creating a new window (`mlx_new_window`).
 * 4. Initializing the screen buffer image (`init_screen_buffer`).
 * 5. Setting up event hooks (`setup_hooks`) for user input and rendering.
 *
 * @param game Pointer to the t_game_data struct to be populated with MLX pointers
 *             and screen information.
 * @return 1 on successful initialization of all components, 0 on any failure.
 *         Errors are printed to stderr for specific failures.
 */
int	init_mlx(t_game_data *game)
{
	// Initialize the MLX library instance.
	game->mlx_ptr = mlx_init();
	if (!game->mlx_ptr)
		return (fprintf(stderr, "Error: mlx_init() failed. Ensure display server is available.\n"), 0);

	// Set screen dimensions (can be from constants or config).
	game->screen_width = SCREEN_WIDTH;  // Using predefined constants.
	game->screen_height = SCREEN_HEIGHT;

	// Create a new window.
	game->win_ptr = mlx_new_window(game->mlx_ptr, game->screen_width, \
									game->screen_height, "cub3D");
	if (!game->win_ptr)
	{
		fprintf(stderr, "Error: mlx_new_window() failed.\n");
		// Note: Some MLX versions might require mlx_destroy_display(game->mlx_ptr) on Linux here.
		// As mlx_ptr is valid, but window creation failed.
		return (0);
	}

	// Create and set up the screen buffer image where graphics will be drawn.
	if (!init_screen_buffer(game))
	{
		// If screen buffer fails, destroy the window that was successfully created.
		mlx_destroy_window(game->mlx_ptr, game->win_ptr);
		game->win_ptr = NULL;
		// Potentially mlx_destroy_display here too for game->mlx_ptr.
		return (0);
	}

	// Set up event hooks (keyboard input, window close, render loop).
	setup_hooks(game);

	return (1); // MLX environment initialized successfully.
}

/**
 * @brief Cleans up MiniLibX resources.
 *
 * Destroys the screen buffer image and the game window.
 * Note: This function assumes `mlx_ptr` is valid if `img_ptr` or `win_ptr` are valid.
 * The MLX display connection itself (`mlx_ptr`) might need separate handling for
 * complete cleanup on some systems (e.g., `mlx_destroy_display`), which is
 * MiniLibX version/platform dependent.
 *
 * @param game Pointer to the t_game_data struct containing the MLX resources.
 */
void	cleanup_mlx(t_game_data *game)
{
	// Ensure mlx_ptr is valid before attempting to destroy images or windows.
	if (game->mlx_ptr)
	{
		// Destroy the screen buffer image if it exists.
		if (game->screen_buffer.img_ptr)
		{
			mlx_destroy_image(game->mlx_ptr, game->screen_buffer.img_ptr);
			game->screen_buffer.img_ptr = NULL; // Mark as NULL to prevent double free.
			game->screen_buffer.addr = NULL;    // Invalidate data address.
		}
		// Destroy the window if it exists.
		if (game->win_ptr)
		{
			mlx_destroy_window(game->mlx_ptr, game->win_ptr);
			game->win_ptr = NULL; // Mark as NULL.
		}
		// For some MiniLibX versions (especially on Linux), the display connection
		// itself needs to be closed and the mlx_ptr freed.
		// Example:
		// if (IS_LINUX_MLX) { // Hypothetical macro
		//    mlx_destroy_display(game->mlx_ptr);
		//    free(game->mlx_ptr); // mlx_ptr itself is allocated by mlx_init on Linux.
		// }
		// game->mlx_ptr = NULL; // Mark as NULL.
	}
	printf("MLX window, screen buffer, and display connection (if applicable) cleaned up.\n");
}
