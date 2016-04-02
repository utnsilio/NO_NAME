#include <stdio.h>			//por el printf
#include <stdlib.h>			//por el malloc
#include <string.h>			//por el memset
#include <netdb.h>			//Tine el tipo addrinfo
#include <commons/string.h>	//para concatenar el nick

#define IP "10.16.19.51"
#define PUERTO "12345"
#define PACKAGESIZE 1024	// Define cual va a ser el size maximo del paquete a enviar

int main (int argc , char ** argv ){

	char nick[10];
	printf("Ingrese su nick: ");
	//fgets(nick, 10, stdin);
	scanf("%s",nick);
	setbuf(stdin,NULL);

	/*
	 * Lo primero que tengo que hacer es preparar la direccion IP,
	 * A diferencia de nuestro server el cliente no asigna localhost
	 * Necesitamos seguridad no velocidad
	 * por lo que utilizare el TCP y no el UDP
	 */
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// No importa si uso IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	/*
	 * Cuidado! cuando utilice a continuacion la funcion getaddrinfo me devuelve un 0
	 * si funciona todo OK, pero puede devolver un numero, correspondiente a un tipo
	 * de error, por ejemplo:
	 * EAI_NODATA -> Significa que no tenemos una direccion de red asignada.
	 *
	 * Notar que a diferencia que mi servidor no le paso NULL en el primer parametro
	 * le paso IP, direccion ip de mi servidor
	 */

	int rc;
	rc = getaddrinfo(IP, PUERTO, &hints, &serverInfo);;
	if(rc)
	{
		fprintf(stderr, "%s: %s \n", argv[0], gai_strerror(rc));
		printf("error \n");
		return EXIT_FAILURE;
	}

	/*
	 * Otra vez cuidado! ahora al utilizar socket tambien puede devolverme un error,
	 * si me da un menor a 0. Para que mi cliente funcione necesitamos que nuestro
	 * sistema operativo fabrique un archivo de tipo socket y nos retorne el identificador.
	 * ese identificador es un entero. cuando creamos el socket usamos los datos de red
	 * que obtuvimos anteriormente.
	 */

	int serverSocket;
	serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	if(serverSocket < 0)
	{
		perror("socket");
		return EXIT_FAILURE;
	}
	printf("socket %d \n",serverSocket);

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

	int cl;
	cl = connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	if(cl < 0)
	{
		perror("accept");
		return EXIT_FAILURE;
	}
	printf("accept %d \n",cl);
	freeaddrinfo(serverInfo);	// No lo necesitamos mas

	//Envio mensajes
	int enviar = 1;
	char message[PACKAGESIZE] = ""; //Mensaje completo
	char mensaje[PACKAGESIZE-12];
	char dospuntos[] = ": ";

	printf("Conectado al servidor chambi-house. Escriba 'exit' para salir\n");

	while(enviar){
		message[0] = 0;
		fgets(mensaje, PACKAGESIZE-12, stdin);			// Lee una linea en el stdin (lo que escribimos en la consola) hasta encontrar un \n (y lo incluye) o llegar a PACKAGESIZE.
		if (!strcmp(mensaje,"exit\n")) enviar = 0;			// Chequeo que el usuario no quiera salir
		strcat(message,nick);
		strcat(message,dospuntos);
		strcat(message,mensaje);
		if (enviar) send(serverSocket, message, strlen(message) + 1, 0); 	// Solo envio si el usuario no quiere salir.
	}

	close(serverSocket);

	return 0;
}
