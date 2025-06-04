/* ************************************************************************** */
/*                                                                            */
/*   texture_loader.c                                                         */
/*                                                                            */
/*   Description:                                                             */
/*     This module is responsible for loading wall textures from XPM files    */
/*     into MiniLibX image structures. It provides functions to load          */
/*     individual textures and a main function to load all required wall      */
/*     textures (North, South, East, West) as specified in the game's         */
/*     configuration. It also includes functionality to free these texture    */
/*     images when they are no longer needed or if an error occurs during     */
/*     loading.                                                               */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h" // For t_game_data, t_img, etc.

/**
 * @brief Loads a single texture from an XPM file into a t_img structure.
 *
 * Takes the path to an XPM file, uses MiniLibX functions to load it into an
 * image, and then retrieves the image's data address for rendering.
 * Populates the width, height, bits_per_pixel, line_length, and endian fields
 * of the provided t_img struct.
 *
 * @param mlx_ptr Pointer to the MiniLibX instance.
 * @param path Path to the .xpm texture file.
 * @param texture_img Pointer to the t_img structure to be filled with texture data.
 * @return 1 on successful loading and data address retrieval, 0 on any failure.
 *         Prints error messages to stderr on failure.
 */
static int	load_single_texture(void *mlx_ptr, char *path, t_img *texture_img)
{
	// Validate that the texture path is provided.
	if (!path)
	{
		fprintf(stderr, "Error: Texture path is NULL. Cannot load texture.\n");
		return (0);
	}

	// Load the XPM file into an MLX image.
	// mlx_xpm_file_to_image also returns the width and height of the image.
	texture_img->img_ptr = mlx_xpm_file_to_image(mlx_ptr, path, \
											&texture_img->width, \
											&texture_img->height);
	if (!texture_img->img_ptr) // Check if image loading failed.
	{
		fprintf(stderr, "Error: Failed to load texture from path: %s\n", path);
		return (0);
	}

	// Get the data address of the loaded image.
	// This address allows direct access to the image's pixel data.
	// Also populates bits_per_pixel, line_length, and endian fields in texture_img.
	texture_img->addr = mlx_get_data_addr(texture_img->img_ptr, \
										&texture_img->bits_per_pixel, \
										&texture_img->line_length, \
										&texture_img->endian);
	if (!texture_img->addr) // Check if getting data address failed.
	{
		fprintf(stderr, "Error: mlx_get_data_addr failed for texture: %s\n", path);
		// If data address retrieval fails, destroy the image that was loaded.
		mlx_destroy_image(mlx_ptr, texture_img->img_ptr);
		texture_img->img_ptr = NULL; // Mark as NULL to prevent double free.
		return (0);
	}
	return (1); // Texture loaded successfully.
}

/**
 * @brief Frees all loaded wall textures (North, South, East, West).
 *
 * Iterates through the texture image pointers in the t_game_data struct.
 * If an image pointer is valid, it calls `mlx_destroy_image` to free the
 * MLX image resource. Pointers are then set to NULL to prevent accidental
 * use of freed memory or double-free attempts.
 *
 * @param game Pointer to the t_game_data struct containing the texture images.
 */
void	free_all_textures(t_game_data *game)
{
	// Ensure mlx_ptr is valid before trying to destroy images.
	if (!game->mlx_ptr)
	{
		printf("Warning: mlx_ptr is NULL in free_all_textures. Cannot free textures.\n");
		return;
	}

	// Free North texture if loaded.
	if (game->north_texture.img_ptr)
		mlx_destroy_image(game->mlx_ptr, game->north_texture.img_ptr);
	// Free South texture if loaded.
	if (game->south_texture.img_ptr)
		mlx_destroy_image(game->mlx_ptr, game->south_texture.img_ptr);
	// Free East texture if loaded.
	if (game->east_texture.img_ptr)
		mlx_destroy_image(game->mlx_ptr, game->east_texture.img_ptr);
	// Free West texture if loaded.
	if (game->west_texture.img_ptr)
		mlx_destroy_image(game->mlx_ptr, game->west_texture.img_ptr);

	// Set all image pointers to NULL after freeing to ensure they are not reused
	// and to make this function safe to call multiple times (idempotent).
	game->north_texture.img_ptr = NULL;
	game->south_texture.img_ptr = NULL;
	game->east_texture.img_ptr = NULL;
	game->west_texture.img_ptr = NULL;

	printf("All wall textures freed.\n");
}

/**
 * @brief Loads all wall textures (North, South, East, West) specified in the config.
 *
 * Initializes all texture `img_ptr` fields in `t_game_data` to NULL for robust
 * error handling in `free_all_textures` (in case of partial loading failure).
 * Then, sequentially calls `load_single_texture` for each of the four wall textures
 * using paths from `game->config`.
 *
 * IMPORTANT: The current implementation does NOT free previously loaded textures if a
 * subsequent texture fails to load. It simply returns 0. The caller (e.g., in main.c)
 * is responsible for calling `free_all_textures` and `free_config` for cleanup
 * if this function returns 0.
 *
 * @param game Pointer to the t_game_data struct. `game->config` must contain
 *             valid texture paths. The texture `t_img` structs within `game`
 *             will be populated.
 * @return 1 if all textures are loaded successfully, 0 if any texture fails to load.
 */
int	load_all_textures(t_game_data *game)
{
	// Initialize img_ptr to NULL for all textures before attempting to load.
	// This ensures that if loading fails partway, free_all_textures can be
	// safely called to clean up only those textures that were successfully loaded.
	game->north_texture.img_ptr = NULL;
	game->south_texture.img_ptr = NULL;
	game->east_texture.img_ptr = NULL;
	game->west_texture.img_ptr = NULL;

	// Load North texture.
	if (!load_single_texture(game->mlx_ptr, game->config.north_texture_path, \
								&game->north_texture))
		return (0); // On failure, return 0. Caller handles full cleanup.

	// Load South texture.
	if (!load_single_texture(game->mlx_ptr, game->config.south_texture_path, \
								&game->south_texture))
		return (0);

	// Load East texture.
	if (!load_single_texture(game->mlx_ptr, game->config.east_texture_path, \
								&game->east_texture))
		return (0);

	// Load West texture.
	if (!load_single_texture(game->mlx_ptr, game->config.west_texture_path, \
								&game->west_texture))
		return (0);

	printf("All wall textures loaded successfully.\n");
	return (1); // All textures loaded successfully.
}
