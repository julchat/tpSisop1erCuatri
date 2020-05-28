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

administrador_mensajes* cola_mensajes = NULL;
int id_unico = 0; // ojo con esto, ¡¡¡¡capaz!!! hay que sincronizarlo, lo vamos a usar como un contador

//Bloque memoria -> para los mensajes (malloquear lo que dice el config que nos dan)



/* HAY QUE LEVANTAR UNA CONFIG, NO SETEARLA!
config = leer_config("/home/utnso/TP OPERATIVOS/tp-2020-1c-CheckPoint/Broker"); //Esto solo va a funcionar en la compu de juan
config_set_value(config,"TAMANO_MEMORIA",aca va el valor del tamaño [Numerico]);
config_set_value(config,"TAMANO_MINIMO_PARTICION",aca va el valor del tamaño minimo de particion[Numerico]);
config_set_value(config,"ALGORITMO_MEMORIA",aca va el valor del algoritmo de memoria[String]);
config_set_value(config,"ALGORITMO_REEMPLAZO",aca va el valor del algoritmo de reemplazo[String]);
config_set_value(config,"ALGORITMO_PARTICION_LIBRE",aca va el valor del algoritmo de particion libre[String]);
config_set_value(config,"IP_BROKER","127.0.0.1");
config_set_value(config,"PUERTO_BROKER",6009);
config_set_value(config,"FRECUENCIA_COMPACTACION",aca va el valor de la frecuencia de compactacion[Numerico]);
config_set_value(config,"LOG_FILE","/home/utnso/TP OPERATIVOS/tp-2020-1c-CheckPoint/Broker");



puerto_broker = config_get_string_value(config,"PUERTO_BROKER");
ip_broker = config_get_string_value(config,"IP_BROKER");

logger = iniciar_logger_de_nivel_minimo(LOG_LEVEL_ERROR,"/home/utnso/TP OPERATIVOS/tp-2020-1c-CheckPoint/Broker");
*/

// -------------------------------------------- Punteros a las listas ----------------------------------------------



//socket_client = crear_conexion(ip_broker,puerto_broker); esto tambien se levanta desde la config creo

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
		case 2: New_Pokemon* mensaje_new = deserializar_new_pokemon(paquete->buffer);
				enlistar_mensaje(cola_mensajes,mensaje_new);
				free(mensaje_new);
				break;

		case 3: Localized_Pokemon* mensaje_localized = deserializar_localized_pokemon(paquete->buffer); //Falta hacer esta funcion con el tema de los vectores :C
				enlistar_mensaje(cola_mensajes,mensaje_localized);
				free(mensaje_localized);
			    break;

		case 4: Get_Pokemon* mensaje_get = deserializar_get_pokemon(paquete->buffer);
				enlistar_mensaje(cola_mensajes,mensaje_get);
				free(mensaje_get);
				break;

		case 5: Appeared_Pokemon* mensaje_appeared = deserializar_appeared_pokemon(paquete->buffer);
				enlistar_mensaje(cola_mensajes,mensaje_appeared);
				free(mensaje_appeared);
				break;

		case 6: Catch_Pokemon* mensaje_catch = deserializar_catch_pokemon(paquete->buffer);
				enlistar_mensaje(cola_mensajes,mensaje_catch);
				free(mensaje_catch);
				break;

		case 7: Caught_Pokemon* mensaje_caught = deserializar_caught_pokemon(paquete->buffer);
				enlistar_mensaje(cola_mensajes,mensaje_caught);
				free(mensaje_caught);
				break;
	}
}


// ----------------------------- Deserializadores (esa palabra si quiera existe?) ---------------------------------------

// Vamos a tener que luego pasar estas funciones a la libreria para que las usen los otros modulos

void* deserializar_new_pokemon(t_buffer buffer){

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

 void* deserializar_get_pokemon(t_buffer buffer){

	 Get_Pokemon get_pokemon = malloc(sizeof(Get_Pokemon));

	 void* stream = buffer->stream;

	 memcpy(&(get_pokemon->nombre->size_nombre),stream,sizeof(uint32_t));
	 stream += sizeof(uint32_t);
	 get_pokemon->nombre->nombre = malloc(get_pokemon->nombre->size_nombre);
	 memcpy(&(get_pokemon->nombre->nombre),stream,get_pokemon->nombre->size_nombre);
	 stream += get_pokemon->nombre->size_nombre;

	 return get_pokemon;
 }

 void* deserializar_appeared_pokemon(t_buffer buffer){

	 Appeared_Pokemon appeared_pokemon = malloc(sizeof(Appeared_Pokemon));

	 void* stream = buffer->stream;

	 memcpy(&(appeared_pokemon->nombre->size_nombre));
	 stream += sizeof(uint32_t);
	 appeared_pokemon->nombre->nombre = malloc(appeared_pokemon->nombre->size_nombre);
	 memcpy(&(appeared_pokemon->nombre->nombre),stream,appeared_pokemon->nombre->size_nombre);
	 stream += appeared_pokemon->nombre->size_nombre;
	 memcpy(&(appeared_pokemon->posicion->posicion_X),stream,sizeof(uint32_t));
	 stream += sizeof(uint32_t);
	 memcpy(&(appeared_pokemon->posicion->posicion_Y),stream,sizeof(uint32_t));
	 stream += sizeof(uint32_t);

	 return appeared_pokemon;
 }

  void* deserializar_catch_pokemon(t_buffer buffer){

	 Catch_Pokemon catch_pokemon = malloc(sizeof(Catch_Pokemon));

	 void* stream = buffer->stream;

	 memcpy(&(catch_pokemon->nombre->size_nombre));
	 stream += sizeof(uint32_t);
	 catch_pokemon->nombre->nombre = malloc(catch_pokemon->nombre->size_nombre);
	 memcpy(&(catch_pokemon->nombre->nombre),stream,catch_pokemon->nombre->size_nombre);
	 stream += catch_pokemon->nombre->size_nombre;
	 memcpy(&(catch_pokemon->posicion->posicion_X),stream,sizeof(uint32_t));
	 stream += sizeof(uint32_t);
	 memcpy(&(catch_pokemon->posicion->posicion_Y),stream,sizeof(uint32_t));
	 stream += sizeof(uint32_t);

	 return catch_pokemon;

  }

  void* deserializar_caught_pokemon(t_buffer buffer){

	  Caught_Pokemon caught_pokemon = malloc(sizeof(Caught_Pokemon));

	  void* stream = buffer->stream;

	 memcpy(&(caught_pokemon->valor),stream,sizeof(uint32_t));
	 stream += sizeof(uint32_t);

	 return caught_pokemon;

  }

//Falta deserializar localized

//----------------------------------------------Enlistador(?)-----------------------------------------------------

  /* Se encarga de meter los mensajes (la estructura administrador_mensaje) a la lista, tambien es el que
   * genera y setea los id_unico de cada mensaje
   * Tendrian que ser una cola? estoy un poco perdido en esto, esto como son los generales estan bien como listas?
   */

  void enlistar_mensaje(administrador_mensajes *p,void* unMensaje){ //agrega los nodos al final de la lista
	  administrador_mensajes nuevo;
	  nuevo->un_mensaje->mensaje = unMensaje;
	  nuevo->un_mensaje->id_unico_mensaje = id_unico; // la variable global
	  id_unico++;
	  nuevo->siguiente_info = NULL;
	  if (p == NULL) {
		  p = nuevo;
	  }
	  else {
		  administrador_mensajes auxiliar = p;
		  while (auxiliar->siguiente_info != NULL){
			  auxiliar = auxiliar->siguiente_info;
		  }
		  auxiliar->siguiente_info = nuevo;
	  }
  }

//----------------------------------------------Manejo de Suscripciones------------------------------------------
  /*void**/
  //revisar como hacer llegar la info hasta acá
  int suscripcion_New_Pokemon(int socket_cliente){
	  Suscriber un_suscriptor = malloc(sizeof(Suscriber));
	  //hay que generar un ID random, dárselo al cliente y guardarlo en la estructura para dps agregarlo a lista
  }


return EXIT_SUCCESS;




}



