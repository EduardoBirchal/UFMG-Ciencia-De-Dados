#include <stdio.h>
#include <stdlib.h>

float praFarenheit(float tempInicial) {
	return (1.8 * tempInicial) + 32; 
}

int main(int argc, char **argv) {
	float tempInicial;
	
	scanf("%f", &tempInicial);
	printf("%.0f", praFarenheit(tempInicial));
	
	return 0;
}
