#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define PI 3.14159

int main() {
    int fd = open("ratio.txt", O_WRONLY | O_CREAT, 0644);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    char buffer[128];
    sprintf(buffer, "Diagonal, AspectRatio, Ratio\n");
    write(fd, buffer, strlen(buffer));

    for (int Diagonal = 14; Diagonal <= 32; Diagonal++) {
        for (int i = 0; i <= 9; i++) {
            double rad = i * PI / 20;
            double Height = Diagonal * sin(rad);
            double Width = Diagonal * cos(rad);
            double Area = Height * Width;
            double Ratio = Area / Diagonal;
            double AspectRatio = Height / Width;

            sprintf(buffer, "%d, %.10f, %.10f\n", Diagonal, AspectRatio, Ratio);
            write(fd, buffer, strlen(buffer));
        }
    }

    close(fd);
    return 0;
}
