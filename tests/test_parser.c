#include "cub3d.h" // Should provide t_config and parser function prototypes
#include <stdio.h>
#include <assert.h>
#include <string.h> // For strcmp
#include <stdlib.h> // For strdup, free

// Utility function to reset and free config
void reset_and_free_config(t_config *config) {
    free_config(config); // Frees internal allocations
    init_config(config); // Re-initializes to default state
}

// Test for a valid simple map file
void test_parse_valid_simple_map(void) {
    printf("Test: Valid Simple Map File... ");
    t_config config;
    init_config(&config);

    int result = parse_cub_file("maps/test_files/valid_simple.cub", &config);
    assert(result == 1); // Should parse successfully

    // Add some basic assertions for valid_simple.cub if its content is known/stable
    // For example:
    assert(strcmp(config.north_texture_path, "./textures/north_simple.xpm") == 0);
    assert(config.floor_color_r == 225);
    assert(config.floor_color_g == 225);
    assert(config.floor_color_b == 225);
    assert(config.map_width == 8); // Based on typical simple map
    assert(config.map_height == 4); // Based on typical simple map
    assert(config.player_orientation == 'N'); // Example

    reset_and_free_config(&config);
    printf("PASSED\n");
}

// Test for a valid complex map file
void test_parse_valid_complex_map(void) {
    printf("Test: Valid Complex Map File... ");
    t_config config;
    init_config(&config);

    int result = parse_cub_file("maps/test_files/valid_complex.cub", &config);
    assert(result == 1); // Should parse successfully

    assert(strcmp(config.north_texture_path, "./textures/north.xpm") == 0);
    assert(strcmp(config.south_texture_path, "./textures/south.xpm") == 0);
    assert(strcmp(config.west_texture_path, "./textures/west.xpm") == 0);
    assert(strcmp(config.east_texture_path, "./textures/east.xpm") == 0);

    assert(config.floor_color_r == 0);
    assert(config.floor_color_g == 0);
    assert(config.floor_color_b == 0);

    assert(config.ceiling_color_r == 100);
    assert(config.ceiling_color_g == 100);
    assert(config.ceiling_color_b == 100);

    assert(config.map_width == 10);
    assert(config.map_height == 5);

    // Player at (2,2) in a 0-indexed map array, orientation 'N'
    // Map:
    // 1111111111
    // 1000000001
    // 10N0000001  <-- Player at map[2][2]
    // 1000000001
    // 1111111111
    // Player coordinates are cell center: (col + 0.5, row + 0.5)
    assert(config.player_start_x == 2.5); // column 2 + 0.5
    assert(config.player_start_y == 2.5); // row 2 + 0.5
    assert(config.player_orientation == 'N');

    reset_and_free_config(&config);
    printf("PASSED\n");
}

// Test for map with invalid characters
void test_parse_invalid_char_map(void) {
    printf("Test: Invalid Character in Map... ");
    t_config config;
    init_config(&config);

    int result = parse_cub_file("maps/test_files/invalid_char.cub", &config);
    assert(result == 0); // Should fail parsing

    reset_and_free_config(&config);
    printf("PASSED\n");
}

// Test for map that is not closed/open
void test_parse_invalid_open_map(void) {
    printf("Test: Invalid Open Map... ");
    t_config config;
    init_config(&config);

    int result = parse_cub_file("maps/test_files/invalid_open.cub", &config);
    assert(result == 0); // Should fail parsing

    reset_and_free_config(&config);
    printf("PASSED\n");
}

// Test for map with no player start position
void test_parse_invalid_no_player_map(void) {
    printf("Test: Invalid No Player Map... ");
    t_config config;
    init_config(&config);

    int result = parse_cub_file("maps/test_files/invalid_no_player.cub", &config);
    assert(result == 0); // Should fail parsing

    reset_and_free_config(&config);
    printf("PASSED\n");
}

// Test for map with multiple player start positions
void test_parse_invalid_multiple_players_map(void) {
    printf("Test: Invalid Multiple Players Map... ");
    t_config config;
    init_config(&config);

    int result = parse_cub_file("maps/test_files/invalid_multiple_players.cub", &config);
    assert(result == 0); // Should fail parsing

    reset_and_free_config(&config);
    printf("PASSED\n");
}

// Test for configuration with a missing texture path
void test_parse_invalid_missing_texture_map(void) {
    printf("Test: Invalid Missing Texture Map... ");
    t_config config;
    init_config(&config);

    int result = parse_cub_file("maps/test_files/invalid_missing_texture.cub", &config);
    assert(result == 0); // Should fail parsing

    reset_and_free_config(&config);
    printf("PASSED\n");
}

// Test for configuration with an invalid color format/value
void test_parse_invalid_bad_color_map(void) {
    printf("Test: Invalid Bad Color Map... ");
    t_config config;
    init_config(&config);

    int result = parse_cub_file("maps/test_files/invalid_bad_color.cub", &config);
    assert(result == 0); // Should fail parsing

    reset_and_free_config(&config);
    printf("PASSED\n");
}


int main() {
    printf("Starting cub3D parser tests...\n\n");

    // Call the original test (now updated to actually parse a file)
    test_parse_valid_simple_map();

    // Call the new test functions
    test_parse_valid_complex_map();
    test_parse_invalid_char_map();
    test_parse_invalid_open_map();
    test_parse_invalid_no_player_map();
    test_parse_invalid_multiple_players_map();
    test_parse_invalid_missing_texture_map();
    test_parse_invalid_bad_color_map();

    printf("\nAll parser tests completed based on assertions.\n");
    return 0;
}
