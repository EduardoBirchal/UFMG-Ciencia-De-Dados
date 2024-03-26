#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14
#define NUM_PEDACOS 8

double getArea(int diametro) {
	double r = ((double) diametro) / 2.0;
	double r2 = pow(r, 2);
	
	return PI * r2;
}

int main(int argc, char **argv) {
	int diametro;
	
	scanf("%d", &diametro);
	double area = getArea(diametro);
	printf("%lf\n%lf\n", area, area/NUM_PEDACOS);
	
	return 0;
}
