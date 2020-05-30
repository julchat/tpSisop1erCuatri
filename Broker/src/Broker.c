#include <stdio.h>
#include <stdlib.h>
#include "lib.h"
#include <pthread.h>
#include <sys/socket.h>
#include <commons/collections/list.h>


int main(void) {

char* config;
t_log* logger;
char* puerto_broker;
char* ip_broker;
int socket_client;

int id_unico = 0; // ojo con esto, ¡¡¡¡capaz!!! hay que sincronizarlo, lo vamos a usar como un contador

administrador_mensajes* cola_mensajes_new = NULL;
administrador_mensajes* cola_mensajes_localized = NULL;
administrador_mensajes* cola_mensajes_get = NULL;
administrador_mensajes* cola_mensajes_appeared = NULL;
administrador_mensajes* cola_mensajes_catch = NULL;
administrador_mensajes* cola_mensajes_caught = NULL;


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
				encolar_mensaje(cola_mensajes_new,mensaje_new);
				free(mensaje_new);
				break;

		case 3: Localized_Pokemon* mensaje_localized = deserializar_localized_pokemon(paquete->buffer); //Falta hacer esta funcion con el tema de los vectores :C
				encolar_mensaje(cola_mensajes_localized,mensaje_localized);
				free(mensaje_localized);
			    break;

		case 4: Get_Pokemon* mensaje_get = deserializar_get_pokemon(paquete->buffer);
				encolar_mensaje(cola_mensajes_get,mensaje_get);
				free(mensaje_get);
				break;

		case 5: Appeared_Pokemon* mensaje_appeared = deserializar_appeared_pokemon(paquete->buffer);
				encolar_mensaje(cola_mensajes_appeared,mensaje_appeared);
				free(mensaje_appeared);
				break;

		case 6: Catch_Pokemon* mensaje_catch = deserializar_catch_pokemon(paquete->buffer);
				encolar_mensaje(cola_mensajes_catch,mensaje_catch);
				free(mensaje_catch);
				break;

		case 7: Caught_Pokemon* mensaje_caught = deserializar_caught_pokemon(paquete->buffer);
				encolar_mensaje(cola_mensajes_caught,mensaje_caught);
				free(mensaje_caught);
				break;
		case 14: Suscribir(paquete->buffer);
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

// Cuando se serialize este mensaje, todos sus atributos deben mandarse como un string plano con el siguiente orden
// nombreCantidad_coordenadasCoordenadaX1CoordenadaY1CoordenadaX2CoordenadaY2...
// sin los "-"

  void* deserializar_localized_pokemon(t_buffer buffer){

	  Localized_Pokemon localized_pokemon = malloc(sizeof(Localized_Pokemon));

	  void* temporal = buffer->stream;
	  uint32_t longitud_nombre_pokemon;
	  memcpy(&(longitud_nombre_pokemon),temporal,sizeof(uint32_t));
	  temporal += sizeof(uint32_t);
	  char* nombre_pokemon = malloc(longitud_nombre_pokemon);
	  memcpy(&(nombre_pokemon),temporal,longitud_nombre_pokemon);
	  temporal += longitud_nombre_pokemon;
	  uint32_t cantidad_posiciones;
	  memcpy(&(cantidad_posiciones),temporal,sizeof(uint32_t));
	  temporal += sizeof(uint32_t);

	  localized_pokemon->nombre->size_nombre = longitud_nombre_pokemon;
	  localized_pokemon->nombre->nombre = nombre_pokemon;
	  localized_pokemon->cantidad_coordenadas = cantidad_posiciones;


	  for(int i; i<cantidad_posiciones; i++){

		  uint32_t posicionX;
		  uint32_t posicionY;

		  memcpy(&(posicionX),temporal,sizeof(uint32_t));
		  temporal += sizeof(uint32_t);
		  memcpy(&(posicionY),temporal,sizeof(uint32_t));
		  temporal += sizeof(uint32_t);

		  t_posicion posiciones; // Si hay segmentation fault -> capaz hay que hacerle malloc.
		  posiciones->posicion_X = posicionX;
		  posiciones->posicion_Y = posicionY;

		  localized_pokemon->posiciones[i] = posiciones;

	  }

	  return localized_pokemon;
  }



//----------------------------------------------Enlistador(?)-----------------------------------------------------

  /* Se encarga de meter los mensajes (la estructura administrador_mensaje) a la lista, tambien es el que
   * genera y setea los id_unico de cada mensaje
   */

  void encolar_mensaje(administrador_mensajes *p,void* unMensaje){ //Agrega los nodos al final de la lista
	  administrador_mensajes nuevo;
	  nuevo->un_mensaje->mensaje = unMensaje;
	  nuevo->un_mensaje->id_unico_mensaje = id_unico; // La variable global
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

  // faltan las funciones que saquen mensajes de las colas

//----------------------------------------------Manejo de Suscripciones------------------------------------------
  void suscribir(t_buffer buffer, int socket_clente){
      Suscriber un_suscriptor = malloc(sizeof(Suscriber));
      void* stream = buffer->stream;

      memcpy(&(un_suscriptor->id_suscriptor),stream,sizeof(uint32_t));
      stream+=sizeof(uint32_t);
      memcpy(&(un_suscriptor->lista_suscripta),stream,sizeof(uint32_t));
      stream+=sizeof(uint32_t);
      un_suscriptor->lista_suscripta = socket_clente;

      switch(un_suscriptor->lista_suscripta){ //Me falta saber cómo establecer conexión entre el Broker y los suscriptores para poder mandarles mjes (falta en la estructura suscriber)

      case 8 = suscripcion_New_Pokemon(Suscribers_New_Pokemon,un_suscriptor);

      case 9 = suscripcion_Localized_Pokemon(Suscribers_Localized_Pokemon,un_suscriptor);

      case 10 = suscripcion_Get_Pokemon(Suscribers_Get_Pokemon,un_suscriptor);

      case 11 = suscripcion_Appeared_Pokemon(Suscribers_Appeared_Pokemon, un_suscriptor);

      case 12 = suscripcion_Catch_Pokemon(Suscriber_Catch_Pokemon,un_suscriptor);

      case 13 = suscripcion_Cought_Pokemon(Suscribers_Cought_Pokemon,un_suscriptor);

  }

return EXIT_SUCCESS;




}



