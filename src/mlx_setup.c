/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_setup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: your_login <your_login@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 12:00:00 by your_login          #+#    #+#             */
/*   Updated: 2023/10/27 12:00:00 by your_login         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	handle_window_close(t_game_data *game)
{
	printf("Window close event triggered (Red Cross).\n");
	free_all_textures(game);
	cleanup_mlx(game);
	free_config(&game->config); // Free parsed config data
	exit(0);
	return (0); // Should not be reached
}

int	handle_keypress(int keycode, t_game_data *game)
{
	// No need to print keycode anymore, unless for debugging
	// printf("Key pressed: %d\n", keycode);
	if (keycode == KEY_ESC)
	{
		printf("ESC key pressed. Exiting.\n");
		free_all_textures(game);
		cleanup_mlx(game);
		free_config(&game->config); // Free parsed config data
		exit(0); // Exit successfully
	}
	if (keycode == KEY_W)
		move_forward_backward(game, 1.0);
	if (keycode == KEY_S)
		move_forward_backward(game, -1.0);
	if (keycode == KEY_A)
		strafe_left_right(game, -1.0);
	if (keycode == KEY_D)
		strafe_left_right(game, 1.0);
	if (keycode == KEY_LEFT_ARROW)
		rotate_player(game, -1.0);
	if (keycode == KEY_RIGHT_ARROW)
		rotate_player(game, 1.0);
	// The game will re-render in the next mlx_loop_hook iteration
	return (0);
}

void	setup_hooks(t_game_data *game)
{
	// KeyPress event
	mlx_hook(game->win_ptr, 2, 1L << 0, handle_keypress, game);
	// Window close event (DestroyNotify)
	mlx_hook(game->win_ptr, 17, 1L << 17, handle_window_close, game);
	// You can add more hooks here, e.g., KeyRelease (type 3, mask 1L<<1)
	// Register the main render loop
	mlx_loop_hook(game->mlx_ptr, render_next_frame, game);
}

static int	init_screen_buffer(t_game_data *game)
{
	game->screen_buffer.img_ptr = mlx_new_image(game->mlx_ptr, \
												game->screen_width, \
												game->screen_height);
	if (!game->screen_buffer.img_ptr)
	{
		fprintf(stderr, "Error: mlx_new_image() failed for screen buffer.\n");
		return (0);
	}
	game->screen_buffer.addr = mlx_get_data_addr(game->screen_buffer.img_ptr, \
										&game->screen_buffer.bits_per_pixel, \
										&game->screen_buffer.line_length, \
										&game->screen_buffer.endian);
	if (!game->screen_buffer.addr)
	{
		fprintf(stderr, "Error: mlx_get_data_addr() failed for screen buffer.\n");
		mlx_destroy_image(game->mlx_ptr, game->screen_buffer.img_ptr); // Clean up created image
		game->screen_buffer.img_ptr = NULL;
		return (0);
	}
	return (1);
}

int	init_mlx(t_game_data *game)
{
	game->mlx_ptr = mlx_init();
	if (!game->mlx_ptr)
		return (fprintf(stderr, "Error: mlx_init() failed.\n"), 0);
	game->screen_width = SCREEN_WIDTH;
	game->screen_height = SCREEN_HEIGHT;
	game->win_ptr = mlx_new_window(game->mlx_ptr, game->screen_width, \
									game->screen_height, "cub3D");
	if (!game->win_ptr)
	{
		fprintf(stderr, "Error: mlx_new_window() failed.\n");
		// On Linux, mlx_destroy_display(game->mlx_ptr) might be needed here if mlx_init succeeded.
		// free(game->mlx_ptr); // This is generally not correct for mlx_ptr.
		return (0);
	}
	if (!init_screen_buffer(game))
	{
		mlx_destroy_window(game->mlx_ptr, game->win_ptr);
		game->win_ptr = NULL;
		// Potentially mlx_destroy_display here too on Linux.
		// Also, free textures if they were loaded before this point in init_mlx
		free_all_textures(game); // Free any textures if image buffer failed
		return (0);
	}
	setup_hooks(game);
	return (1);
}

void	cleanup_mlx(t_game_data *game)
{
	// Textures should be freed by calling free_all_textures() before this,
	// or by the exit hooks. This function focuses on MLX components.
	if (game->screen_buffer.img_ptr && game->mlx_ptr)
	{
		mlx_destroy_image(game->mlx_ptr, game->screen_buffer.img_ptr);
		game->screen_buffer.img_ptr = NULL;
		game->screen_buffer.addr = NULL;
	}
	if (game->win_ptr && game->mlx_ptr)
	{
		mlx_destroy_window(game->mlx_ptr, game->win_ptr);
		game->win_ptr = NULL;
	}
	// Note: free_all_textures should be called before this if not done by caller.
	// The mlx_ptr itself (display connection) might need freeing on Linux,
	// e.g., mlx_destroy_display(game->mlx_ptr); then free(game->mlx_ptr);
	// This is highly dependent on the MiniLibX version.
	printf("MLX window and screen buffer cleaned up.\n");
}
