#include <stdio.h>
#include <stdlib.h>
#include "lib.h"
#include <pthread.h>
#include <sys/socket.h>


int main(void) {

char* config;
t_log* logger;
char* puerto_broker;
char* ip_broker;
int socket_client;


config = leer_config("/home/utnso/TP OPERATIVOS/tp-2020-1c-CheckPoint/Broker");
//config_set_value(config,"TAMANO_MEMORIA",aca va el valor del tamaño [Numerico]);
//config_set_value(config,"TAMANO_MINIMO_PARTICION",aca va el valor del tamaño minimo de particion[Numerico]);
//config_set_value(config,"ALGORITMO_MEMORIA",aca va el valor del algoritmo de memoria[String]);
//config_set_value(config,"ALGORITMO_REEMPLAZO",aca va el valor del algoritmo de reemplazo[String]);
//config_set_value(config,"ALGORITMO_PARTICION_LIBRE",aca va el valor del algoritmo de particion libre[String]);
config_set_value(config,"IP_BROKER","127.0.0.1");
config_set_value(config,"PUERTO_BROKER",6009);
//config_set_value(config,"FRECUENCIA_COMPACTACION",aca va el valor de la frecuencia de compactacion[Numerico]);
config_set_value(config,"LOG_FILE","/home/utnso/TP OPERATIVOS/tp-2020-1c-CheckPoint/Broker");



puerto_broker = config_get_string_value(config,"PUERTO_BROKER");
ip_broker = config_get_string_value(config,"IP_BROKER");

logger = iniciar_logger_de_nivel_minimo(LOG_LEVEL_ERROR,"/home/utnso/TP OPERATIVOS/tp-2020-1c-CheckPoint/Broker");


socket_client = crear_conexion(ip_broker,puerto_broker);

	while(1){
		esperar_conexion(socket_client);
		int cliente = aceptar_cliente(socket_client);
		atender_cliente(cliente);
	}

void esperar_conexion(int socket_client){
	int cliente = aceptar_cliente(socket_client);
	log_info(logger,"Se conecto un cliente con el socket numero %d", cliente);
// HASTA ACÁ ESTA ADAPTADO A NUESTRO CODIGO MAS O MENOS
	//falta la implementacion de la funcion que va a hacer el hilo al conectarse sac-cli

	pthread_t hilo_nuevo_cliente;
	if(pthread_create(&hilo_nuevo_cliente,NULL,(void*)atender_cliente,(void*)cliente)!=0){
		log_error(logger,"Error al crear el hilo de journal");
		}

	pthread_detach(hilo_nuevo_cliente);
	// close(cliente);
}







int aceptar_cliente(int servidor){
	struct sockaddr_in direccion_cliente;
	uint32_t tamanio_direccion = sizeof(struct sockaddr_in);
	uint32_t cliente;
	cliente = accept(servidor,(void*) &direccion_cliente, &tamanio_direccion);
	return cliente;
}








void recbir_mensaje(int socket_cliente){

	op_code codigo_mensaje;
	recv(socket_cliente,&codigo_mensaje,sizeof(codigo_mensaje),0);

	switch(codigo_mensaje){
		case 0: New_Pokemon mensaje_new = descerializar_new_pokemon(socket_cliente);
				//encolar(punterotanto,mensaje_new)
				break;

		case 1: Localized_Pokemon mensaje_localized = descerializar_localized_pokemon(socket_cliente);
				//encolar(punterotanto,mensaje_localized)
			    break;

		case 2: Get_Pokemon mensaje_get = descerializar_get_pokemon(socket_cliente);
				//encolar(punterotanto,mensaje_get)
				break;

		case 3: Appeared_Pokemon mensaje_appeared = descerializar_appeared_pokemon(socket_cliente);
				//encolar(punterotanto,mensaje_appeared)
				break;

		case 4: Catch_Pokemon mensaje_catch = descerializar_catch_pokemon(socket_cliente);
				//encolar(punterotanto,mensaje_catch)
				break;

		case 5: Caught_Pokemon mensaje_caught = descerializar_caught_pokemon(socket_cliente);
				//encolar(punterotanto,mensaje_caught)
				break;
	}
}

/*void* descerializar_new_pokemon(int socket_cliente){
	int new_pokemon_size;
	recv(socket_cliente, &new_pokemon_size, sizeof(new_pokemon_size), 0);
	int size_nombre;
	recv(socket_cliente, &size_nombre, sizeof(size_nombre), 0);
	char *nombre = malloc(size_nombre);
	recv(socket_cliente, nombre, size_nombre, 0);



}*/

return EXIT_SUCCESS;




}



