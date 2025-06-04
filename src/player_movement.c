/* ************************************************************************** */
/*                                                                            */
/*   player_movement.c                                                        */
/*                                                                            */
/*   Description:                                                             */
/*     This module handles player movement and rotation within the game       */
/*     world. It includes functions for moving forward, backward, strafing    */
/*     left/right, and rotating the player's view. Collision detection        */
/*     against walls is also managed here to prevent movement into            */
/*     solid map elements.                                                    */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h" // For t_game_data, MOVE_SPEED, ROT_SPEED, map data
#include <math.h>   // For cos, sin used in rotation

/**
 * @brief Checks if a given world coordinate (check_x, check_y) is a wall.
 *
 * Converts the floating-point world coordinates to integer map grid coordinates.
 * Performs boundary checks to ensure the coordinates are within the map dimensions.
 * If within bounds, it checks the map data array (game->config.map_data)
 * to see if the character at that grid cell is '1' (a wall).
 *
 * @param game Pointer to the t_game_data struct, containing map dimensions and data.
 * @param check_x The x-coordinate in world space to check.
 * @param check_y The y-coordinate in world space to check.
 * @return 1 if the coordinate is a wall or out of bounds, 0 otherwise (clear path).
 */
static int	is_wall(t_game_data *game, double check_x, double check_y)
{
	int	map_x; // Integer map grid x-coordinate.
	int	map_y; // Integer map grid y-coordinate.

	// Convert world coordinates to map grid coordinates by truncation.
	map_x = (int)check_x;
	map_y = (int)check_y;

	// Boundary check: if target coordinates are outside the map, treat as a wall.
	if (map_x < 0 || map_x >= game->config.map_width || \
		map_y < 0 || map_y >= game->config.map_height)
		return (1); // Out of bounds is considered a wall.

	// Check the character in the map data at the target grid cell.
	return (game->config.map_data[map_y][map_x] == '1'); // Return 1 if it's a wall ('1'), 0 otherwise.
}

/**
 * @brief Moves the player forward or backward based on their current direction.
 *
 * Calculates the potential new x and y coordinates based on the player's
 * direction vector (dir_x, dir_y), movement speed (MOVE_SPEED), and the
 * move_dir (-1 for backward, 1 for forward).
 * Implements a simple "sliding walls" collision detection:
 * - It first checks if the new x-position is valid while keeping the current y-position.
 *   If clear, the player's x-position is updated.
 * - Then, it checks if the new y-position is valid using the (potentially updated)
 *   x-position. If clear, the player's y-position is updated.
 * This allows the player to slide along walls if moving diagonally towards them.
 *
 * @param game Pointer to the t_game_data struct.
 * @param move_dir Multiplier for movement: 1.0 for forward, -1.0 for backward.
 */
void	move_forward_backward(t_game_data *game, double move_dir)
{
	double	new_x; // Potential new x-coordinate.
	double	new_y; // Potential new y-coordinate.

	// Calculate potential new position.
	new_x = game->player_x + game->dir_x * MOVE_SPEED * move_dir;
	new_y = game->player_y + game->dir_y * MOVE_SPEED * move_dir;

	// Check collision for the x-component of the movement.
	// If moving to (new_x, game->player_y) is not a wall, update player_x.
	if (!is_wall(game, new_x, game->player_y))
		game->player_x = new_x;

	// Check collision for the y-component of the movement.
	// This uses the (potentially) updated game->player_x. If moving from the
	// new x-position to (game->player_x, new_y) is not a wall, update player_y.
	// This allows sliding along walls.
	if (!is_wall(game, game->player_x, new_y))
		game->player_y = new_y;
}

/**
 * @brief Moves (strafes) the player left or right relative to their current direction.
 *
 * Calculates the potential new x and y coordinates using the camera plane vector
 * (plane_x, plane_y), which is perpendicular to the player's direction vector.
 * Movement speed (MOVE_SPEED) and strafe_dir (-1 for left, 1 for right) are applied.
 * Uses the same "sliding walls" collision detection as move_forward_backward:
 * checks X-axis movement first, then Y-axis movement.
 *
 * @param game Pointer to the t_game_data struct.
 * @param strafe_dir Multiplier for strafing: 1.0 for right, -1.0 for left.
 */
void	strafe_left_right(t_game_data *game, double strafe_dir)
{
	double	new_x; // Potential new x-coordinate.
	double	new_y; // Potential new y-coordinate.

	// Calculate potential new position using the camera plane vector for perpendicular movement.
	new_x = game->player_x + game->plane_x * MOVE_SPEED * strafe_dir;
	new_y = game->player_y + game->plane_y * MOVE_SPEED * strafe_dir;

	// Check collision for the x-component of the strafe.
	if (!is_wall(game, new_x, game->player_y))
		game->player_x = new_x;

	// Check collision for the y-component of the strafe.
	if (!is_wall(game, game->player_x, new_y))
		game->player_y = new_y;
}

/**
 * @brief Rotates the player's viewing direction and camera plane.
 *
 * Applies a 2D rotation matrix to both the player's direction vector (dir_x, dir_y)
 * and the camera plane vector (plane_x, plane_y).
 * The rotation angle is determined by ROT_SPEED and rot_angle_multiplier.
 *
 * Rotation matrix for angle `a`:
 *   [ cos(a)  -sin(a) ]
 *   [ sin(a)   cos(a) ]
 * New dir_x = old_dir_x * cos(a) - old_dir_y * sin(a)
 * New dir_y = old_dir_x * sin(a) + old_dir_y * cos(a)
 * Same transformation applies to the camera plane vector.
 *
 * @param game Pointer to the t_game_data struct.
 * @param rot_angle_multiplier Multiplier for rotation: 1.0 for right, -1.0 for left.
 */
void	rotate_player(t_game_data *game, double rot_angle_multiplier)
{
	double	old_dir_x;   // Store original dir_x before modification for dir_y calculation.
	double	old_plane_x; // Store original plane_x for plane_y calculation.
	double	angle;       // The rotation angle in radians.

	angle = ROT_SPEED * rot_angle_multiplier; // Calculate actual rotation angle.

	// Rotate the direction vector.
	old_dir_x = game->dir_x;
	game->dir_x = game->dir_x * cos(angle) - game->dir_y * sin(angle);
	game->dir_y = old_dir_x * sin(angle) + game->dir_y * cos(angle);

	// Rotate the camera plane vector.
	old_plane_x = game->plane_x;
	game->plane_x = game->plane_x * cos(angle) - game->plane_y * sin(angle);
	game->plane_y = old_plane_x * sin(angle) + game->plane_y * cos(angle);
}
