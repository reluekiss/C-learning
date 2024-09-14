#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GRID_SIZE 20
#define GRID_CENTER (GRID_SIZE / 2)

typedef struct {
	float x;
	float y;
} vec_2d;

void clear_grid(char grid[GRID_SIZE][GRID_SIZE])
{
	for (int row = 0; row < GRID_SIZE; row++) {
		memset(grid[row], ' ', GRID_SIZE);
	}
}

void render_grid(char grid[GRID_SIZE][GRID_SIZE])
{
	for (int row = 0; row < GRID_SIZE; row++) {
		printf("%.*s\n", GRID_SIZE, grid[row]);
	}
}

void plot_point(char grid[GRID_SIZE][GRID_SIZE], float x, float y,
		char point_char)
{
	int grid_x = (int)(x + GRID_CENTER + 0.5f);
	int grid_y = (int)(-y + GRID_CENTER + 0.5f);
	if (grid_x >= 0 && grid_x < GRID_SIZE && grid_y >= 0 &&
	    grid_y < GRID_SIZE) {
		grid[grid_y][grid_x] = point_char;
	}
}

void draw_line(char grid[GRID_SIZE][GRID_SIZE], vec_2d start, vec_2d end,
	       char line_char)
{
	int start_x = (int)(start.x + GRID_CENTER + 0.5f);
	int start_y = (int)(-start.y + GRID_CENTER + 0.5f);
	int end_x   = (int)(end.x + GRID_CENTER + 0.5f);
	int end_y   = (int)(-end.y + GRID_CENTER + 0.5f);
	int delta_x = abs(end_x - start_x), step_x = start_x < end_x ? 1 : -1;
	int delta_y = -abs(end_y - start_y), step_y = start_y < end_y ? 1 : -1;
	int error = delta_x + delta_y, error_double;
	while (1) {
		if (start_x >= 0 && start_x < GRID_SIZE && start_y >= 0 &&
		    start_y < GRID_SIZE)
			grid[start_y][start_x] = line_char;
		if (start_x == end_x && start_y == end_y)
			break;
		error_double = 2 * error;
		if (error_double >= delta_y) {
			error += delta_y;
			start_x += step_x;
		}
		if (error_double <= delta_x) {
			error += delta_x;
			start_y += step_y;
		}
	}
}

vec_2d rotate_point(vec_2d point, float angle_rad)
{
	float  cos_angle = cosf(angle_rad);
	float  sin_angle = sinf(angle_rad);
	vec_2d rotated_point;
	rotated_point.x = point.x * cos_angle - point.y * sin_angle;
	rotated_point.y = point.x * sin_angle + point.y * cos_angle;
	return rotated_point;
}

void test()
{
	vec_2d arrow_points[4] = {
		{ 0.0f, 0.0f }, // Tail of the arrow
		{ 5.0f, 0.0f }, // Head point of the arrow
		{ 4.0f, 1.0f }, // Upper side of the arrowhead
		{ 4.0f, -1.0f } // Lower side of the arrowhead
	};

	char grid_before[GRID_SIZE][GRID_SIZE];
	char grid_after[GRID_SIZE][GRID_SIZE];
	clear_grid(grid_before);
	clear_grid(grid_after);

	draw_line(grid_before, arrow_points[0], arrow_points[1], '*'); // Arrow
								       // shaft
	draw_line(grid_before, arrow_points[1], arrow_points[2],
		  '*'); // Upper arrowhead
	draw_line(grid_before, arrow_points[1], arrow_points[3],
		  '*'); // Lower arrowhead

	float rotation_angle = M_PI / 4.0f; // 45 degrees in radians

	vec_2d rotated_arrow_points[4];
	for (int i = 0; i < 4; i++) {
		rotated_arrow_points[i] =
			rotate_point(arrow_points[i], rotation_angle);
	}

	draw_line(grid_after, rotated_arrow_points[0], rotated_arrow_points[1],
		  '*');
	draw_line(grid_after, rotated_arrow_points[1], rotated_arrow_points[2],
		  '*');
	draw_line(grid_after, rotated_arrow_points[1], rotated_arrow_points[3],
		  '*');

	printf("Arrow Before Rotation:\n");
	render_grid(grid_before);
	printf("\nArrow After Rotation (45 degrees):\n");
	render_grid(grid_after);
}

int main()
{
	test();
	return 0;
}
