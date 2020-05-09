#include <stdio.h>
#include <stdlib.h>
#include "lib.h"
#include <pthread.h>
#include <sys/socket.h>
// commons/collections/list.h


int main(void) {

char* config;
t_log* logger;
char* puerto_broker;
char* ip_broker;
int socket_client;

//Bloque memoria -> para los mensajes




config = leer_config("/home/utnso/TP OPERATIVOS/tp-2020-1c-CheckPoint/Broker"); //Esto solo va a funcionar en la compu de juan
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
	}

void esperar_conexion(int socket_cliente){
	int cliente = aceptar_cliente(socket_cliente);
	log_info(logger,"Se conecto un cliente con el socket numero %d", cliente);
	pthread_t hilo_nuevo_cliente;
		if(pthread_create(&hilo_nuevo_cliente,NULL,(void*)atender_cliente,(void*)cliente)!=0){
			log_error(logger,"Error al crear el hilo de journal");
			}
		pthread_detach(hilo_nuevo_cliente);
		//close(cliente);
	}


int aceptar_cliente(int servidor){
	struct sockaddr_in direccion_cliente;
	uint32_t tamanio_direccion = sizeof(struct sockaddr_in);
	uint32_t cliente;
	cliente = accept(servidor,(void*) &direccion_cliente, &tamanio_direccion);
	return cliente;
}


void atender_cliente(int socket_cliente){

	t_paquete* paquete = malloc(sizeof(t_paquete));
	recv(socket_cliente,&(paquete->codigo_operacion),sizeof(uint8_t),0);

	recv(socket_cliente,&(paquete->buffer->size),sizeof(uint32_t),0);
	recv(socket_cliente,&(paquete->buffer),sizeof(paquete->buffer->size),0);

	switch(paquete->codigo_operacion){
		case 0: New_Pokemon* mensaje_new = descerializar_new_pokemon(paquete->buffer);
				//encolar(punterotanto,mensaje_new)
				free(mensaje_new);
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


// ----------------------------- Descerializadores (esa palabra si quiera existe?) ---------------------------------------

// No hay una forma de usar un unico descerializar?
// ASI NO SE DESCERIALIZA FLACO, MEMCPY

void* descerializar_new_pokemon(t_buffer buffer){

	New_Pokemon new_pokemon = malloc(sizeof(New_Pokemon));

	void* stream = buffer->stream;

	memcpy(&(new_pokemon->nombre->size_nombre),stream,sizeof(uint32_t));
	stream += sizeof(uint32_t);
	new_pokemon->nombre->nombre = malloc(new_pokemon->nombre->size_nombre);
	memcpy(&(new_pokemon->nombre->nombre),stream,new_pokemon->nombre->size_nombre);
	stream += new_pokemon->nombre->size_nombre;
	memcpy(&(new_pokemon->posicion->posicion_X),stream,sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&(new_pokemon->posicion->posicion_Y),stream,sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&(new_pokemon->cantidad),stream,sizeof(uint32_t));
	stream += sizeof(uint32_t);

	return new_pokemon;

}



return EXIT_SUCCESS;




}



