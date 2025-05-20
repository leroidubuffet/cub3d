/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: your_login <your_login@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/28 17:00:00 by your_login          #+#    #+#             */
/*   Updated: 2023/10/28 17:00:00 by your_login         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

# include <stdlib.h>
# include <unistd.h>
# include <string.h> // For basic strlen, strncmp, etc. in libft functions

// GNL Buffer size
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

// Function prototypes
char	*ft_strdup(const char *s1);
char	**ft_split(char const *s, char c);
int		ft_atoi(const char *str);
char	*ft_strtrim(char const *s1, char const *set);
size_t	ft_strlen(const char *s);
char	*ft_strchr(const char *s, int c);
char	*ft_substr(char const *s, unsigned int start, size_t len);
char	*ft_strjoin(char *s1, char *s2); // Modified for GNL to take char *
void	ft_putstr_fd(char *s, int fd); // Example utility
void	ft_putchar_fd(char c, int fd); // Example utility
int		ft_strncmp(const char *s1, const char *s2, size_t n); // For parser

// GNL
char	*get_next_line(int fd);

#endif
