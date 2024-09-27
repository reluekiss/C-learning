#include <stdio.h>
#define GRID_SIZE 42
#define GRID_CENTER (GRID_SIZE / 2)

void render_grid(char grid[GRID_SIZE][GRID_SIZE])
{
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            char L = ' ';
            char R = ' ';
            if(grid[y][x] == 1) {
                L = '[';
                R = ']';
            }
            printf("%c%c", L, R);
        }
        printf("\n");
    }
}

void circleBres(char grid[GRID_SIZE][GRID_SIZE], int cx, int cy, int r)
{
    int r2 = r + r;
    int x = r, y = 0;
	int dy = -2, dx = r2 + r2 - 4;
    int d = r2 - 1;
	while (y <= x) {
        grid[cy - y][cx - x] = 1;
        grid[cy - y][cx + x] = 1;
        grid[cy + y][cx - x] = 1;
        grid[cy + y][cx + x] = 1;
        grid[cy - x][cx - y] = 1;
        grid[cy - x][cx + y] = 1;
        grid[cy + x][cx - y] = 1;
        grid[cy + x][cx + y] = 1;
        
        d += dy;
        dy -= 4;
        y++;

		int mask = (d >> 31);
		d += dx & mask;
	    dx -= 4 & mask;
        x += mask;
	}
}

int main()
{
	int  xc = GRID_CENTER, yc = GRID_CENTER, r = 20;
	char grid[GRID_SIZE][GRID_SIZE];

	circleBres(grid, xc, yc, r);
	render_grid(grid);

	return 0;
}
