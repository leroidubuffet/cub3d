# Makefile for libft

NAME = libft.a
CC = cc
CFLAGS = -Wall -Wextra -Werror
AR = ar rcs

# Source files
SRCS_DIR = ./
SRCS =	ft_strlen.c \
		ft_strdup.c \
		ft_strchr.c \
		ft_substr.c \
		ft_strjoin.c \
		ft_split.c \
		ft_atoi.c \
		ft_strtrim.c \
		ft_strncmp.c \
		get_next_line.c \
		# Add other .c files here if you create more for a fuller libft

# Object files
OBJS_DIR = obj/
OBJS = $(addprefix $(OBJS_DIR), $(SRCS:.c=.o))

# Includes - libft.h is in the current directory
INCLUDES = -I./

all: $(NAME)

$(NAME): $(OBJS)
	@$(AR) $(NAME) $(OBJS)
	@echo "libft.a compiled and created!"

$(OBJS_DIR)%.o: $(SRCS_DIR)%.c libft.h
	@mkdir -p $(OBJS_DIR)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@rm -rf $(OBJS_DIR) $(OBJS) # Ensure .o files in root are also cleaned if obj/ fails
	@echo "libft object files cleaned!"

fclean: clean
	@rm -f $(NAME)
	@echo "libft.a cleaned!"

re: fclean all

.PHONY: all clean fclean re
