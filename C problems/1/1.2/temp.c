#include <stdio.h>

/* print Farenheit-Celcius table
	for fahr = 0, 20, ..., 300 */

void fahr2celc()
{
	float fahr, celcius;
	float lower, upper, step;

	lower = 0;
	upper = 300;
	step = 20;

	fahr = lower;
	printf("Fahr\tCelcius\n");
	while (fahr <= upper) {
		celcius = (5.0/9.0) * (fahr - 32);
		printf("%3.0f\t%6.1f\n", fahr, celcius);
		fahr = fahr + step;
	}
}

void celc2fahr()
{
	float fahr, celcius;
	float lower, upper, step;

	lower = 0;
	upper = 300;
	step = 20;

	celcius = lower;
	printf("Celcius\tFahr\n");
	while (celcius <= upper) {
		fahr = (9.0/5.0) * celcius + 32;
		printf("%3.0f\t%6.1f\n", celcius, fahr);
		celcius = celcius + step;
	}
}

int main()
{
	celc2fahr();
	fahr2celc();
}
