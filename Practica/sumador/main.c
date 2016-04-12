#include "sumador.c"
#include <stdio.h>

int main(){
	Sumador* suma = new_sumador(10,15);
	Sumador* suma2 = new_sumador(5,20);

	printf("la suma de %d \n", suma->sumar(suma));
	printf("la suma de %d \n", suma2->sumar(suma2));


	destroy_sumador(suma);

	printf("la suma de %d \n", suma->sumar(suma));



	return 0;
}
