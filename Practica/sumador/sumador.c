#include <stdlib.h>
#include "sumador.h"

//CONSTRUCTOR
Sumador * new_sumador(int numA, int numB){
	Sumador * s;
	s = malloc(sizeof(struct sumador));
	s->numA = numA;
	s->numB = numB;

	s->sumar = sumador_sumar;
	return s;
}

//DESTRUCTOR
void destroy_sumador(Sumador * this){
	free(this);
}

//IMPLEMENTAR LOS METODOS
int sumador_sumar(Sumador * this){
	return this->numA + this->numB;
}
