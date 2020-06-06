#include <stdio.h>
#include <stdlib.h>
#include "lib.h"
#include <pthread.h>
#include <sys/socket.h>
#include <commons/collections/list.h>


// revisar que va en el main y que no

int id_unico = 0; // ojo con esto, ¡¡¡¡capaz!!! hay que sincronizarlo, lo vamos a usar como un contador

int main(void) {


int socket_client;


administrador_mensajes* cola_mensajes_new = NULL;
administrador_mensajes* cola_mensajes_localized = NULL;
administrador_mensajes* cola_mensajes_get = NULL;
administrador_mensajes* cola_mensajes_appeared = NULL;
administrador_mensajes* cola_mensajes_catch = NULL;
administrador_mensajes* cola_mensajes_caught = NULL;


//Bloque memoria -> para los mensajes (malloquear lo que dice el config que nos dan) FALTA!!!!!!!!!!!!!!!!!!!!!!

// --------------------------------- Levantando la configuración ---------------------------------------------------

t_config_broker configuracion_broker;

t_config* config = config_create("config.txt"); // Asumimos que el archivo está en la misma carpeta que el modulo.

configuracion_broker->tamano_memoria =  config_get_int_value(config,"TAMANO_MEMORIA");
configuracion_broker->tamano_minimo_particion = config_get_int_value(config,"TAMANO_MINIMO_PARTICION");
configuracion_broker->algoritmo_memoria = config_get_string_value(config,"ALGORITMO_MEMORIA");
configuracion_broker->algoritmo_reemplazo = config_get_string_value(config,"ALGORITMO_REEMPLAZO");
configuracion_broker->algoritmo_particion_libre = config_get_string_value(config,"ALGORITMO_PARTICION_LIBRE");
configuracion_broker->ip_broker = config_get_string_value(config,"IP_BROKER");
configuracion_broker->puerto_broker =  config_get_string_value(config,"PUERTO_BROKER");
configuracion_broker->frecuencia_compactacion =  config_get_int_value(config,"FRECUENCIA_COMPACTACION");
configuracion_broker->log_file = config_get_string_value(config,"LOG_FILE");

t_log* logger = iniciar_logger_de_nivel_minimo(LOG_LEVEL_INFO,configuracion_broker->log_file);
socket_client = crear_conexion(configuracion_broker->ip_broker,configuracion_broker->puerto_broker);
// Hay un problema de tipos con crear_conexion y lo que levantamos desde la config, IP_BROKER es un INT pero la funcion
// lo usa como char* (lo levantamos como char*, no hay problema)

// -------------------------------------------- Punteros a las listas ----------------------------------------------


	while(1){
		esperar_conexion(socket_client);
	}

void esperar_conexion(int socket_cliente){
	int cliente = aceptar_cliente(socket_cliente);
	log_info(logger,"Se conecto un cliente con el socket numero %d", cliente);
	pthread_t hilo_nuevo_cliente;
		if(pthread_create(&hilo_nuevo_cliente,NULL,(void*)atender_cliente,(void*)cliente)!=0){
			log_error(logger,"Error al crear el hilo de cliente");
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
				log_info(logger,"Se agregó un mensaje a la cola de mensajes New_Pokemon");
				free(mensaje_new);
				break;

		case 3: Localized_Pokemon* mensaje_localized = deserializar_localized_pokemon(paquete->buffer); //Falta hacer esta funcion con el tema de los vectores :C
				encolar_mensaje(cola_mensajes_localized,mensaje_localized);
				log_info(logger,"Se agregó un mensaje a la cola de mensajes Localized_Pokemon");
				free(mensaje_localized);
			    break;

		case 4: Get_Pokemon* mensaje_get = deserializar_get_pokemon(paquete->buffer);
				encolar_mensaje(cola_mensajes_get,mensaje_get);
				log_info(logger,"Se agregó un mensaje a la cola de mensajes Get_Pokemon");
				free(mensaje_get);
				break;

		case 5: Appeared_Pokemon* mensaje_appeared = deserializar_appeared_pokemon(paquete->buffer);
				encolar_mensaje(cola_mensajes_appeared,mensaje_appeared);
				log_info(logger,"Se agregó un mensaje a la cola de mensajes Appeared_Pokemon");
				free(mensaje_appeared);
				break;

		case 6: Catch_Pokemon* mensaje_catch = deserializar_catch_pokemon(paquete->buffer);
				encolar_mensaje(cola_mensajes_catch,mensaje_catch);
				log_info(logger,"Se agregó un mensaje a la cola de mensajes Catch_Pokemon");
				free(mensaje_catch);
				break;

		case 7: Caught_Pokemon* mensaje_caught = deserializar_caught_pokemon(paquete->buffer);
				encolar_mensaje(cola_mensajes_caught,mensaje_caught);
				log_info(logger,"Se agregó un mensaje a la cola de mensajes Caught_Pokemon");
				free(mensaje_caught);
				break;

		case 14: Suscribir(paquete->buffer);
				 break;
	}
}


//----------------------------------------------Manejo de colas(que son listas, shh nadie lo sabe)----------------------------------------

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

  // faltan las funciones que saquen mensajes de las colas, hay que ver el criterio para sacar mensajes de las colas


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

  }

return EXIT_SUCCESS;

}


