/* ************************************************************************** */
/*                                                                            */
/*   player_setup.c                                                           */
/*                                                                            */
/*   Description:                                                             */
/*     This module is responsible for initializing the player's state at the  */
/*     start of the game. This includes setting the player's initial          */
/*     position on the map, their starting viewing direction, and the         */
/*     camera plane vector, which is crucial for the raycasting algorithm.    */
/*     The initial state is derived from the data parsed from the .cub file,  */
/*     specifically the player's starting coordinates (player_start_x,       */
/*     player_start_y) and orientation character ('N', 'S', 'E', 'W').        */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h" // For t_game_data
#include <math.h>   // For mathematical functions if ever needed for more complex setups.

/**
 * @brief Initializes the player's state based on parsed configuration.
 *
 * Sets the player's initial position (player_x, player_y) directly from the
 * configuration values (config.player_start_x, config.player_start_y).
 *
 * Determines the initial direction vector (dir_x, dir_y) and camera plane vector
 * (plane_x, plane_y) based on the player's starting orientation character
 * (config.player_orientation: 'N', 'S', 'E', 'W').
 *
 * - Direction Vector: A unit vector pointing in the direction the player is facing.
 *   - 'N': (0, -1)  (North is typically negative Y in 2D Cartesian for graphics)
 *   - 'S': (0, 1)
 *   - 'E': (1, 0)
 *   - 'W': (-1, 0)
 *
 * - Camera Plane: A vector perpendicular to the direction vector. Its length
 *   influences the Field of View (FOV). A common length is 0.66, which
 *   approximates a 66-degree FOV (tan(66_degrees / 2) approx 0.65).
 *   The camera plane points to the right relative to the direction vector by convention for raycasting.
 *   - For (dir_x, dir_y), the perpendicular plane is typically (-dir_y * L, dir_x * L) or (dir_y * L, -dir_x * L).
 *     Choosing (0.66, 0) for dir(0,-1) means plane_x is to the right.
 *   - 'N' (dir 0,-1): plane (0.66, 0)
 *   - 'S' (dir 0,1):  plane (-0.66, 0) (maintains rightward relative plane)
 *   - 'E' (dir 1,0):  plane (0, 0.66)
 *   - 'W' (dir -1,0): plane (0, -0.66)
 *
 * @param game Pointer to the t_game_data struct where the player state will be written.
 *             The game->config field must already be populated by the parser.
 */
void	init_player_state(t_game_data *game)
{
	// Set player's initial world coordinates from parsed config.
	game->player_x = game->config.player_start_x;
	game->player_y = game->config.player_start_y;

	// Initialize direction and camera plane vectors to zero before setting.
	game->dir_x = 0.0;
	game->dir_y = 0.0;
	game->plane_x = 0.0;
	game->plane_y = 0.0;

	// Set direction and camera plane based on player's starting orientation.
	if (game->config.player_orientation == 'N')
	{
		game->dir_y = -1.0;  // Facing North (negative Y direction).
		game->plane_x = 0.66; // Camera plane is to the right (positive X).
	}
	else if (game->config.player_orientation == 'S')
	{
		game->dir_y = 1.0;   // Facing South (positive Y direction).
		game->plane_x = -0.66;// Camera plane is to the right (negative X, as view is flipped).
	}
	else if (game->config.player_orientation == 'E')
	{
		game->dir_x = 1.0;   // Facing East (positive X direction).
		game->plane_y = 0.66; // Camera plane is to the right (positive Y).
	}
	else if (game->config.player_orientation == 'W')
	{
		game->dir_x = -1.0;  // Facing West (negative X direction).
		game->plane_y = -0.66;// Camera plane is to the right (negative Y).
	}
	// An else case could set a default orientation (e.g., North) and print a
	// warning if player_orientation was somehow invalid, but the parser's
	// validation should prevent this scenario.
}
