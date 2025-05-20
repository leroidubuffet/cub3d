/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: your_login <your_login@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 10:00:00 by your_login          #+#    #+#             */
/*   Updated: 2023/10/28 10:00:00 by your_login         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H

# include <stdlib.h> // For malloc, free
# include <stdio.h>  // For printf (for testing) and perror
# include <fcntl.h>  // For open
# include <unistd.h> // For read, close (used by GNL)
# include "mlx.h"   // MiniLibX header
# include "libft.h" // Libft header

// string.h is no longer needed if all string ops are via libft

// --- Constants ---
// BUFFER_SIZE for GNL is now defined in libft.h
# define SCREEN_WIDTH 1024
# define SCREEN_HEIGHT 768

// Keycodes
# ifdef __APPLE__ // macOS
#  define KEY_ESC 53
#  define KEY_W 13
#  define KEY_A 0
#  define KEY_S 1
#  define KEY_D 2
#  define KEY_LEFT_ARROW 123
#  define KEY_RIGHT_ARROW 124
# else // Linux
#  define KEY_ESC 65307
#  define KEY_W 119
#  define KEY_A 97
#  define KEY_S 115
#  define KEY_D 100
#  define KEY_LEFT_ARROW 65361
#  define KEY_RIGHT_ARROW 65363
# endif

// Movement and Rotation Speeds
# define MOVE_SPEED 0.1    // Units per frame
# define ROT_SPEED 0.05     // Radians per frame

// --- Data Structures ---

typedef struct s_config
{
	char	*north_texture_path;
	char	*south_texture_path;
	char	*west_texture_path;
	char	*east_texture_path;
	int		floor_color_r;
	int		floor_color_g;
	int		floor_color_b;
	int		ceiling_color_r;
	int		ceiling_color_g;
	int		ceiling_color_b;
	char	**map_data;
	int		map_height;
	int		map_width;
	double	player_start_x;
	double	player_start_y;
	char	player_orientation;
	int		player_found;
	int		north_texture_set;
	int		south_texture_set;
	int		west_texture_set;
	int		east_texture_set;
	int		floor_color_set;
	int		ceiling_color_set;
}	t_config;

typedef struct s_img
{
	void	*img_ptr;
	char	*addr;
	int		bits_per_pixel;
	int		line_length;
	int		endian;
	int		width;  // Added texture width
	int		height; // Added texture height
}	t_img;

typedef struct s_game_data
{
	void		*mlx_ptr;
	void		*win_ptr;
	t_config	config;
	t_img		screen_buffer;
	int			screen_width;
	int			screen_height;
	double		player_x;
	double		player_y;
	double		dir_x;
	double		dir_y;
	double		plane_x;
	double		plane_y;
	// Wall textures
	t_img		north_texture;
	t_img		south_texture;
	t_img		east_texture;
	t_img		west_texture;
}	t_game_data;

// --- Function Prototypes ---

// Parser (parser.c)
int		parse_cub_file(const char *filename, t_config *config);
void	init_config(t_config *config);
void	free_config(t_config *config);
int		validate_map(t_config *config);
// Map reading helpers (parser.c or map_utils.c)
typedef struct s_map_line_node
{
	char					*line;
	struct s_map_line_node	*next;
}	t_map_line_node;

void	add_map_line(t_map_line_node **head, char *line);
void	free_map_lines(t_map_line_node *head);
int		convert_map_lines_to_array(t_map_line_node *head, t_config *config);


// MLX and Hooks (mlx_setup.c)
int		init_mlx(t_game_data *game);
void	cleanup_mlx(t_game_data *game);
int		handle_keypress(int keycode, t_game_data *game);
int		handle_window_close(t_game_data *game);
void	setup_hooks(t_game_data *game);

// Player Setup (player_setup.c)
void	init_player_state(t_game_data *game);

// Player Movement (player_movement.c)
void	move_forward_backward(t_game_data *game, double move_dir);
void	strafe_left_right(t_game_data *game, double strafe_dir);
void	rotate_player(t_game_data *game, double rot_angle_multiplier);

// Texture Loading (texture_loader.c)
int		load_all_textures(t_game_data *game);
void	free_all_textures(t_game_data *game);

// Raycasting (raycaster.c)
void	cast_all_rays(t_game_data *game);

// Rendering (render.c)
void	my_mlx_pixel_put(t_img *img, int x, int y, int color);
int		rgb_to_int(int r, int g, int b);
void	draw_floor_and_ceiling(t_game_data *game);
int		render_next_frame(t_game_data *game);

#endif
