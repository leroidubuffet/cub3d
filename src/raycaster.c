/* ************************************************************************** */
/*                                                                            */
/*   raycaster.c                                                              */
/*                                                                            */
/*   Description:                                                             */
/*     This module implements the raycasting algorithm for rendering the 3D   */
/*     world view in cub3D. It casts rays from the player's viewpoint for     */
/*     each vertical column of the screen, determines wall intersections,     */
/*     calculates wall distances and heights, and then draws textured         */
/*     vertical stripes to create the 3D perspective.                         */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <math.h>   // For fabs, floor

/**
 * @brief Holds all parameters for a single ray during its casting process.
 * This structure simplifies passing ray-specific data between helper functions.
 */
typedef struct s_ray_params
{
	double	camera_x;       // X-coordinate on the camera plane (-1 to 1).
	double	ray_dir_x;      // X-component of the ray's direction vector.
	double	ray_dir_y;      // Y-component of the ray's direction vector.
	int		map_x;          // Current map grid cell X the ray is in.
	int		map_y;          // Current map grid cell Y the ray is in.
	double	side_dist_x;    // Distance the ray has to travel from its start
	                        // position to the first x-side (vertical grid line).
	double	side_dist_y;    // Distance the ray has to travel from its start
	                        // position to the first y-side (horizontal grid line).
	double	delta_dist_x;   // Distance the ray has to travel from one x-side
	                        // to the next x-side.
	double	delta_dist_y;   // Distance the ray has to travel from one y-side
	                        // to the next y-side.
	double	perp_wall_dist; // Perpendicular distance from the player to the wall hit by the ray.
	int		step_x;         // Direction to step in x (either -1 or +1).
	int		step_y;         // Direction to step in y (either -1 or +1).
	int		hit;            // Flag: 1 if a wall was hit, 0 otherwise.
	int		side;           // Determines wall side hit: 0 for X-side (East/West), 1 for Y-side (North/South).
}	t_ray_params;

/**
 * @brief Initializes ray parameters for a given screen column (x).
 * Calculates the ray's direction based on the player's direction, camera plane,
 * and the current screen column. Also sets initial map coordinates and delta distances.
 * @param game Pointer to t_game_data containing player and screen info.
 * @param ray Pointer to t_ray_params to be initialized.
 * @param x The current screen column for which the ray is being cast.
 */
static void	init_ray_directions(t_game_data *game, t_ray_params *ray, int x)
{
	// Calculate camera_x: x-coordinate in camera space (-1 for left, 0 center, 1 for right edge).
	ray->camera_x = 2 * x / (double)game->screen_width - 1;

	// Calculate ray direction vector.
	// ray_dir = player_dir + camera_plane * camera_x
	ray->ray_dir_x = game->dir_x + game->plane_x * ray->camera_x;
	ray->ray_dir_y = game->dir_y + game->plane_y * ray->camera_x;

	// Current map grid cell the player is in.
	ray->map_x = (int)game->player_x;
	ray->map_y = (int)game->player_y;

	// Calculate delta_dist: distance ray travels to cross one x or y grid cell.
	// Avoid division by zero if ray_dir_x or ray_dir_y is 0 by setting a large value.
	ray->delta_dist_x = (ray->ray_dir_x == 0) ? 1e30 : fabs(1 / ray->ray_dir_x);
	ray->delta_dist_y = (ray->ray_dir_y == 0) ? 1e30 : fabs(1 / ray->ray_dir_y);
}

/**
 * @brief Calculates initial step directions and side distances for the DDA algorithm.
 * Determines step_x/step_y (-1 or +1) based on ray direction.
 * Calculates side_dist_x/side_dist_y: distance from player's start to the first
 * x-side or y-side encountered by the ray.
 * @param game Pointer to t_game_data containing player position.
 * @param ray Pointer to t_ray_params to be updated.
 */
static void	calculate_step_and_side_dist(t_game_data *game, t_ray_params *ray)
{
	// Calculate step_x and initial side_dist_x.
	if (ray->ray_dir_x < 0) // Ray moving left
	{
		ray->step_x = -1;
		ray->side_dist_x = (game->player_x - ray->map_x) * ray->delta_dist_x;
	}
	else // Ray moving right (or straight vertically if ray_dir_x is 0)
	{
		ray->step_x = 1;
		ray->side_dist_x = (ray->map_x + 1.0 - game->player_x) * ray->delta_dist_x;
	}
	// Calculate step_y and initial side_dist_y.
	if (ray->ray_dir_y < 0) // Ray moving up (North)
	{
		ray->step_y = -1;
		ray->side_dist_y = (game->player_y - ray->map_y) * ray->delta_dist_y;
	}
	else // Ray moving down (South) (or straight horizontally if ray_dir_y is 0)
	{
		ray->step_y = 1;
		ray->side_dist_y = (ray->map_y + 1.0 - game->player_y) * ray->delta_dist_y;
	}
}

/**
 * @brief Performs the Digital Differential Analysis (DDA) algorithm.
 * Steps through the grid, extending the ray until a wall ('1') is hit.
 * Updates ray->map_x, ray->map_y, ray->side, and ray->hit.
 * Includes a boundary check to prevent infinite loops or out-of-bounds access.
 * @param game Pointer to t_game_data containing map data.
 * @param ray Pointer to t_ray_params to be updated by DDA.
 */
static void	perform_dda(t_game_data *game, t_ray_params *ray)
{
	ray->hit = 0; // Initialize hit flag to 0 (no wall hit yet).
	while (ray->hit == 0)
	{
		// Jump to next map grid cell, either in x-direction or y-direction.
		if (ray->side_dist_x < ray->side_dist_y)
		{
			ray->side_dist_x += ray->delta_dist_x; // Increment distance to next x-side.
			ray->map_x += ray->step_x;             // Move to next cell in x-direction.
			ray->side = 0;                         // Wall hit was on an X-side (East/West face).
		}
		else
		{
			ray->side_dist_y += ray->delta_dist_y; // Increment distance to next y-side.
			ray->map_y += ray->step_y;             // Move to next cell in y-direction.
			ray->side = 1;                         // Wall hit was on a Y-side (North/South face).
		}
		// Check if ray has gone out of map boundaries.
		if (ray->map_x < 0 || ray->map_x >= game->config.map_width || \
			ray->map_y < 0 || ray->map_y >= game->config.map_height)
		{
			ray->hit = 1; // Consider this a "hit" on a virtual boundary wall.
			ray->perp_wall_dist = 1e30; // Set to a very large distance.
			return ; // Exit DDA.
		}
		// Check if the ray has hit a wall ('1') in the map.
		if (game->config.map_data[ray->map_y][ray->map_x] == '1')
			ray->hit = 1; // Wall hit.
	}
}

/**
 * @brief Calculates the perpendicular distance to the wall.
 * This corrects for fisheye distortion that would occur if Euclidean distance was used.
 * @param ray Pointer to t_ray_params containing DDA results. perp_wall_dist is updated.
 */
static void	calculate_wall_distance(t_ray_params *ray)
{
	if (ray->side == 0) // Wall was X-side (East/West)
		ray->perp_wall_dist = (ray->side_dist_x - ray->delta_dist_x);
	else // Wall was Y-side (North/South)
		ray->perp_wall_dist = (ray->side_dist_y - ray->delta_dist_y);

	// Clamp distance to a small positive value to prevent division by zero
	// or other issues if the wall is extremely close.
	if (ray->perp_wall_dist < 0.01)
		ray->perp_wall_dist = 0.01;
}

/**
 * @brief Selects the appropriate wall texture based on which side of the wall was hit
 * and the direction of the ray.
 * @param game Pointer to t_game_data containing loaded textures.
 * @param ray Pointer to t_ray_params indicating wall side and ray direction.
 * @return Pointer to the t_img texture structure to be used for the wall stripe.
 */
static t_img	*select_texture(t_game_data *game, t_ray_params *ray)
{
	if (ray->side == 0) // Hit an East/West wall face.
	{
		if (ray->ray_dir_x > 0) // Ray is moving towards positive X (East). Hit a West face.
			return (&game->west_texture);
		else // Ray is moving towards negative X (West). Hit an East face.
			return (&game->east_texture);
	}
	else // Hit a North/South wall face.
	{
		if (ray->ray_dir_y > 0) // Ray is moving towards positive Y (South). Hit a North face.
			return (&game->north_texture);
		else // Ray is moving towards negative Y (North). Hit a South face.
			return (&game->south_texture);
	}
}

/**
 * @brief Calculates wall_x, the exact x-coordinate on the wall where the ray hit.
 * This value ranges from 0.0 to 1.0 and is used for texture mapping.
 * @param game Pointer to t_game_data containing player position.
 * @param ray Pointer to t_ray_params.
 * @return The fractional x-coordinate of the wall hit.
 */
static double	calculate_wall_x(t_game_data *game, t_ray_params *ray)
{
	double	wall_x; // The exact hit point on the wall, normalized to [0,1]

	if (ray->side == 0) // Hit an EW wall face
		wall_x = game->player_y + ray->perp_wall_dist * ray->ray_dir_y;
	else // Hit an NS wall face
		wall_x = game->player_x + ray->perp_wall_dist * ray->ray_dir_x;

	// Get the fractional part of wall_x for texture mapping.
	wall_x -= floor(wall_x);
	return (wall_x);
}

/**
 * @brief Calculates the x-coordinate of the texture column to use.
 * It takes into account potential flipping of the texture based on wall orientation
 * and ray direction, ensuring textures are not mirrored incorrectly.
 * @param ray Pointer to t_ray_params.
 * @param texture Pointer to the t_img texture for the wall.
 * @param wall_x The fractional x-coordinate of the wall hit (0.0 to 1.0).
 * @return The integer x-coordinate of the texture column.
 */
static int	calculate_texture_x(t_ray_params *ray, t_img *texture, double wall_x)
{
	int	tex_x;

	// Convert normalized wall_x to texture column index.
	tex_x = (int)(wall_x * (double)texture->width);

	// Texture flipping logic:
	// If wall is EW (side 0) and ray points East (ray_dir_x > 0), it hit a West face.
	// Textures for West faces might need to be flipped if their natural orientation is for East faces.
	// Lodev's tutorial flips if (side == 0 && rayDirX > 0) or (side == 1 && rayDirY < 0).
	// This means: flip for West-facing walls and South-facing walls.
	if (ray->side == 0 && ray->ray_dir_x > 0) // Hit West face
		tex_x = texture->width - tex_x - 1;
	if (ray->side == 1 && ray->ray_dir_y < 0) // Hit South face
		tex_x = texture->width - tex_x - 1;

	return (tex_x);
}

/**
 * @brief Draws a textured vertical stripe on the screen buffer.
 * Calculates wall height, texture coordinates, and iterates through pixels
 * of the stripe to draw the textured wall segment.
 * @param g Pointer to t_game_data.
 * @param r Pointer to t_ray_params for the current ray.
 * @param x The screen column (x-coordinate) where the stripe is drawn.
 * @param s_h Screen height, passed for convenience.
 */
static void	draw_textured_stripe(t_game_data *g, t_ray_params *r, int x, int s_h)
{
	t_img	*tex;        // Pointer to the selected wall texture.
	double	wall_x;      // Exact hit point on wall (0.0-1.0).
	int		tex_x;       // X-coordinate on the texture.
	int		line_h;      // Height of the wall stripe to draw on screen.
	int		draw_s;      // Start Y-coordinate for drawing the stripe on screen.
	int		draw_e;      // End Y-coordinate for drawing the stripe on screen.
	double	step;        // How much to increment texture coordinate per screen pixel.
	double	tex_pos;     // Current Y-texture coordinate (can be fractional).
	int		tex_y;       // Integer Y-texture coordinate.
	int		color;       // Color of the texture pixel.
	int		y_coord;     // Current Y-coordinate on screen being drawn.

	tex = select_texture(g, r); // Determine which texture to use (N,S,E,W).
	if (!tex || !tex->img_ptr) return; // Safety: skip if texture is missing/unloaded.

	wall_x = calculate_wall_x(g, r);      // Calculate exact horizontal hit point.
	tex_x = calculate_texture_x(r, tex, wall_x); // Calculate texture X-coordinate.

	// Calculate height of line to draw on screen.
	line_h = (int)(s_h / r->perp_wall_dist);

	// Calculate lowest and highest pixel to fill in current stripe.
	draw_s = -line_h / 2 + s_h / 2;
	if (draw_s < 0) draw_s = 0; // Clamp to top of screen.
	draw_e = line_h / 2 + s_h / 2;
	if (draw_e >= s_h) draw_e = s_h - 1; // Clamp to bottom of screen.

	// Calculate texture stepping and initial texture position.
	step = 1.0 * tex->height / line_h; // How much Y-texture coord changes per screen Y-pixel.
	// Starting texture coordinate, adjusted for when stripe is clipped by screen edge.
	tex_pos = (draw_s - s_h / 2 + line_h / 2) * step;

	// Draw the pixels of the stripe.
	y_coord = draw_s;
	while (y_coord <= draw_e)
	{
		// Get the Y-texture coordinate (cast to int) and ensure it's within texture height.
		// Using `& (tex->height - 1)` is a fast modulo if texture height is a power of 2.
		// Otherwise, `tex_y = (int)tex_pos % tex->height;` should be used.
		tex_y = (int)tex_pos & (tex->height - 1);
		if (tex_y < 0) tex_y = 0; // Clamp tex_y just in case.
		tex_pos += step; // Increment texture position for next screen pixel.

		// Get the color of the texture pixel.
		color = *(unsigned int *)(tex->addr + (tex_y * tex->line_length + \
				tex_x * (tex->bits_per_pixel / 8)));

		// Draw the pixel to the screen buffer.
		my_mlx_pixel_put(&g->screen_buffer, x, y_coord, color);
		y_coord++;
	}
}

/**
 * @brief Casts rays for every vertical screen column to render the scene.
 * For each column `x` from 0 to screen_width-1:
 * 1. Initializes ray parameters (direction, initial position, deltas).
 * 2. Calculates step directions and initial side distances.
 * 3. Performs DDA to find the wall hit.
 * 4. If a wall is hit (and not a map boundary), calculates wall distance.
 * 5. Draws the textured vertical stripe corresponding to the wall hit.
 * @param game Pointer to the t_game_data struct.
 */
void	cast_all_rays(t_game_data *game)
{
	int				x;
	t_ray_params	ray; // Re-use the same ray struct for each column.

	x = 0;
	while (x < game->screen_width) // Loop through every vertical stripe of the screen.
	{
		// Step 1: Initialize ray for this column.
		init_ray_directions(game, &ray, x);
		// Step 2: Calculate initial step and side distances.
		calculate_step_and_side_dist(game, &ray);
		// Step 3: Perform DDA algorithm to find wall hit.
		perform_dda(game, &ray);

		// Step 4: If DDA hit a wall (not an arbitrary boundary set to 1e30 distance).
		if (ray.hit && ray.perp_wall_dist < 1e29)
		{
			// Step 4a: Calculate perpendicular wall distance.
			calculate_wall_distance(&ray);
			// Step 4b: Draw the textured vertical stripe for this column.
			draw_textured_stripe(game, &ray, x, game->screen_height);
		}
		// Else (if ray hit virtual boundary or no hit), the column remains
		// as floor/ceiling color, or could be black/default if not pre-filled.
		x++;
	}
}
