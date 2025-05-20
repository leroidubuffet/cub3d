# Main Makefile for cub3D

NAME = cub3D
CC = cc
CFLAGS = -Wall -Wextra -Werror
# Assuming MiniLibX is installed in a standard location or headers/libs are in compiler search paths.
# Common MLX flags for Linux. For macOS, -framework OpenGL -framework AppKit might be used with mlx.
# For this pass, I'll use the typical X11 flags.
MLX_FLAGS = -lmlx -lXext -lX11 -lm
LDFLAGS = -Llibft -lft $(MLX_FLAGS)

# Source files
SRCS_DIR = src/
SRCS = $(addprefix $(SRCS_DIR), main.c parser.c mlx_setup.c render.c player_setup.c raycaster.c player_movement.c texture_loader.c) # Added texture_loader.c

# Object files
OBJS_DIR = obj/
OBJS = $(patsubst $(SRCS_DIR)%.c, $(OBJS_DIR)%.o, $(SRCS))

# Includes
INCLUDES_DIR = includes/
INCLUDES = -I$(INCLUDES_DIR) -Ilibft # libft.h is directly in libft/
# If mlx.h is not in a standard path, add -I/path/to/mlx here

# Libft
LIBFT_DIR = libft/
LIBFT_A = $(LIBFT_DIR)libft.a

# Rules
all: $(NAME)

$(NAME): $(OBJS) $(LIBFT_A)
	@$(MAKE) -C $(LIBFT_DIR) # Make libft first
	@$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) $(LDFLAGS) -o $(NAME) # Link main project with libft.a
	@echo "$(NAME) compiled!"

$(OBJS_DIR)%.o: $(SRCS_DIR)%.c $(INCLUDES_DIR)cub3d.h
	@mkdir -p $(OBJS_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIBFT_A):
	@echo "Making libft..."
	@$(MAKE) -C $(LIBFT_DIR) all

clean:
	@$(MAKE) -C $(LIBFT_DIR) clean
	@rm -rf $(OBJS_DIR)
	@echo "Object files cleaned!"

fclean: clean
	@$(MAKE) -C $(LIBFT_DIR) fclean
	@rm -f $(NAME)
	@echo "$(NAME) executable cleaned!"

re: fclean all

bonus: # Placeholder for bonus rule
	@echo "Bonus rule not yet implemented."

# Phony targets
.PHONY: all clean fclean re bonus
