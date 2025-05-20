/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture_loader.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: your_login <your_login@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 14:00:00 by your_login          #+#    #+#             */
/*   Updated: 2023/10/28 14:00:00 by your_login         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h" // For t_game_data, t_img, etc.

// Helper function to load a single texture
static int	load_single_texture(void *mlx_ptr, char *path, t_img *texture_img)
{
	if (!path) // Path might be NULL if not set in config (parser should prevent this)
	{
		fprintf(stderr, "Error: Texture path is NULL.\n");
		return (0);
	}
	texture_img->img_ptr = mlx_xpm_file_to_image(mlx_ptr, path, \
											&texture_img->width, \
											&texture_img->height);
	if (!texture_img->img_ptr)
	{
		fprintf(stderr, "Error: Failed to load texture: %s\n", path);
		return (0);
	}
	texture_img->addr = mlx_get_data_addr(texture_img->img_ptr, \
										&texture_img->bits_per_pixel, \
										&texture_img->line_length, \
										&texture_img->endian);
	if (!texture_img->addr)
	{
		fprintf(stderr, "Error: mlx_get_data_addr failed for texture: %s\n", path);
		mlx_destroy_image(mlx_ptr, texture_img->img_ptr);
		texture_img->img_ptr = NULL;
		return (0);
	}
	return (1);
}

void	free_all_textures(t_game_data *game)
{
	if (game->north_texture.img_ptr)
		mlx_destroy_image(game->mlx_ptr, game->north_texture.img_ptr);
	if (game->south_texture.img_ptr)
		mlx_destroy_image(game->mlx_ptr, game->south_texture.img_ptr);
	if (game->east_texture.img_ptr)
		mlx_destroy_image(game->mlx_ptr, game->east_texture.img_ptr);
	if (game->west_texture.img_ptr)
		mlx_destroy_image(game->mlx_ptr, game->west_texture.img_ptr);
	// Important to NULLify pointers after freeing to prevent double free attempts
	game->north_texture.img_ptr = NULL;
	game->south_texture.img_ptr = NULL;
	game->east_texture.img_ptr = NULL;
	game->west_texture.img_ptr = NULL;
	printf("All textures freed.\n");
}

// Loads all wall textures. If any fails, frees previously loaded ones.
int	load_all_textures(t_game_data *game)
{
	// Initialize img_ptr to NULL for robust error handling in free_all_textures
	game->north_texture.img_ptr = NULL;
	game->south_texture.img_ptr = NULL;
	game->east_texture.img_ptr = NULL;
	game->west_texture.img_ptr = NULL;

	if (!load_single_texture(game->mlx_ptr, game->config.north_texture_path, \
								&game->north_texture))
		return (0);
	if (!load_single_texture(game->mlx_ptr, game->config.south_texture_path, \
								&game->south_texture))
		return (0);
	if (!load_single_texture(game->mlx_ptr, game->config.east_texture_path, \
								&game->east_texture))
		return (0);
	if (!load_single_texture(game->mlx_ptr, game->config.west_texture_path, \
								&game->west_texture))
		return (0);
	printf("All textures loaded successfully.\n");
	return (1);
}
