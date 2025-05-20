/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player_setup.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: your_login <your_login@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 16:00:00 by your_login          #+#    #+#             */
/*   Updated: 2023/10/27 16:00:00 by your_login         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h" // For t_game_data and M_PI if needed (not for this part)
#include <math.h>   // For M_PI if needed, but not for basic vector setup

/*
 * Initializes the player's position, direction vector, and camera plane
 * based on the starting orientation parsed from the map.
 * The camera plane is perpendicular to the direction vector.
 * The length of the plane vector (e.g., 0.66) determines the Field of View (FOV).
 * A common FOV is 66 degrees, where tan(FOV/2) = plane_length / dir_length.
 * If dir_length is 1, then plane_length = tan(FOV/2). For 66 deg, tan(33) ~ 0.65.
 */
void	init_player_state(t_game_data *game)
{
	game->player_x = game->config.player_start_x;
	game->player_y = game->config.player_start_y;
	if (game->config.player_orientation == 'N')
	{
		game->dir_x = 0.0;
		game->dir_y = -1.0;
		game->plane_x = 0.66; // Standard FOV
		game->plane_y = 0.0;
	}
	else if (game->config.player_orientation == 'S')
	{
		game->dir_x = 0.0;
		game->dir_y = 1.0;
		game->plane_x = -0.66; // Points opposite to North's plane
		game->plane_y = 0.0;
	}
	else if (game->config.player_orientation == 'E')
	{
		game->dir_x = 1.0;
		game->dir_y = 0.0;
		game->plane_x = 0.0;
		game->plane_y = 0.66; // Plane is perpendicular to East direction
	}
	else if (game->config.player_orientation == 'W')
	{
		game->dir_x = -1.0;
		game->dir_y = 0.0;
		game->plane_x = 0.0;
		game->plane_y = -0.66; // Plane is perpendicular to West direction
	}
	// Optional: else case for error or default if orientation is somehow invalid
	// (should be caught by parser, but good for robustness)
	// else { set to a default, e.g., North, and print warning }
}
