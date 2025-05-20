/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player_movement.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: your_login <your_login@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 10:05:00 by your_login          #+#    #+#             */
/*   Updated: 2023/10/28 10:05:00 by your_login         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <math.h> // For cos, sin

/*
 * Checks collision at the target (new_x, new_y) position.
 * For this simple check, we only look at the target grid cell.
 * More advanced collision would check a small radius or corners.
 * Returns 1 if collision, 0 if clear.
 */
static int	is_wall(t_game_data *game, double check_x, double check_y)
{
	int	map_x;
	int	map_y;

	map_x = (int)check_x;
	map_y = (int)check_y;
	// Basic boundary check for the map array itself
	if (map_x < 0 || map_x >= game->config.map_width || \
		map_y < 0 || map_y >= game->config.map_height)
		return (1); // Treat out of bounds as a wall
	return (game->config.map_data[map_y][map_x] == '1');
}

void	move_forward_backward(t_game_data *game, double move_dir)
{
	double	new_x;
	double	new_y;

	new_x = game->player_x + game->dir_x * MOVE_SPEED * move_dir;
	new_y = game->player_y + game->dir_y * MOVE_SPEED * move_dir;
	// Check collision for X movement
	if (!is_wall(game, new_x, game->player_y))
		game->player_x = new_x;
	// Check collision for Y movement
	// This check is on the potentially updated player_x for smoother cornering
	if (!is_wall(game, game->player_x, new_y))
		game->player_y = new_y;
}

void	strafe_left_right(t_game_data *game, double strafe_dir)
{
	double	new_x;
	double	new_y;

	// Strafe direction is perpendicular to current direction vector,
	// which is the camera plane vector.
	new_x = game->player_x + game->plane_x * MOVE_SPEED * strafe_dir;
	new_y = game->player_y + game->plane_y * MOVE_SPEED * strafe_dir;
	// Check collision for X movement
	if (!is_wall(game, new_x, game->player_y))
		game->player_x = new_x;
	// Check collision for Y movement
	if (!is_wall(game, game->player_x, new_y))
		game->player_y = new_y;
}

void	rotate_player(t_game_data *game, double rot_angle_multiplier)
{
	double	old_dir_x;
	double	old_plane_x;
	double	angle;

	angle = ROT_SPEED * rot_angle_multiplier;
	old_dir_x = game->dir_x;
	game->dir_x = game->dir_x * cos(angle) - game->dir_y * sin(angle);
	game->dir_y = old_dir_x * sin(angle) + game->dir_y * cos(angle);
	old_plane_x = game->plane_x;
	game->plane_x = game->plane_x * cos(angle) - game->plane_y * sin(angle);
	game->plane_y = old_plane_x * sin(angle) + game->plane_y * cos(angle);
}
