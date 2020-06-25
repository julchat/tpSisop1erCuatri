#include <stdio.h>
#include <stdlib.h>
#include "lib.h"
#include <pthread.h>
#include <sys/socket.h>
#include <commons/collections/list.h>


/* To do list:
 * 1) Crear la funcion que envie mensajes
 * 2) Ver el tema de los id_correlativos, hay que modificar los deserializadores (agregar id_correlativo a los void* stream)
 * 3) Faltan las funciones de "sacar de memoria"
 * 4) todo el tema de compactacion de memoria
 * 5) los logs restantes (mas abajo se especifica)
 *
 *  */


// revisar que va en el main y que no !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

int id_unico = 0; // ojo con esto, ¡¡¡¡capaz!!! hay que sincronizarlo, lo vamos a usar como un contador

int main(void) {


int socket_client;


administrador_mensajes* cola_mensajes_new = NULL;
administrador_mensajes* cola_mensajes_localized = NULL;
administrador_mensajes* cola_mensajes_get = NULL;
administrador_mensajes* cola_mensajes_appeared = NULL;
administrador_mensajes* cola_mensajes_catch = NULL;
administrador_mensajes* cola_mensajes_caught = NULL;



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


// Logs hechos:
/*1(conexion de un proceso al broker)
 *3(llegada de un nuevo mensaje a la cola de mensajes)
 *Faltan 2,4,5,6,7,8
 */

// --------------------------------------------------- Memoria -----------------------------------------------------

void* memoria_broker = malloc(configuracion_broker->tamano_memoria);
void* siguiente_posicion_libre = memoria_broker;


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
	paquete->buffer = malloc(sizeof(t_buffer));
	recv(socket_cliente,&(paquete->codigo_operacion),sizeof(uint8_t),0);

	recv(socket_cliente,&(paquete->buffer->size),sizeof(uint32_t),0);
	paquete->buffer->stream = malloc(paquete->buffer->size);
	recv(socket_cliente,&(paquete->buffer),sizeof(paquete->buffer->size),0);

	//Aca podemos agregar un recv mas para el tema del id_correlativo, asi no modifico los descerializadores
	//lo mandariamos ultimo en la secuencia para appeared , caught y localized


	switch(paquete->codigo_operacion){
		case 2: New_Pokemon* mensaje_new = deserializar_new_pokemon(paquete->buffer);
				encolar_mensaje(cola_mensajes_new,paquete->codigo_operacion);
				cargar_new_en_memoria(mensaje_new);
				// Falta meter a cola la estructura administrativa del mensaje
				log_info(logger,"Se agregó un mensaje a la cola de mensajes New_Pokemon");
				free(mensaje_new->nombre->nombre);
				free(mensaje_new);
				break;

		case 3: Localized_Pokemon* mensaje_localized = deserializar_localized_pokemon(paquete->buffer);
				encolar_mensaje(cola_mensajes_localized,paquete->codigo_operacion);
				cargar_localized_en_memoria(mensaje_localized);
				log_info(logger,"Se agregó un mensaje a la cola de mensajes Localized_Pokemon");
				free(mensaje_localized->nombre->nombre);
				free(mensaje_localized);
			    break;

		case 4: Get_Pokemon* mensaje_get = deserializar_get_pokemon(paquete->buffer);
				encolar_mensaje(cola_mensajes_get,paquete->codigo_operacion);
				cargar_get_en_memoria(mensaje_get);
				log_info(logger,"Se agregó un mensaje a la cola de mensajes Get_Pokemon");
				free(mensaje_get->nombre->nombre);
				free(mensaje_get);
				break;

		case 5: Appeared_Pokemon* mensaje_appeared = deserializar_appeared_pokemon(paquete->buffer);
				encolar_mensaje(cola_mensajes_appeared,paquete->codigo_operacion);
				cargar_appeared_en_memoria(mensaje_appeared);
				log_info(logger,"Se agregó un mensaje a la cola de mensajes Appeared_Pokemon");
				free(mensaje_appeared->nombre->nombre);
				free(mensaje_appeared);
				break;

		case 6: Catch_Pokemon* mensaje_catch = deserializar_catch_pokemon(paquete->buffer);
				encolar_mensaje(cola_mensajes_catch,paquete->codigo_operacion);
				cargar_catch_en_memoria(mensaje_catch);
				log_info(logger,"Se agregó un mensaje a la cola de mensajes Catch_Pokemon");
				free(mensaje_catch->nombre->nombre);
				free(mensaje_catch);
				break;

		case 7: Caught_Pokemon* mensaje_caught = deserializar_caught_pokemon(paquete->buffer);
				cargar_caught_en_memoria(mensaje_caught);
				encolar_mensaje(cola_mensajes_caught,paquete->codigo_operacion);
				log_info(logger,"Se agregó un mensaje a la cola de mensajes Caught_Pokemon");
				free(mensaje_caught);
				break;

		case 14: Suscribir(paquete->buffer);
				break;
	}

				free(paquete->buffer->stream);
				free(paquete->buffer);
				free(paquete);
}


//----------------------------------------------Cargar mensajes en memoria----------------------------------------------------------------

  void cargar_new_en_memoria(New_Pokemon new_pokemon){

	  	memcpy(siguiente_posicion_libre,new_pokemon->nombre->size_nombre,sizeof(uint32_t));
	  	siguiente_posicion_libre += sizeof(uint32_t);
	  	memcpy(siguiente_posicion_libre,new_pokemon->nombre->nombre,new_pokemon->nombre->size_nombre);
	  	siguiente_posicion_libre += new_pokemon->nombre->size_nombre;
	  	memcpy(siguiente_posicion_libre,new_pokemon->posicion->posicion_X,sizeof(uint32_t));
	  	siguiente_posicion_libre += sizeof(uint32_t);
	  	memcpy(siguiente_posicion_libre,new_pokemon->posicion->posicion_Y,sizeof(uint32_t));
	  	siguiente_posicion_libre += sizeof(uint32_t);
	  	memcpy(siguiente_posicion_libre,new_pokemon->cantidad,sizeof(uint32_t));
	  	siguiente_posicion_libre += sizeof(uint32_t);

  }


  void cargar_localized_en_memoria(Localized_Pokemon localized_pokemon){

  	  	memcpy(siguiente_posicion_libre,localized_pokemon->nombre->size_nombre,sizeof(uint32_t));
  	  	siguiente_posicion_libre += sizeof(uint32_t);
  	  	memcpy(siguiente_posicion_libre,localized_pokemon->nombre->nombre,localized_pokemon->nombre->size_nombre);
  	  	siguiente_posicion_libre += localized_pokemon->nombre->size_nombre;
  	  	memcpy(siguiente_posicion_libre,localized_pokemon->cantidad_coordenadas,sizeof(uint32_t));
  	  	siguiente_posicion_libre += sizeof(uint32_t);

  	  	for(int i; i<localized_pokemon->cantidad_coordenadas; i++){

  			  memcpy(siguiente_posicion_libre,localized_pokemon->posiciones[i]->posicion_X,sizeof(uint32_t));
  			  siguiente_posicion_libre += sizeof(uint32_t);
  			  memcpy(siguiente_posicion_libre,localized_pokemon->posiciones[i]->posicion_Y,sizeof(uint32_t));
  			  siguiente_posicion_libre += sizeof(uint32_t);

  }


  void cargar_get_en_memoria(Get_Pokemon get_pokemon){

  	 	memcpy(siguiente_posicion_libre,get_pokemon->nombre->size_nombre,sizeof(uint32_t));
  	 	siguiente_posicion_libre += sizeof(uint32_t);
  		memcpy(siguiente_posicion_libre,get_pokemon->nombre->nombre,get_pokemon->nombre->size_nombre);
  		siguiente_posicion_libre += get_pokemon->nombre->size_nombre;

  }


  void cargar_appeared_en_memoria(Appeared_Pokemon appeared_pokemon){

	    memcpy(siguiente_posicion_libre,appeared_pokemon->nombre->size_nombre,sizeof(uint32_t));
	  	siguiente_posicion_libre += sizeof(uint32_t);
	  	memcpy(siguiente_posicion_libre,appeared_pokemon->nombre->nombre,appeared_pokemon->nombre->size_nombre);
	  	siguiente_posicion_libre += appeared_pokemon->nombre->size_nombre;
	  	memcpy(siguiente_posicion_libre,appeared_pokemon->posicion->posicion_X,sizeof(uint32_t));
	  	siguiente_posicion_libre += sizeof(uint32_t);
	  	memcpy(siguiente_posicion_libre,appeared_pokemon->posicion->posicion_Y,sizeof(uint32_t));
	  	siguiente_posicion_libre += sizeof(uint32_t);
  }


  void cargar_catch_en_memoria(Catch_Pokemon catch_pokemon){

	    memcpy(siguiente_posicion_libre,catch_pokemon->nombre->size_nombre,sizeof(uint32_t));
	  	siguiente_posicion_libre += sizeof(uint32_t);
	  	memcpy(siguiente_posicion_libre,catch_pokemon->nombre->nombre,catch_pokemon->nombre->size_nombre);
	  	siguiente_posicion_libre += catch_pokemon->nombre->size_nombre;
	  	memcpy(siguiente_posicion_libre,catch_pokemon->posicion->posicion_X,sizeof(uint32_t));
	  	siguiente_posicion_libre += sizeof(uint32_t);
	  	memcpy(siguiente_posicion_libre,catch_pokemon->posicion->posicion_Y,sizeof(uint32_t));
	  	siguiente_posicion_libre += sizeof(uint32_t);
  }


  void cargar_caught_en_memoria(Caught_Pokemon caught_pokemon){

	    memcpy(siguiente_posicion_libre,caught_pokemon->valor,sizeof(uint32_t));
	  	siguiente_posicion_libre += sizeof(uint32_t);

  }


  //----------------------------------------------Sacar mensajes de memoria (si pero no)----------------------------------------------------------------




  //----------------------------------------------Manejo de colas(que son listas, shh nadie lo sabe)----------------------------------------


  /* Se encarga de meter los info_mensajes (la estructura administrador_mensaje) a la lista, tambien es el que
   * genera y setea los id_unico de cada mensaje
   */

  // Dudas: Quiero el opcode? me sirve de algo tenerlo?

  void encolar_mensaje(administrador_mensajes *p, int op_code){ //Agrega los nodos al final de la lista
	  administrador_mensajes nuevo;
	  nuevo->un_mensaje->principio_del_mensaje_en_memoria = siguiente_posicion_libre; // Funciona de momento, cuando tenga compactacion esto vuela
    //nuevo->un_mensaje->id_correlativo Hay que pasarlo por
	  nuevo->un_mensaje->op_code = op_code;
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
  }

return EXIT_SUCCESS;

}


