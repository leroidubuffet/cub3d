#include "libft.h"

char	*ft_strjoin(char *s1, char *s2)
{
	char	*join;
	size_t	s1_len;
	size_t	s2_len;
	size_t	i;
	size_t	j;

	if (!s1)
		return (ft_strdup(s2));
	if (!s2)
		return (ft_strdup(s1));
	s1_len = ft_strlen(s1);
	s2_len = ft_strlen(s2);
	join = (char *)malloc(sizeof(char) * (s1_len + s2_len + 1));
	if (!join)
		return (NULL);
	i = 0;
	while (i < s1_len)
	{
		join[i] = s1[i];
		i++;
	}
	j = 0;
	while (j < s2_len)
	{
		join[i + j] = s2[j];
		j++;
	}
	join[i + j] = '\0';
	free(s1); // GNL specific: free the old stash
	return (join);
}
