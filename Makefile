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

# Project test source files (subset of SRCS needed for tests)
PROJECT_TEST_SRCS_DIR = src/
PROJECT_TEST_SRCS = $(addprefix $(PROJECT_TEST_SRCS_DIR), parser.c) # Example: only parser.c

# Test specific source files
TEST_SRCS_DIR = tests/
TEST_SRCS = $(addprefix $(TEST_SRCS_DIR), test_parser.c)

# Object files
OBJS_DIR = obj/
OBJS = $(patsubst $(SRCS_DIR)%.c, $(OBJS_DIR)%.o, $(SRCS))
PROJECT_TEST_OBJS = $(patsubst $(PROJECT_TEST_SRCS_DIR)%.c, $(OBJS_DIR)%.o, $(PROJECT_TEST_SRCS))
TEST_OBJS_DIR = $(OBJS_DIR)tests/
TEST_OBJS = $(patsubst $(TEST_SRCS_DIR)%.c, $(TEST_OBJS_DIR)%.o, $(TEST_SRCS))

# Test executable name
TEST_NAME = test_cub3d

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

# Rule for project source files (main program)
$(OBJS_DIR)%.o: $(SRCS_DIR)%.c $(INCLUDES_DIR)cub3d.h Makefile
	@mkdir -p $(OBJS_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Rule for project test source files (subset used in tests)
# This rule is similar to the one above but uses PROJECT_TEST_SRCS_DIR
# $(OBJS_DIR)%.o: $(PROJECT_TEST_SRCS_DIR)%.c $(INCLUDES_DIR)cub3d.h Makefile
# 	@mkdir -p $(OBJS_DIR)
# 	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Rule for test source files
$(TEST_OBJS_DIR)%.o: $(TEST_SRCS_DIR)%.c $(INCLUDES_DIR)cub3d.h Makefile
	@mkdir -p $(TEST_OBJS_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIBFT_A):
	@echo "Making libft..."
	@$(MAKE) -C $(LIBFT_DIR) all

test: $(TEST_NAME)
	@./$(TEST_NAME)

# LDFLAGS for tests (excluding MLX)
TEST_LDFLAGS = -Llibft -lft -lm

$(TEST_NAME): $(PROJECT_TEST_OBJS) $(TEST_OBJS) $(LIBFT_A)
	@$(MAKE) -C $(LIBFT_DIR) # Ensure libft is up to date
	@$(CC) $(CFLAGS) $(INCLUDES) $(PROJECT_TEST_OBJS) $(TEST_OBJS) $(TEST_LDFLAGS) -o $(TEST_NAME)
	@echo "$(TEST_NAME) compiled!"

clean:
	@$(MAKE) -C $(LIBFT_DIR) clean
	@rm -rf $(OBJS_DIR)
	@echo "Object files cleaned!"

fclean: clean
	@$(MAKE) -C $(LIBFT_DIR) fclean
	@rm -f $(NAME)
	@rm -f $(TEST_NAME) # Also clean the test executable
	@echo "$(NAME) and $(TEST_NAME) executables cleaned!"

re: fclean all

bonus: # Placeholder for bonus rule
	@echo "Bonus rule not yet implemented."

# Phony targets
.PHONY: all clean fclean re bonus test
