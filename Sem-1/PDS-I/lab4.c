#include <stdio.h>
#include <stdlib.h>

#define DIAS_EM_ANO 365
#define HORAS_EM_DIA 24

long long int horasEm(long long int num) {
	return num * HORAS_EM_DIA * DIAS_EM_ANO;
}

int main(int argc, char **argv) {
	long long int numAnos;
	
	scanf("%lld", &numAnos);
	printf("%lld", horasEm(numAnos));
	
	return 0;
}
