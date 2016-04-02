#include <stdio.h>			//por el printf
#include <stdlib.h>			//por el malloc
#include <string.h>			//por el memset
#include <netdb.h>			//Tine el tipo addrinfo
#include <pthread.h>		//por los hilos

#define BACKLOG 5			// Define cuantas conexiones vamos a mantener pendientes al mismo tiempo
#define PUERTO "12345"
#define PACKAGESIZE 1024	// Define cual va a ser el size maximo del paquete a enviar

//Esta es la funcion que se ejecuta para cada cliente
void *atender_cliente(void *arg)
{
	int cl = (long)arg;
	char package[PACKAGESIZE];
	int status = 1;		// Estructura que manjea el status de los recieve.

	printf("Cliente conectado. Esperando mensajes:\n");

	while (status != 0){
		status = recv(cl, (void*) package, PACKAGESIZE, 0);
		if (status != 0) printf("%s", package);
	}

	close(cl);
	return NULL;
}

int main (int argc , char ** argv ){

	/*
	 * Lo primero que tengo que hacer es preparar la direccion IP,
	 * Como es un servidor, no me interesa desde donde vienen las conexiones.
	 * Me alcanzara con definir localhost. Necesitamos seguridad no velocidad
	 * por lo que utilizare el TCP y no el UDP
	 */
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// No importa si uso IPv4 o IPv6
	hints.ai_flags = AI_PASSIVE;		// Asigna el address del localhost: 127.0.0.1
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	/*
	 * Cuidado! cuando utilice a continuacion la funcion getaddrinfo me devuelve un 0
	 * si funciona todo OK, pero puede devolver un numero, correspondiente a un tipo
	 * de error, por ejemplo:
	 * EAI_NODATA -> Significa que no tenemos una direccion de red asignada.
	 */

	int rc;
	rc = getaddrinfo(NULL, PUERTO, &hints, &serverInfo);
	if(rc)
	{
		fprintf(stderr, "%s: %s \n", argv[0], gai_strerror(rc));
		printf("error \n");
		return EXIT_FAILURE;
	}

	/*
	 * Otra vez cuidado! ahora al utilizar socket tambien puede devolverme un error,
	 * si me da un menor a 0. Para que mi servidor funcione necesitamos que nuestro
	 * sistema operativo fabrique un archivo de tipo socket y nos retorne el identificador.
	 * ese identificador es un entero. cuando creamos el socket usamos los datos de red
	 * que obtuvimos anteriormente.
	 */

	int listenningSocket;
	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	if(listenningSocket < 0)
	{
		perror("socket");
		return EXIT_FAILURE;
	}
	printf("socket %d \n",listenningSocket);

	/*
	 * Debo configurar el socket para cuando lo active mas adelante escuche en una ip y puerto
	 * Cuidado, tambien puede darme un error, si le mando fruta en los parametros.
	 */

	rc = bind(listenningSocket,serverInfo->ai_addr, serverInfo->ai_addrlen);
	if(rc < 0)
	{
		perror("connect");
		return EXIT_FAILURE;
	}
	printf("bind %d \n",rc);
	freeaddrinfo(serverInfo); // Ya no lo vamos a necesitar

	/*
	 * Ahora escuchemos por el socket, cuidado que la funcion listen me bloquea el proceso,
	 * ademas puede darme un error, el BACKLOG es una constante que defini yo, que indica
	 * la cantidad de Clientes que se puede conectar, si se lleno rompe.
	 */

	rc = listen(listenningSocket, BACKLOG);		// IMPORTANTE: listen() es una syscall BLOQUEANTE.
	if(rc < 0)
		{
			perror("listen");
			return EXIT_FAILURE;
		}
	printf("listen %d \n",rc);

	/*
	 * con la funcion accept voy a aceptar una peticion de un Cliente. Aqui voy a agregar
	 * algo como para saber la ip del Cliente. Ademas.. cuando venga un cliente.. voy a
	 * crear un hilo.. y le mando el socket de una manera rara... para que mi proceso
	 * principal sigua en el while escuchando nuevos clientes... y el hilo nuevo se encargue
	 * del cliente aceptado. Se van a ir creando hilos por cada cliente. Como los hilos
	 * no compartes variables es una manera facil. podriamos crear nuevos procesos para cada
	 * cliente, es otra manera con sus pro y con contra.
	 *
	 * la funcion accept, tambien puede darme un error, si le doy un socket basura por
	 * ejemplo, el accept no solo acepta la conexion, tambien obtiene datos del cliente
	 * que lo vamos a colocar en un sockaddr y en un socklen_t..
	 *
	 * Luego usando la funcion getnameinfo puedo obtener un string de la ip del cliente
	 *
	 */

	while(1)
	{
		int cl;
		struct sockaddr addr;
		socklen_t addrlen = sizeof(addr);
		pthread_t thread;

		cl = accept(listenningSocket, &addr, &addrlen);
		if(cl < 0)
		{
			perror("accept");
			return EXIT_FAILURE;
		}
		printf("accept %d \n",cl);

		char host[NI_MAXHOST];
		getnameinfo(&addr, addrlen, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
		printf("Cliente conectado desde %s \n",host);

		pthread_create(&thread, NULL, atender_cliente, (void*)(long)cl);
	}

	close(listenningSocket);

	printf("correcto \n");

	return 0;
}
