#include "libft.h"

static size_t	count_words(char const *s, char c)
{
	size_t	count;
	int		in_word;

	count = 0;
	in_word = 0;
	while (*s)
	{
		if (*s == c)
			in_word = 0;
		else if (in_word == 0)
		{
			in_word = 1;
			count++;
		}
		s++;
	}
	return (count);
}

static char	*extract_word(char const **s, char c)
{
	char const	*start;
	size_t		len;
	char		*word;

	while (**s == c)
		(*s)++;
	start = *s;
	len = 0;
	while ((*s)[len] && (*s)[len] != c)
		len++;
	word = ft_substr(start, 0, len);
	*s += len;
	return (word);
}

static void	free_tab(char **tab, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n)
	{
		free(tab[i]);
		i++;
	}
	free(tab);
}

char	**ft_split(char const *s, char c)
{
	char	**tab;
	size_t	num_words;
	size_t	i;

	if (!s)
		return (NULL);
	num_words = count_words(s, c);
	tab = (char **)malloc(sizeof(char *) * (num_words + 1));
	if (!tab)
		return (NULL);
	i = 0;
	while (i < num_words)
	{
		tab[i] = extract_word(&s, c);
		if (!tab[i])
		{
			free_tab(tab, i);
			return (NULL);
		}
		i++;
	}
	tab[i] = NULL;
	return (tab);
}
