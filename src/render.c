/* ************************************************************************** */
/*                                                                            */
/*   render.c                                                                 */
/*                                                                            */
/*   Description:                                                             */
/*     This module is responsible for all rendering operations in cub3D.      */
/*     It includes functions for drawing pixels to the screen buffer,         */
/*     converting colors, drawing the floor and ceiling, and orchestrating    */
/*     the overall frame rendering process which involves calling the         */
/*     raycaster and then displaying the final image.                         */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/**
 * @brief Writes a pixel of a specified color to an image buffer at (x, y).
 *
 * Calculates the memory address of the target pixel within the image's raw data
 * array (`img->addr`). The calculation uses the image's line length (bytes per row)
 * and bits per pixel to correctly offset into the buffer.
 * It performs bounds checking to ensure `x` and `y` are within screen limits.
 *
 * @param img Pointer to the t_img structure representing the image (e.g., screen_buffer).
 * @param x The x-coordinate of the pixel to draw.
 * @param y The y-coordinate of the pixel to draw.
 * @param color The integer representation of the color to be written.
 */
void	my_mlx_pixel_put(t_img *img, int x, int y, int color)
{
	char	*dst; // Pointer to the destination pixel in the image data.

	// Perform bounds checking to prevent writing outside the image buffer.
	if (x < 0 || x >= game->screen_width || y < 0 || y >= game->screen_height) // Assuming SCREEN_WIDTH/HEIGHT are from game struct or global
		return;

	// Calculate the address of the pixel.
	// y * img->line_length: offset to the beginning of the correct row.
	// x * (img->bits_per_pixel / 8): offset within that row to the correct pixel.
	// (img->bits_per_pixel / 8) gives the number of bytes per pixel.
	dst = img->addr + (y * img->line_length + x * (img->bits_per_pixel / 8));

	// Write the color value to the calculated memory address.
	// Cast to unsigned int* as colors are typically handled as 32-bit integers.
	*(unsigned int *)dst = color;
}

/**
 * @brief Converts RGB color components into a single integer.
 *
 * This function combines the red, green, and blue color components into a
 * single integer format suitable for many graphics libraries, often 0xRRGGBB.
 * The alpha component is implicitly 0 (opaque) in this scheme.
 *
 * @param r Red component (0-255).
 * @param g Green component (0-255).
 * @param b Blue component (0-255).
 * @return Integer representation of the color (e.g., (r << 16) | (g << 8) | b).
 */
int	rgb_to_int(int r, int g, int b)
{
	// Shift red component 16 bits to the left, green 8 bits, blue stays.
	// Bitwise OR combines them. Ensure components are masked to 8 bits (0xFF)
	// to prevent overflow if larger values are passed, though parser should ensure 0-255.
	return (((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF));
}

/**
 * @brief Draws the floor and ceiling with solid colors on the screen buffer.
 *
 * The screen is divided horizontally at its midpoint.
 * The top half (from y=0 to y=screen_height/2 - 1) is filled with the ceiling color.
 * The bottom half (from y=screen_height/2 to y=screen_height - 1) is filled with the floor color.
 * Colors are retrieved from the game's configuration.
 *
 * @param game Pointer to the t_game_data struct, containing screen dimensions,
 *             screen buffer, and parsed color configurations.
 */
void	draw_floor_and_ceiling(t_game_data *game)
{
	int	x, y;
	int	ceiling_color_int;
	int	floor_color_int;

	// Convert parsed R,G,B components to integer color representations.
	ceiling_color_int = rgb_to_int(game->config.ceiling_color_r, \
									game->config.ceiling_color_g, \
									game->config.ceiling_color_b);
	floor_color_int = rgb_to_int(game->config.floor_color_r, \
									game->config.floor_color_g, \
									game->config.floor_color_b);
	// Draw the ceiling (top half of the screen).
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
	// Draw the floor (bottom half of the screen).
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

/**
 * @brief Main rendering function called repeatedly by the MLX loop hook.
 *
 * This function orchestrates the drawing of a single frame:
 * 1. Clears the previous frame by drawing the floor and ceiling.
 * 2. Calls the raycasting logic (`cast_all_rays`) to draw the walls based on
 *    the player's current position and orientation.
 * 3. (Future work placeholders: drawing sprites, minimap).
 * 4. Puts the fully rendered frame from the screen buffer image to the window
 *    for display.
 *
 * @param game Pointer to the t_game_data struct.
 * @return int Always returns 0, as required by `mlx_loop_hook`.
 */
int	render_next_frame(t_game_data *game)
{
	// Step 1: Draw the static background (floor and ceiling).
	// This effectively clears the buffer from the previous frame's dynamic content (walls).
	draw_floor_and_ceiling(game);

	// Step 2: Perform raycasting and draw the walls onto the screen_buffer.
	// cast_all_rays will iterate through screen columns, calculate wall intersections,
	// and use my_mlx_pixel_put (or similar) to draw textured wall slices.
	cast_all_rays(game);

	// Step 3: (Placeholder for future features)
	// draw_sprites(game);   // If sprites are implemented.
	// draw_minimap(game); // If a minimap is implemented.

	// Step 4: Display the completed frame.
	// Copy the contents of the off-screen screen_buffer (where all drawing occurred)
	// to the visible window.
	mlx_put_image_to_window(game->mlx_ptr, game->win_ptr, \
							game->screen_buffer.img_ptr, 0, 0);

	return (0); // Return 0 as per mlx_loop_hook requirements.
}
