/* ************************************************************************** */
/*                                                                            */
/*   parser.c                                                                 */
/*                                                                            */
/*   Description:                                                             */
/*     This module is responsible for parsing the .cub scene description      */
/*     file. It reads and validates texture paths, floor/ceiling color        */
/*     settings, and the 2D map layout. It ensures that the input data        */
/*     conforms to the expected format and that the map structure is valid    */
/*     (e.g., closed, contains a player start).                               */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h" // Will include libft.h for ft_ GNL, etc.

/**
 * @brief Frees a NULL-terminated array of strings (char **).
 * Typically used to free the result of ft_split.
 * @param array The array of strings to free.
 */
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

// init_config is documented in cub3d.h
void	init_config(t_config *config)
{
	config->north_texture_path = NULL;
	config->south_texture_path = NULL;
	config->west_texture_path = NULL;
	config->east_texture_path = NULL;
	config->floor_color_r = -1; // Use -1 to indicate not set
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
	config->player_orientation = '\0'; // Null char for no orientation
	config->player_found = 0;
	config->north_texture_set = 0;
	config->south_texture_set = 0;
	config->west_texture_set = 0;
	config->east_texture_set = 0;
	config->floor_color_set = 0;
	config->ceiling_color_set = 0;
}

// free_config is documented in cub3d.h
void	free_config(t_config *config)
{
	int	i;

	if (config->north_texture_path)
	{
		free(config->north_texture_path);
		config->north_texture_path = NULL;
	}
	if (config->south_texture_path)
	{
		free(config->south_texture_path);
		config->south_texture_path = NULL;
	}
	if (config->west_texture_path)
	{
		free(config->west_texture_path);
		config->west_texture_path = NULL;
	}
	if (config->east_texture_path)
	{
		free(config->east_texture_path);
		config->east_texture_path = NULL;
	}
	if (config->map_data)
	{
		i = 0;
		while (i < config->map_height) // Only free up to map_height
		{
			free(config->map_data[i]);
			config->map_data[i] = NULL;
			i++;
		}
		free(config->map_data);
		config->map_data = NULL;
	}
}

/**
 * @brief Parses a texture path from a line in the .cub file.
 *
 * Validates that the texture has not been previously set (duplicate check).
 * Trims whitespace from the path. Ensures the path is not empty.
 * Stores the allocated trimmed path in the t_config struct and sets the corresponding flag.
 *
 * @param line_content The content of the line after the texture identifier (e.g., "NO ").
 * @param texture_path_ptr Pointer to the char* in t_config where the path should be stored.
 * @param flag_set Pointer to the flag (e.g., config->north_texture_set) indicating if this texture type has been set.
 * @return 1 on success, 0 on failure (e.g., duplicate, empty path, allocation error).
 */
static int	parse_texture(const char *line_content, char **texture_path_ptr, int *flag_set)
{
	char *trimmed_path;

	if (*flag_set) // Check for duplicate texture identifier
	{
		fprintf(stderr, "Error: Duplicate texture identifier found.\n");
		return (0);
	}
	// Trim leading/trailing whitespace from the path. ft_strtrim allocates memory.
	trimmed_path = ft_strtrim(line_content, " \t\n");
	if (!trimmed_path) // Allocation failure in ft_strtrim
	{
		perror("Error: ft_strtrim failed for texture path");
		return (0);
	}
	if (ft_strlen(trimmed_path) == 0) // Path is empty after trimming
	{
		fprintf(stderr, "Error: Missing texture path after identifier.\n");
		free(trimmed_path); // Free the empty allocated string
		return (0);
	}
	*texture_path_ptr = trimmed_path; // Store the successfully parsed and trimmed path.
	*flag_set = 1; // Mark this texture type as set.
	return (1);
}

/**
 * @brief Parses an RGB color definition from a line in the .cub file.
 *
 * Validates for duplicate color definitions. Trims the content, splits it by commas.
 * Ensures exactly three components (R,G,B) are present.
 * Validates that each component is a number and within the range [0-255].
 * Stores the parsed R, G, B values in the t_config struct and sets the corresponding flag.
 *
 * @param line_content Content of the line after the color identifier (e.g., "F ").
 * @param r Pointer to store the Red component.
 * @param g Pointer to store the Green component.
 * @param b Pointer to store the Blue component.
 * @param type Character representing the color type ('F' for floor, 'C' for ceiling), used for error messages.
 * @param flag_set Pointer to the flag (e.g., config->floor_color_set) indicating if this color type has been set.
 * @return 1 on success, 0 on failure (e.g., duplicate, format error, value out of range, allocation error).
 */
static int	parse_color(const char *line_content, int *r, int *g, int *b, char type, int *flag_set)
{
	char	**rgb_values; // Array to hold R,G,B strings after splitting.
	char	*trimmed_content; // Line content after whitespace trimming.
	int		temp_r, temp_g, temp_b; // Temporary storage for parsed color values.

	if (*flag_set) // Check for duplicate color identifier
	{
		fprintf(stderr, "Error: Duplicate color identifier '%c' found.\n", type);
		return (0);
	}
	trimmed_content = ft_strtrim(line_content, " \t\n");
	if (!trimmed_content) // Allocation failure
	{
		perror("Error: ft_strtrim failed for color values");
		return (0);
	}
	if (ft_strlen(trimmed_content) == 0) // Color definition is empty
	{
		fprintf(stderr, "Error: Missing color values for %c.\n", type);
		free(trimmed_content);
		return (0);
	}
	// Split the trimmed content by commas to get individual R,G,B strings.
	rgb_values = ft_split(trimmed_content, ',');
	free(trimmed_content); // Original trimmed string no longer needed.
	if (!rgb_values) // Allocation failure in ft_split
	{
		perror("Error: ft_split failed for color values");
		return (0);
	}
	// Validate correct number of components (must be 3: R, G, B).
	if (!rgb_values[0] || !rgb_values[1] || !rgb_values[2] || rgb_values[3] != NULL)
	{
		fprintf(stderr, "Error: Invalid color format for %c. Expected R,G,B (e.g., 255,100,0).\n", type);
		free_split_array(rgb_values);
		return (0);
	}
	// Validate that each component is numeric.
	for (int i_val = 0; i_val < 3; i_val++) {
		for (int j_val = 0; rgb_values[i_val][j_val]; j_val++) {
			// Allow digits, or a single leading '+' (ft_atoi handles this).
			if (!ft_isdigit(rgb_values[i_val][j_val]) && \
			    !(j_val == 0 && rgb_values[i_val][j_val] == '+')) {
				fprintf(stderr, "Error: Non-numeric value in color component for %c: '%s'.\n", type, rgb_values[i_val]);
				free_split_array(rgb_values);
				return (0);
			}
		}
	}
	// Convert string components to integers.
	temp_r = ft_atoi(rgb_values[0]);
	temp_g = ft_atoi(rgb_values[1]);
	temp_b = ft_atoi(rgb_values[2]);
	free_split_array(rgb_values); // Free the array of string components.
	// Validate that RGB values are within the valid range [0-255].
	if (temp_r < 0 || temp_r > 255 || temp_g < 0 || temp_g > 255 || temp_b < 0 || temp_b > 255)
	{
		fprintf(stderr, "Error: RGB values out of range [0-255] for %c.\n", type);
		return (0);
	}
	// Store parsed values and set flag.
	*r = temp_r;
	*g = temp_g;
	*b = temp_b;
	*flag_set = 1;
	return (1);
}

/**
 * @brief Checks if all mandatory configuration elements (textures and colors) have been parsed.
 * @param config Pointer to the t_config struct.
 * @return 1 if all elements are set, 0 otherwise.
 */
static int	all_elements_parsed(t_config *config)
{
	return (config->north_texture_set && config->south_texture_set && \
			config->west_texture_set && config->east_texture_set && \
			config->floor_color_set && config->ceiling_color_set);
}

/**
 * @brief Processes a single line of configuration from the .cub file.
 *
 * Identifies the type of configuration (NO, SO, WE, EA, F, C) based on the prefix
 * and calls the appropriate parsing function (parse_texture or parse_color).
 *
 * @param trimmed_line A line from the .cub file, already trimmed of leading/trailing whitespace.
 * @param config Pointer to the t_config struct to be populated.
 * @return 1 if the line was successfully parsed as a valid config item, 0 otherwise
 *         (e.g., invalid identifier, or error from parse_texture/parse_color).
 */
static int	process_config_line(char *trimmed_line, t_config *config)
{
	int success = 0; // Default to failure

	// Check for texture identifiers (NO, SO, WE, EA)
	if (ft_strncmp(trimmed_line, "NO ", 3) == 0)
		success = parse_texture(trimmed_line + 3, &config->north_texture_path, &config->north_texture_set);
	else if (ft_strncmp(trimmed_line, "SO ", 3) == 0)
		success = parse_texture(trimmed_line + 3, &config->south_texture_path, &config->south_texture_set);
	else if (ft_strncmp(trimmed_line, "WE ", 3) == 0)
		success = parse_texture(trimmed_line + 3, &config->west_texture_path, &config->west_texture_set);
	else if (ft_strncmp(trimmed_line, "EA ", 3) == 0)
		success = parse_texture(trimmed_line + 3, &config->east_texture_path, &config->east_texture_set);
	// Check for color identifiers (F, C)
	else if (ft_strncmp(trimmed_line, "F ", 2) == 0)
		success = parse_color(trimmed_line + 2, &config->floor_color_r, &config->floor_color_g, &config->floor_color_b, 'F', &config->floor_color_set);
	else if (ft_strncmp(trimmed_line, "C ", 2) == 0)
		success = parse_color(trimmed_line + 2, &config->ceiling_color_r, &config->ceiling_color_g, &config->ceiling_color_b, 'C', &config->ceiling_color_set);
	else // Not a recognized configuration identifier
	{
		fprintf(stderr, "Error: Invalid identifier in configuration line: %s\n", trimmed_line);
		return (0);
	}
	return (success);
}

// --- Map Reading Linked List Helpers ---
// add_map_line is documented in cub3d.h
void	add_map_line(t_map_line_node **head, char *line_content)
{
	t_map_line_node	*new_node;
	t_map_line_node	*current;

	new_node = (t_map_line_node *)malloc(sizeof(t_map_line_node));
	if (!new_node)
	{
		perror("Error: Failed to allocate memory for map line node");
		return ; // Return on error, let parse_cub_file handle cleanup if needed
	}
	new_node->line = ft_strdup(line_content);
	if (!new_node->line)
	{
		perror("Error: Failed to ft_strdup map line string");
		free(new_node);
		return ; // Return on error
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

// free_map_lines is documented in cub3d.h
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

/**
 * @brief Converts a linked list of map lines to a 2D char array.
 *
 * Iterates through the linked list to determine dimensions (height, max_width).
 * Allocates memory for the 2D array (config->map_data).
 * Copies lines from the list to the array, padding shorter lines with spaces (' ')
 * to ensure the map is rectangular. The array is NULL-terminated.
 *
 * @param head Head of the linked list of map lines.
 * @param config Pointer to t_config struct where map_data, map_height, map_width are stored.
 * @return 1 on success, 0 on memory allocation failure.
 */
int	convert_map_lines_to_array(t_map_line_node *head, t_config *config)
{
	t_map_line_node	*current;
	int				line_count;
	int				i;
	size_t			max_width; // Use size_t for lengths

	// First pass: calculate height and maximum width of the map.
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
	config->map_width = (int)max_width; // Store as int
	// Allocate rows for map_data (array of char pointers).
	config->map_data = (char **)malloc(sizeof(char *) * (line_count + 1)); // +1 for NULL terminator
	if (!config->map_data) return (perror("Error: Malloc failed for map_data rows"), 0);
	// Second pass: allocate each row and copy/pad line content.
	current = head;
	i = 0;
	while (i < line_count)
	{
		config->map_data[i] = (char *)malloc(sizeof(char) * (max_width + 1)); // +1 for '\0'
		if (!config->map_data[i]) // Allocation failure for a row
		{ // Cleanup previously allocated rows
			while (--i >=0) free(config->map_data[i]);
			free(config->map_data);
			config->map_data = NULL; // Ensure it's NULL on error
			return (perror("Error: Malloc failed for map_data column"), 0);
		}
		// Copy the line content and pad with spaces if it's shorter than max_width.
		size_t current_line_len = ft_strlen(current->line);
		ft_memcpy(config->map_data[i], current->line, current_line_len);
		for (size_t j = current_line_len; j < max_width; ++j)
			config->map_data[i][j] = ' '; // Pad with spaces
		config->map_data[i][max_width] = '\0'; // Null-terminate the row.
		current = current->next;
		i++;
	}
	config->map_data[i] = NULL; // Null-terminate the array of strings itself.
	return (1);
}

// --- Map Validation ---

/**
 * @brief Stores player starting information if valid.
 * Checks for multiple player definitions. Updates map to replace player char with '0'.
 * @param config Pointer to t_config struct.
 * @param r Row index of player.
 * @param c Column index of player.
 * @param orientation Player's orientation character ('N', 'S', 'E', 'W').
 * @return 1 if player info stored, 0 if multiple players found.
 */
static int store_player_info(t_config *config, int r, int c, char orientation)
{
    if (config->player_found) // Check if a player has already been found
    {
        fprintf(stderr, "Error: Multiple player start positions found in map.\n");
        return (0); // Indicate error: multiple players
    }
    // Store player's starting position (center of the grid cell) and orientation.
    config->player_start_x = (double)c + 0.5;
    config->player_start_y = (double)r + 0.5;
    config->player_orientation = orientation;
    config->player_found = 1; // Mark player as found.
    config->map_data[r][c] = '0'; // Replace player character on map with '0' (walkable space).
    return (1);
}

/**
 * @brief Checks if a character is a valid map character.
 * Valid characters are '0' (empty space), '1' (wall), ' ' (padding/unvisited),
 * and 'N', 'S', 'E', 'W' (player start positions).
 * @param c The character to check.
 * @return 1 if valid, 0 otherwise.
 */
static int is_valid_map_char(char c)
{
    return (c == '0' || c == '1' || c == ' ' || \
            c == 'N' || c == 'S' || c == 'E' || c == 'W');
}

/**
 * @brief Checks if the map is properly closed/enclosed by walls ('1').
 *
 * Iterates through each cell of the map. If a cell is '0' (empty space)
 * or a player start position (which is treated as '0' after parsing),
 * it checks its four neighbors (up, down, left, right).
 * If any neighbor is a space (' ') or is outside the map boundaries,
 * the map is considered not closed.
 * This is a simplified check; it doesn't use flood fill but checks local adjacencies.
 *
 * @param config Pointer to the t_config struct containing the map.
 * @return 1 if the map is closed, 0 otherwise (error message is printed).
 */
static int is_map_closed(t_config *config)
{
    int y, x;
    char cell;

    for (y = 0; y < config->map_height; y++)
    {
        for (x = 0; x < config->map_width; x++)
        {
            cell = config->map_data[y][x];
            // Only check cells that are part of the navigable area ('0')
            // Player position was converted to '0' by store_player_info
            if (cell == '0')
            {
                // Check boundaries and adjacent cells for ' ' (space)
                // Spaces are effectively holes if adjacent to '0'.
                if (y == 0 || config->map_data[y-1][x] == ' ')
                    return (fprintf(stderr, "Error: Map not closed (cell [%d,%d] top border issue).\n", y, x), 0);
                if (y == config->map_height - 1 || config->map_data[y+1][x] == ' ')
                    return (fprintf(stderr, "Error: Map not closed (cell [%d,%d] bottom border issue).\n", y, x), 0);
                if (x == 0 || config->map_data[y][x-1] == ' ')
                    return (fprintf(stderr, "Error: Map not closed (cell [%d,%d] left border issue).\n", y, x), 0);
                if (x == config->map_width - 1 || config->map_data[y][x+1] == ' ')
                    return (fprintf(stderr, "Error: Map not closed (cell [%d,%d] right border issue).\n", y, x), 0);
            }
        }
    }
    return (1); // Map is considered closed by this basic check.
}

/**
 * @brief Validates the entire map structure and content.
 *
 * Checks:
 * 1. If the map is empty.
 * 2. For invalid characters within the map.
 * 3. For the presence and uniqueness of a player start position ('N', 'S', 'E', 'W').
 *    Calls store_player_info to handle this.
 * 4. If a player start position was found.
 * 5. If the map is properly closed/enclosed by walls (calls is_map_closed).
 *
 * @param config Pointer to the t_config struct containing the map to validate.
 * @return 1 if the map is valid, 0 otherwise. Errors are printed for specific failures.
 */
int validate_map(t_config *config)
{
    int y, x;

    if (config->map_height == 0 || config->map_width == 0) // Check for empty map
        return (fprintf(stderr, "Error: Map is empty or has zero width/height.\n"), 0);

    config->player_found = 0; // Reset player_found flag before iterating map.
    for (y = 0; y < config->map_height; y++)
    {
        for (x = 0; x < config->map_width; x++)
        {
            if (!is_valid_map_char(config->map_data[y][x])) // Check for invalid characters
            {
                fprintf(stderr, "Error: Invalid character '%c' in map at [%d,%d].\n", config->map_data[y][x], y, x);
                return (0);
            }
            // If character is a player start symbol, try to store it.
            if (config->map_data[y][x] == 'N' || config->map_data[y][x] == 'S' || \
                config->map_data[y][x] == 'E' || config->map_data[y][x] == 'W')
            {
                if (!store_player_info(config, y, x, config->map_data[y][x]))
                    return (0); // Error (e.g. multiple players) printed by store_player_info
            }
        }
    }
    if (!config->player_found) // Check if any player start position was found
        return (fprintf(stderr, "Error: No player start position found in map.\n"), 0);

    if (!is_map_closed(config)) // Check if the map is enclosed by walls
        return (0); // Error message printed by is_map_closed

    return (1); // Map is valid.
}

/**
 * @brief Main function to parse a .cub file.
 *
 * Reads the file line by line using get_next_line (GNL).
 * Differentiates between parsing configuration elements (textures, colors) and
 * parsing the map layout based on line content and parsing state.
 * Uses helper functions to process specific elements and validate data.
 * Manages memory for lines read and intermediate data structures (like map_lines_head).
 * Ensures comprehensive cleanup on any error.
 *
 * @param filename Path to the .cub file.
 * @param config Pointer to t_config struct to be populated.
 * @return 1 on success, 0 on failure. Errors are printed.
 */
int	parse_cub_file(const char *filename, t_config *config)
{
	int				fd;
	char			*line;          // Line read by GNL, needs freeing.
	char			*trimmed_line;  // Line after ft_strtrim, needs freeing.
	int				line_num;       // For error reporting.
	int				parsing_map_phase; // Flag: 0 for config, 1 for map.
	t_map_line_node	*map_lines_head; // Temp linked list for map lines.

	init_config(config); // Initialize config struct to defaults.
	map_lines_head = NULL;
	parsing_map_phase = 0;
	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{ // File opening error
		perror("Error opening file");
		// free_config(config); // config is fresh from init_config, nothing to free yet.
		return (0);
	}
	line_num = 0;
	line = get_next_line(fd); // Read first line.
	while (line != NULL) // Loop through each line of the file.
	{
		line_num++;
		trimmed_line = ft_strtrim(line, " \t\n\v\f\r"); // Trim standard whitespace.
		free(line); // Free original line from GNL.
		if (!trimmed_line) // Malloc error in ft_strtrim
		{
			perror("Error: ft_strtrim failed in parse_cub_file");
			close(fd); free_map_lines(map_lines_head); free_config(config); return (0);
		}
		if (ft_strlen(trimmed_line) == 0) // Line is empty or only whitespace.
		{
			free(trimmed_line);
			// If already parsing map, an empty line might be significant (part of map structure)
			// or could signify end of map if not handled carefully.
			// For this parser, empty lines in map data are preserved.
			if (parsing_map_phase)
				add_map_line(&map_lines_head, ""); // Add as an empty string to list.
			line = get_next_line(fd);
			continue; // Process next line.
		}
		// Check if this line starts the map section.
		// Map lines typically start with '1', '0', or ' ' (for padding or unclosed maps).
		if (!parsing_map_phase && (trimmed_line[0] == '1' || trimmed_line[0] == '0' || trimmed_line[0] == ' '))
		{
			if (!all_elements_parsed(config)) // All textures/colors must be defined before map.
			{
				fprintf(stderr, "Error: Map data encountered (line %d) before all config elements (textures/colors) are defined.\n", line_num);
				free(trimmed_line); free_map_lines(map_lines_head); close(fd); free_config(config); return (0);
			}
			parsing_map_phase = 1; // Switch to map parsing mode.
		}
		if (parsing_map_phase) // Currently parsing map lines.
		{
			// Basic validation of characters on the map line *before* adding to list.
			for (size_t i_map = 0; i_map < ft_strlen(trimmed_line); ++i_map) {
				if (!is_valid_map_char(trimmed_line[i_map])) {
					fprintf(stderr, "Error: Invalid character '%c' in map data (line %d): %s\n", trimmed_line[i_map], line_num, trimmed_line);
					free(trimmed_line); free_map_lines(map_lines_head); close(fd); free_config(config); return(0);
				}
			}
			add_map_line(&map_lines_head, trimmed_line);
		}
		else // Currently parsing configuration elements.
		{
			if (!process_config_line(trimmed_line, config))
			{ // process_config_line prints its own specific error.
				fprintf(stderr, "Error: Failed to parse configuration line %d: %s\n", line_num, trimmed_line);
				free(trimmed_line); free_map_lines(map_lines_head); close(fd); free_config(config); return (0);
			}
		}
		free(trimmed_line); // Free the trimmed line for this iteration.
		line = get_next_line(fd); // Read next line.
	}
	close(fd); // All lines read, close file.

	// Final checks after processing all lines.
	if (!all_elements_parsed(config)) // Ensure all required config elements were found.
	{
		fprintf(stderr, "Error: Missing one or more required configuration elements (textures/colors) at end of file.\n");
		free_map_lines(map_lines_head); free_config(config); return (0);
	}
	if (!map_lines_head && parsing_map_phase) // Map parsing started but no map lines were actually added.
	{
		fprintf(stderr, "Error: Map definition is empty or missing after map section started.\n");
		free_map_lines(map_lines_head); free_config(config); return (0);
	}
    if (!map_lines_head && !parsing_map_phase) // No map lines at all, and never started map phase (e.g. file only has config)
    {
        fprintf(stderr, "Error: Map data is entirely missing from the file.\n");
        free_map_lines(map_lines_head); free_config(config); return (0);
    }
    // Convert linked list of map lines to 2D array in config.
    if (map_lines_head && !convert_map_lines_to_array(map_lines_head, config))
    { // Memory allocation error during conversion.
        free_map_lines(map_lines_head); free_config(config); return (0);
    }
	free_map_lines(map_lines_head); // Free the temporary linked list.

    // Validate the parsed map.
	if (config->map_data && !validate_map(config))
	{ // validate_map prints specific errors.
		free_config(config); return (0); // Ensure config is freed on validation failure.
	}
    // This case should be caught by !map_lines_head check earlier, but as a safeguard:
    if (!config->map_data && all_elements_parsed(config))
    {
        fprintf(stderr, "Error: Map data is missing, though all configurations were parsed.\n");
        free_config(config); return (0);
    }
	return (1); // Parsing successful.
}
