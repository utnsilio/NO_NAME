//HEADER DEL SUMADOR
#ifndef SUMADOR_H_
#define SUMADOR_H_

typedef struct sumador Sumador;

struct sumador{
	//PROPIEDADES
	int numA;
	int numB;

	//METODOS
	int (* sumar)(Sumador * this);
};

//METODOS
int sumador_sumar(Sumador * this);

//CONSTRUCTOR
Sumador * new_sumador(int numA, int numB);

//DESTRUCTOR
void destroy_sumador(Sumador * this);

#endif /* SUMADOR_H_ */
