/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycaster.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: your_login <your_login@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 18:00:00 by your_login          #+#    #+#             */
/*   Updated: 2023/10/27 18:00:00 by your_login         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h" // For t_game_data, math functions, etc.
#include <math.h>   // For fabs

// Structure to hold ray parameters for a single ray, to help with Norm
typedef struct s_ray_params
{
	double	camera_x;
	double	ray_dir_x;
	double	ray_dir_y;
	int		map_x;
	int		map_y;
	double	side_dist_x;
	double	side_dist_y;
	double	delta_dist_x;
	double	delta_dist_y;
	double	perp_wall_dist;
	int		step_x;
	int		step_y;
	int		hit;
	int		side; // 0 for EW wall, 1 for NS wall
}	t_ray_params;

static void	init_ray_directions(t_game_data *game, t_ray_params *ray, int x)
{
	ray->camera_x = 2 * x / (double)game->screen_width - 1;
	ray->ray_dir_x = game->dir_x + game->plane_x * ray->camera_x;
	ray->ray_dir_y = game->dir_y + game->plane_y * ray->camera_x;
	ray->map_x = (int)game->player_x;
	ray->map_y = (int)game->player_y;
	ray->delta_dist_x = (ray->ray_dir_x == 0) ? 1e30 : fabs(1 / ray->ray_dir_x);
	ray->delta_dist_y = (ray->ray_dir_y == 0) ? 1e30 : fabs(1 / ray->ray_dir_y);
}

static void	calculate_step_and_side_dist(t_game_data *game, t_ray_params *ray)
{
	if (ray->ray_dir_x < 0)
	{
		ray->step_x = -1;
		ray->side_dist_x = (game->player_x - ray->map_x) * ray->delta_dist_x;
	}
	else
	{
		ray->step_x = 1;
		ray->side_dist_x = (ray->map_x + 1.0 - game->player_x) * ray->delta_dist_x;
	}
	if (ray->ray_dir_y < 0)
	{
		ray->step_y = -1;
		ray->side_dist_y = (game->player_y - ray->map_y) * ray->delta_dist_y;
	}
	else
	{
		ray->step_y = 1;
		ray->side_dist_y = (ray->map_y + 1.0 - game->player_y) * ray->delta_dist_y;
	}
}

static void	perform_dda(t_game_data *game, t_ray_params *ray)
{
	ray->hit = 0;
	while (ray->hit == 0)
	{
		if (ray->side_dist_x < ray->side_dist_y)
		{
			ray->side_dist_x += ray->delta_dist_x;
			ray->map_x += ray->step_x;
			ray->side = 0; // EW wall
		}
		else
		{
			ray->side_dist_y += ray->delta_dist_y;
			ray->map_y += ray->step_y;
			ray->side = 1; // NS wall
		}
		// Check map boundaries (simple check, assumes map is somewhat valid)
		if (ray->map_x < 0 || ray->map_x >= game->config.map_width || \
			ray->map_y < 0 || ray->map_y >= game->config.map_height)
		{
			ray->hit = 1; // Hit "virtual" boundary wall
			ray->perp_wall_dist = 1e30; // Very large distance
			return ;
		}
		if (game->config.map_data[ray->map_y][ray->map_x] == '1')
			ray->hit = 1;
	}
}

static void	calculate_wall_distance(t_ray_params *ray)
{
	if (ray->side == 0) // EW wall
		ray->perp_wall_dist = (ray->side_dist_x - ray->delta_dist_x);
	else // NS wall
		ray->perp_wall_dist = (ray->side_dist_y - ray->delta_dist_y);
	if (ray->perp_wall_dist < 0.01) // Prevent division by zero or too close walls
		ray->perp_wall_dist = 0.01;
}

// Helper to select the correct texture based on wall side and ray direction
static t_img	*select_texture(t_game_data *game, t_ray_params *ray)
{
	if (ray->side == 0) // EW wall
	{
		if (ray->ray_dir_x > 0) // Ray pointing East, hit West face
			return (&game->west_texture);
		else // Ray pointing West, hit East face
			return (&game->east_texture);
	}
	else // NS wall
	{
		if (ray->ray_dir_y > 0) // Ray pointing South, hit North face
			return (&game->north_texture);
		else // Ray pointing North, hit South face
			return (&game->south_texture);
	}
}

// Helper to calculate wall_x (where exactly the ray hit the wall)
static double	calculate_wall_x(t_game_data *game, t_ray_params *ray)
{
	double	wall_x;

	if (ray->side == 0) // EW wall
		wall_x = game->player_y + ray->perp_wall_dist * ray->ray_dir_y;
	else // NS wall
		wall_x = game->player_x + ray->perp_wall_dist * ray->ray_dir_x;
	wall_x -= floor(wall_x);
	return (wall_x);
}

// Helper to calculate texture_x coordinate, with flipping
static int	calculate_texture_x(t_ray_params *ray, t_img *texture, double wall_x)
{
	int	tex_x;

	tex_x = (int)(wall_x * (double)texture->width);
	// Flip texture based on ray direction and wall side
	// If ray hits an East wall (side 0, ray_dir_x < 0), tex_x is as is.
	// If ray hits a West wall (side 0, ray_dir_x > 0), tex_x needs flipping.
	if (ray->side == 0 && ray->ray_dir_x > 0)
		tex_x = texture->width - tex_x - 1;
	// If ray hits a South wall (side 1, ray_dir_y < 0), tex_x is as is.
	// If ray hits a North wall (side 1, ray_dir_y > 0), tex_x needs flipping.
	if (ray->side == 1 && ray->ray_dir_y < 0) // Corrected: South face hit by Northward ray needs flipping
		tex_x = texture->width - tex_x - 1; // My previous comment's logic was inverted for N/S.
											// Standard: N/W faces might be "forward", S/E "flipped"
											// Lodev: If side 0 and rayDirX > 0, flip. If side 1 and rayDirY < 0, flip.
	return (tex_x);
}

static void	draw_textured_stripe(t_game_data *g, t_ray_params *r, int x, int s_h)
{
	t_img	*tex;
	double	wall_x;
	int		tex_x;
	int		line_h;
	int		draw_s;
	int		draw_e;
	double	step;
	double	tex_pos;
	int		tex_y;
	int		color;
	int		y_coord;

	tex = select_texture(g, r);
	if (!tex || !tex->img_ptr) return; // Safety check for missing texture
	wall_x = calculate_wall_x(g, r);
	tex_x = calculate_texture_x(r, tex, wall_x);
	line_h = (int)(s_h / r->perp_wall_dist);
	draw_s = -line_h / 2 + s_h / 2;
	if (draw_s < 0) draw_s = 0;
	draw_e = line_h / 2 + s_h / 2;
	if (draw_e >= s_h) draw_e = s_h - 1;
	step = 1.0 * tex->height / line_h;
	tex_pos = (draw_s - s_h / 2 + line_h / 2) * step;
	y_coord = draw_s;
	while (y_coord <= draw_e)
	{
		tex_y = (int)tex_pos & (tex->height - 1); // Using & for fast modulo if height is power of 2, else use %
		if (tex_y < 0) tex_y = 0; // Clamp
		tex_pos += step;
		color = *(unsigned int *)(tex->addr + (tex_y * tex->line_length + \
				tex_x * (tex->bits_per_pixel / 8)));
		my_mlx_pixel_put(&g->screen_buffer, x, y_coord, color);
		y_coord++;
	}
}

void	cast_all_rays(t_game_data *game)
{
	int				x;
	t_ray_params	ray;

	x = 0;
	while (x < game->screen_width)
	{
		init_ray_directions(game, &ray, x);
		calculate_step_and_side_dist(game, &ray);
		perform_dda(game, &ray);
		if (ray.hit && ray.perp_wall_dist < 1e29) // Check if it's not a boundary hit
		{
			calculate_wall_distance(&ray);
			// Call the new textured drawing function
			draw_textured_stripe(game, &ray, x, game->screen_height);
		}
		x++;
	}
}
