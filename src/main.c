/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: your_login <your_login@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 10:10:00 by your_login          #+#    #+#             */
/*   Updated: 2023/10/27 10:10:00 by your_login         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: your_login <your_login@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 10:10:00 by your_login          #+#    #+#             */
/*   Updated: 2023/10/27 12:05:00 by your_login         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <stdio.h>

// This function is called if init_mlx fails, to ensure parsed config is freed.
void	handle_init_error(t_game_data *game)
{
	fprintf(stderr, "Error during initialization. Cleaning up.\n");
	// game->mlx_ptr and game->win_ptr might be NULL or partially initialized.
	// cleanup_mlx should handle this, but config is definitely parsed.
	free_config(&game->config);
	// If mlx_ptr was initialized but window failed, some mlx versions might need
	// specific cleanup for mlx_ptr itself, e.g. mlx_destroy_display(game->mlx_ptr) on Linux.
	// For now, this is a simple exit.
	exit(1);
}

int	main(int argc, char **argv)
{
	t_game_data	game; // Use t_game_data to hold everything

	// Initialize pointers to NULL for safety, especially for cleanup functions
	game.mlx_ptr = NULL;
	game.win_ptr = NULL;
	// game.config is a struct, its members are initialized by init_config via parse_cub_file

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <map_file.cub>\n", argv[0]);
		return (1);
	}

	// Parse the .cub file and store data in game.config
	if (!parse_cub_file(argv[1], &game.config))
	{
		// Error messages are printed by the parser.
		// free_config(&game.config) is called by parse_cub_file on error.
		return (1);
	}

	printf("Parsing successful!\n");
	// Optional: Print parsed details (can be removed for final version)
	printf("North Texture: %s\n", game.config.north_texture_path ? game.config.north_texture_path : "(null)");
	printf("South Texture: %s\n", game.config.south_texture_path ? game.config.south_texture_path : "(null)");
	printf("West Texture: %s\n", game.config.west_texture_path ? game.config.west_texture_path : "(null)");
	printf("East Texture: %s\n", game.config.east_texture_path ? game.config.east_texture_path : "(null)");
	printf("Floor Color: R=%d, G=%d, B=%d\n", game.config.floor_color_r, game.config.floor_color_g, game.config.floor_color_b);
	printf("Ceiling Color: R=%d, G=%d, B=%d\n", game.config.ceiling_color_r, game.config.ceiling_color_g, game.config.ceiling_color_b);
	if (game.config.map_data)
	{
		printf("Player Start in config: X=%.2f, Y=%.2f, Orientation=%c\n", \
			game.config.player_start_x, game.config.player_start_y, game.config.player_orientation);
	}

	// Initialize Player State
	init_player_state(&game);
	printf("Player Initial State:\n");
	printf("  Position: (X=%.2f, Y=%.2f)\n", game.player_x, game.player_y);
	printf("  Direction: (dir_X=%.2f, dir_Y=%.2f)\n", game.dir_x, game.dir_y);
	printf("  Camera Plane: (plane_X=%.2f, plane_Y=%.2f)\n", game.plane_x, game.plane_y);


	// Initialize MiniLibX
	if (!init_mlx(&game))
	{
		// init_mlx should print specific errors.
		// cleanup_mlx might have been partially called or needs to be robust.
		// Ensure config data is freed if parsing was successful but MLX failed.
		handle_init_error(&game); // Frees config and exits
	}
	printf("MiniLibX initialized. Window created. Setting up hooks.\n");

	// Load textures
	if (!load_all_textures(&game))
	{
		// load_all_textures should print specific errors and free partially loaded textures.
		// It does not handle game.config or mlx resources itself, so we must.
		fprintf(stderr, "Error: Failed to load one or more textures.\n");
		cleanup_mlx(&game); // Clean up MLX window and screen buffer
		free_config(&game.config); // Clean up parsed config data
		exit(1);
	}
	printf("Textures loaded successfully.\n");
	// Hooks are set up within init_mlx via setup_hooks()

	printf("Starting MLX event loop.\n");
	mlx_loop(game.mlx_ptr);

	// Code here is reached only if mlx_loop exits, which typically means
	// cleanup has already been handled by keypress/close events.
	// However, as a fallback or if mlx_loop could exit differently:
	printf("MLX loop finished. Performing final cleanup.\n");
	free_all_textures(&game); // Free textures first
	cleanup_mlx(&game);       // Then MLX window/image resources
	free_config(&game.config); // Then config data

	return (0);
}
