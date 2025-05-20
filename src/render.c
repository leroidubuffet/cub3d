/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: your_login <your_login@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 14:00:00 by your_login          #+#    #+#             */
/*   Updated: 2023/10/27 14:00:00 by your_login         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/*
 * Writes a pixel color to the image buffer at position (x, y).
 * The address calculation takes into account the line length and
 * bits per pixel of the image.
 */
void	my_mlx_pixel_put(t_img *img, int x, int y, int color)
{
	char	*dst;

	if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT)
		return ; // Stay within buffer bounds
	dst = img->addr + (y * img->line_length + x * (img->bits_per_pixel / 8));
	*(unsigned int *)dst = color;
}

/*
 * Converts RGB color components into a single integer representation.
 * Assumes standard 0xAARRGGBB format, but typically alpha is ignored or set to 0.
 * For MiniLibX, the order is often 0x00RRGGBB.
 */
int	rgb_to_int(int r, int g, int b)
{
	return ((r & 0xFF) << 16 | (g & 0xFF) << 8 | (b & 0xFF));
}

/*
 * Draws the floor and ceiling with solid colors.
 * The ceiling is drawn on the top half of the screen, and the floor on the bottom.
 */
void	draw_floor_and_ceiling(t_game_data *game)
{
	int	x;
	int	y;
	int	ceiling_color_int;
	int	floor_color_int;

	ceiling_color_int = rgb_to_int(game->config.ceiling_color_r, \
									game->config.ceiling_color_g, \
									game->config.ceiling_color_b);
	floor_color_int = rgb_to_int(game->config.floor_color_r, \
									game->config.floor_color_g, \
									game->config.floor_color_b);
	y = 0;
	while (y < game->screen_height / 2)
	{
		x = 0;
		while (x < game->screen_width)
		{
			my_mlx_pixel_put(&game->screen_buffer, x, y, ceiling_color_int);
			x++;
		}
		y++;
	}
	while (y < game->screen_height)
	{
		x = 0;
		while (x < game->screen_width)
		{
			my_mlx_pixel_put(&game->screen_buffer, x, y, floor_color_int);
			x++;
		}
		y++;
	}
}

/*
 * Main rendering function called by the mlx_loop_hook.
 * It orchestrates the drawing of different elements.
 */
int	render_next_frame(t_game_data *game)
{
	// Clear previous frame (optional if drawing over everything)
	// For example, fill with black:
	// for (int y = 0; y < game->screen_height; ++y)
	//     for (int x = 0; x < game->screen_width; ++x)
	//         my_mlx_pixel_put(&game->screen_buffer, x, y, 0x000000);

	draw_floor_and_ceiling(game);
	cast_all_rays(game); // This will draw the walls
	// Later: draw_sprites(game);
	// Later: draw_minimap(game);

	mlx_put_image_to_window(game->mlx_ptr, game->win_ptr, \
							game->screen_buffer.img_ptr, 0, 0);
	return (0);
}
