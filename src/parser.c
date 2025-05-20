/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: your_login <your_login@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 10:05:00 by your_login          #+#    #+#             */
/*   Updated: 2023/10/27 10:05:00 by your_login         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h" // Will include libft.h

// Helper to free string array from ft_split
static void	free_split_array(char **array)
{
	int	i;

	if (!array)
		return ;
	i = 0;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}

void	init_config(t_config *config)
{
	config->north_texture_path = NULL;
	config->south_texture_path = NULL;
	config->west_texture_path = NULL;
	config->east_texture_path = NULL;
	config->floor_color_r = -1;
	config->floor_color_g = -1;
	config->floor_color_b = -1;
	config->ceiling_color_r = -1;
	config->ceiling_color_g = -1;
	config->ceiling_color_b = -1;
	config->map_data = NULL;
	config->map_height = 0;
	config->map_width = 0;
	config->player_start_x = -1.0;
	config->player_start_y = -1.0;
	config->player_orientation = '\0';
	config->player_found = 0;
	config->north_texture_set = 0;
	config->south_texture_set = 0;
	config->west_texture_set = 0;
	config->east_texture_set = 0;
	config->floor_color_set = 0;
	config->ceiling_color_set = 0;
}

void	free_config(t_config *config)
{
	int	i;

	if (config->north_texture_path)
		free(config->north_texture_path);
	if (config->south_texture_path)
		free(config->south_texture_path);
	if (config->west_texture_path)
		free(config->west_texture_path);
	if (config->east_texture_path)
		free(config->east_texture_path);
	if (config->map_data)
	{
		i = 0;
		while (i < config->map_height)
		{
			free(config->map_data[i]);
			i++;
		}
		free(config->map_data);
	}
}

static int	parse_texture(const char *line_content, char **texture_path_ptr, int *flag_set)
{
	char *trimmed_path;

	if (*flag_set)
	{
		fprintf(stderr, "Error: Duplicate texture identifier found.\n");
		return (0);
	}
	// ft_strtrim allocates a new string.
	trimmed_path = ft_strtrim(line_content, " \t\n");
	if (!trimmed_path) // Malloc error in ft_strtrim
	{
		perror("Error: ft_strtrim failed for texture path");
		return (0);
	}
	if (ft_strlen(trimmed_path) == 0)
	{
		fprintf(stderr, "Error: Missing texture path after identifier.\n");
		free(trimmed_path);
		return (0);
	}
	*texture_path_ptr = trimmed_path; // Store the trimmed path
	*flag_set = 1;
	return (1);
}

static int	parse_color(const char *line_content, int *r, int *g, int *b, char type, int *flag_set)
{
	char	**rgb_values;
	char	*trimmed_content;
	int		temp_r;
	int		temp_g;
	int		temp_b;

	if (*flag_set)
	{
		fprintf(stderr, "Error: Duplicate color identifier '%c' found.\n", type);
		return (0);
	}
	trimmed_content = ft_strtrim(line_content, " \t\n");
	if (!trimmed_content) // Malloc error
	{
		perror("Error: ft_strtrim failed for color values");
		return (0);
	}
	if (ft_strlen(trimmed_content) == 0)
	{
		fprintf(stderr, "Error: Missing color values for %c.\n", type);
		free(trimmed_content);
		return (0);
	}
	rgb_values = ft_split(trimmed_content, ',');
	free(trimmed_content); // No longer need trimmed_content after split
	if (!rgb_values) // Malloc error in ft_split
	{
		perror("Error: ft_split failed for color values");
		return (0);
	}
	if (!rgb_values[0] || !rgb_values[1] || !rgb_values[2] || rgb_values[3] != NULL)
	{
		fprintf(stderr, "Error: Invalid color format for %c. Expected R,G,B.\n", type);
		free_split_array(rgb_values);
		return (0);
	}
	// Basic validation for numbers
	for (int i_val = 0; i_val < 3; i_val++) {
		for (int j_val = 0; rgb_values[i_val][j_val]; j_val++) {
			if (!((rgb_values[i_val][j_val] >= '0' && rgb_values[i_val][j_val] <= '9') ||
				  (j_val == 0 && rgb_values[i_val][j_val] == '+'))) {
				fprintf(stderr, "Error: Non-numeric value in color for %c.\n", type);
				free_split_array(rgb_values);
				return (0);
			}
		}
	}
	temp_r = ft_atoi(rgb_values[0]);
	temp_g = ft_atoi(rgb_values[1]);
	temp_b = ft_atoi(rgb_values[2]);
	free_split_array(rgb_values); // Free the array from ft_split
	if (temp_r < 0 || temp_r > 255 || temp_g < 0 || temp_g > 255 || temp_b < 0 || temp_b > 255)
	{
		fprintf(stderr, "Error: RGB values out of range [0-255] for %c.\n", type);
		return (0);
	}
	*r = temp_r;
	*g = temp_g;
	*b = temp_b;
	*flag_set = 1;
	return (1);
}

static int	all_elements_parsed(t_config *config)
{
	return (config->north_texture_set && config->south_texture_set && \
			config->west_texture_set && config->east_texture_set && \
			config->floor_color_set && config->ceiling_color_set);
}

static int	process_config_line(char *trimmed_line, t_config *config)
{
	int success;

	success = 0;
	if (strncmp(trimmed_line, "NO ", 3) == 0)
		success = parse_texture(trimmed_line + 3, &config->north_texture_path, &config->north_texture_set);
	else if (ft_strncmp(trimmed_line, "SO ", 3) == 0)
		success = parse_texture(trimmed_line + 3, &config->south_texture_path, &config->south_texture_set);
	else if (ft_strncmp(trimmed_line, "WE ", 3) == 0)
		success = parse_texture(trimmed_line + 3, &config->west_texture_path, &config->west_texture_set);
	else if (ft_strncmp(trimmed_line, "EA ", 3) == 0)
		success = parse_texture(trimmed_line + 3, &config->east_texture_path, &config->east_texture_set);
	else if (ft_strncmp(trimmed_line, "F ", 2) == 0)
		success = parse_color(trimmed_line + 2, &config->floor_color_r, &config->floor_color_g, &config->floor_color_b, 'F', &config->floor_color_set);
	else if (ft_strncmp(trimmed_line, "C ", 2) == 0)
		success = parse_color(trimmed_line + 2, &config->ceiling_color_r, &config->ceiling_color_g, &config->ceiling_color_b, 'C', &config->ceiling_color_set);
	else
	{
		fprintf(stderr, "Error: Invalid identifier in configuration: %s\n", trimmed_line);
		return (0); // Invalid line type for config
	}
	return (success);
}

// --- Map Reading Linked List Helpers ---
void	add_map_line(t_map_line_node **head, char *line_content)
{
	t_map_line_node	*new_node;
	t_map_line_node	*current;

	new_node = (t_map_line_node *)malloc(sizeof(t_map_line_node));
	if (!new_node)
		return (perror("Error: Failed to allocate memory for map line node"));
	new_node->line = ft_strdup(line_content);
	if (!new_node->line) // Malloc error in ft_strdup
	{
		perror("Error: Failed to ft_strdup map line string");
		free(new_node);
		return ;
	}
	new_node->next = NULL;
	if (*head == NULL)
		*head = new_node;
	else
	{
		current = *head;
		while (current->next != NULL)
			current = current->next;
		current->next = new_node;
	}
}

void	free_map_lines(t_map_line_node *head)
{
	t_map_line_node	*current;
	t_map_line_node	*next_node;

	current = head;
	while (current != NULL)
	{
		next_node = current->next;
		free(current->line);
		free(current);
		current = next_node;
	}
}

int	convert_map_lines_to_array(t_map_line_node *head, t_config *config)
{
	t_map_line_node	*current;
	int				line_count;
	int				i;
	size_t			max_width;

	line_count = 0;
	current = head;
	max_width = 0;
	while (current != NULL)
	{
		if (ft_strlen(current->line) > max_width)
			max_width = ft_strlen(current->line);
		line_count++;
		current = current->next;
	}
	config->map_height = line_count;
	config->map_width = max_width;
	config->map_data = (char **)malloc(sizeof(char *) * (line_count + 1));
	if (!config->map_data) return (perror("Error: Malloc failed for map_data rows"), 0);
	current = head;
	i = 0;
	while (i < line_count)
	{
		config->map_data[i] = (char *)malloc(sizeof(char) * (max_width + 1));
		if (!config->map_data[i])
		{
			while (--i >=0) free(config->map_data[i]);
			free(config->map_data);
			config->map_data = NULL;
			return (perror("Error: Malloc failed for map_data column"), 0);
		}
		// Use ft_memcpy or similar if available, or simple loop
		size_t current_line_len = ft_strlen(current->line);
		for (size_t k = 0; k < current_line_len; ++k)
			config->map_data[i][k] = current->line[k];
		for (size_t j = current_line_len; j < max_width; ++j)
			config->map_data[i][j] = ' '; // Pad with spaces
		config->map_data[i][max_width] = '\0';
		current = current->next;
		i++;
	}
	config->map_data[i] = NULL; // Null-terminate the array of strings
	return (1);
}

// --- Map Validation ---
static int store_player_info(t_config *config, int r, int c, char orientation)
{
    if (config->player_found)
    {
        fprintf(stderr, "Error: Multiple player start positions found.\n");
        return (0);
    }
    // Store as center of the cell
    config->player_start_x = (double)c + 0.5;
    config->player_start_y = (double)r + 0.5;
    config->player_orientation = orientation;
    config->player_found = 1;
    config->map_data[r][c] = '0'; // Replace player char with '0' on the map
    return (1);
}

static int is_valid_map_char(char c)
{
    return (c == '0' || c == '1' || c == ' ' || \
            c == 'N' || c == 'S' || c == 'E' || c == 'W');
}

// Basic check: ensure '0's are not adjacent to ' ' or out of bounds.
// This is a simplified check and does not use flood fill.
static int is_map_closed(t_config *config)
{
    int y;
    int x;
    char cell;

    y = 0;
    while (y < config->map_height)
    {
        x = 0;
        while (x < config->map_width)
        {
            cell = config->map_data[y][x];
            if (cell == '0' || cell == config->player_orientation) // Player pos is now '0'
            {
                // Check above
                if (y == 0 || config->map_data[y-1][x] == ' ') return (fprintf(stderr, "Error: Map not closed (cell [%d,%d] top border).\n", y, x), 0);
                // Check below
                if (y == config->map_height - 1 || config->map_data[y+1][x] == ' ') return (fprintf(stderr, "Error: Map not closed (cell [%d,%d] bottom border).\n", y, x), 0);
                // Check left
                if (x == 0 || config->map_data[y][x-1] == ' ') return (fprintf(stderr, "Error: Map not closed (cell [%d,%d] left border).\n", y, x), 0);
                // Check right
                if (x == config->map_width - 1 || config->map_data[y][x+1] == ' ') return (fprintf(stderr, "Error: Map not closed (cell [%d,%d] right border).\n", y, x), 0);
            }
            x++;
        }
        y++;
    }
    return (1);
}


int validate_map(t_config *config)
{
    int y;
    int x;

    if (config->map_height == 0) return (fprintf(stderr, "Error: Map is empty.\n"), 0);
    config->player_found = 0; // Reset before iterating
    y = 0;
    while (y < config->map_height)
    {
        x = 0;
        while (x < config->map_width)
        {
            if (!is_valid_map_char(config->map_data[y][x]))
                return (fprintf(stderr, "Error: Invalid character '%c' in map at [%d,%d].\n", config->map_data[y][x], y, x), 0);
            if (config->map_data[y][x] == 'N' || config->map_data[y][x] == 'S' || \
                config->map_data[y][x] == 'E' || config->map_data[y][x] == 'W')
            {
                if (!store_player_info(config, y, x, config->map_data[y][x]))
                    return (0); // Error already printed by store_player_info
            }
            x++;
        }
        y++;
    }
    if (!config->player_found) return (fprintf(stderr, "Error: No player start position found in map.\n"), 0);
    if (!is_map_closed(config)) return (0); // Error message in is_map_closed
    return (1);
}


// GNL_BUFFER_SIZE is now defined in libft.h
// Removed get_next_line_basic and its static variables.

int	parse_cub_file(const char *filename, t_config *config)
{
	int				fd;
	char			*line; // From GNL, needs to be freed
	char			*trimmed_line; // From ft_strtrim, needs to be freed
	int				line_num;
	int				parsing_map;
	t_map_line_node	*map_lines_head;

	init_config(config);
	map_lines_head = NULL;
	parsing_map = 0;
	fd = open(filename, O_RDONLY);
	if (fd < 0) return (perror("Error opening file"), free_config(config), 0);
	line_num = 0;
	line = get_next_line(fd); // Use libft's GNL
	while (line != NULL)
	{
		line_num++;
		// GNL lines can include newline, ft_strtrim will handle it.
		// The set for ft_strtrim is " \t\n\v\f\r" (standard whitespace)
		trimmed_line = ft_strtrim(line, " \t\n\v\f\r");
		free(line); // Free line from GNL
		if (!trimmed_line) // Malloc error in ft_strtrim
		{
			perror("Error: ft_strtrim failed in parse_cub_file");
			close(fd); free_map_lines(map_lines_head); free_config(config); return (0);
		}
		if (ft_strlen(trimmed_line) == 0) // Skip empty lines or lines with only whitespace
		{
			free(trimmed_line);
			if (parsing_map) // Empty line after map started
				add_map_line(&map_lines_head, ""); // Preserve empty lines in map for structure
			line = get_next_line(fd);
			continue;
		}
		if (!parsing_map && (trimmed_line[0] == '1' || trimmed_line[0] == '0' || trimmed_line[0] == ' '))
		{
			if (!all_elements_parsed(config))
			{
				fprintf(stderr, "Error: Map data encountered before all config elements defined (line %d).\n", line_num);
				free(trimmed_line); free_map_lines(map_lines_head); close(fd); free_config(config); return (0);
			}
			parsing_map = 1;
		}
		if (parsing_map)
		{
			for (size_t i_map = 0; i_map < ft_strlen(trimmed_line); ++i_map) {
				if (!is_valid_map_char(trimmed_line[i_map])) { // is_valid_map_char allows space
					fprintf(stderr, "Error: Invalid char '%c' in map line %d: %s\n", trimmed_line[i_map], line_num, trimmed_line);
					free(trimmed_line); free_map_lines(map_lines_head); close(fd); free_config(config); return(0);
				}
			}
			add_map_line(&map_lines_head, trimmed_line);
		}
		else
		{
			if (!process_config_line(trimmed_line, config))
			{
				fprintf(stderr, "Error parsing config line %d: %s\n", line_num, trimmed_line);
				free(trimmed_line); free_map_lines(map_lines_head); close(fd); free_config(config); return (0);
			}
		}
		free(trimmed_line);
		line = get_next_line(fd);
	}
	close(fd); // Close file descriptor once all lines are read
	if (!all_elements_parsed(config))
	{
		fprintf(stderr, "Error: Missing one or more configuration elements (textures/colors).\n");
		free_map_lines(map_lines_head); free_config(config); return (0);
	}
	if (!map_lines_head && parsing_map) // Started map but got nothing
	{
		fprintf(stderr, "Error: Map definition is empty or missing after starting map section.\n");
		free_map_lines(map_lines_head); free_config(config); return (0);
	}
    if (map_lines_head && !convert_map_lines_to_array(map_lines_head, config))
    {
        free_map_lines(map_lines_head); free_config(config); return (0); // Error in conversion
    }
	free_map_lines(map_lines_head); // List is now converted or was empty
	if (config->map_data && !validate_map(config)) // Only validate if map data exists
	{
		free_config(config); return (0);
	}
    if (!config->map_data && all_elements_parsed(config)) // All elements there, but no map data
    {
        fprintf(stderr, "Error: Map data is missing from the file.\n");
        free_config(config); return (0);
    }
	return (1);
}
