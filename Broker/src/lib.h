#ifndef LIB_H_
#define LIB_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>

//---------------------------------------Configuración Broker--------------------------------------------//

typedef struct
{
	int tamano_memoria;
	int tamano_minimo_particion;
	char* algoritmo_memoria;
	char* algoritmo_reemplazo;
	char* algoritmo_particion_libre;
	char* ip_broker;
	int puerto_broker;
	int frecuencia_compactacion;
	char* log_file;

}t_config_broker;


//---------------------------------------Estructura de paquetes-------------------------------------------//


typedef enum
{
	MENSAJE = 1,
	NEW_POKEMON = 2,
	LOCALIZED_POKEMON = 3,
	GET_POKEMON = 4,
	APPEARED_POKEMON = 5,
	CATCH_POKEMON = 6,
	COUGHT_POKEMON = 7,
	SUSCRIBER_NEW_POKEMON = 8,
	SUSCRIBER_LOCALIZED_POKEMON = 9,
	SUSCRIBER_GET_POKEMON = 10,
	SUSCRIBER_APPEARED_POKEMON = 11,
	SUSCRIBER_CATCH_POKEMON = 12,
	SUSCRIBER_COUGTH_POKEMON = 13,
	RECIBIR_SUSCRIPCION = 14,

}op_code; //IDENTIFICADORES DE CADA TIPO DE MENSAJE

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;


// ------------------------------- Manejo de loggers y conexiones --------------------------------------


t_log* iniciar_logger_de_nivel_minimo(t_log_level level, char* ruta){

	t_log *logger = log_create(ruta,"logger",true,level);
	if(logger == NULL){
		printf("No pude crear el logger \n");
		exit(1);
	}

	return logger;
}

t_config* leer_config(char* ruta){

	t_config* config = config_create(ruta);
	if(config == NULL){
		printf("No pude leer la config \n");
		exit(2);
	}

	return config;
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

void terminar_programa(int conexion, t_log* logger, t_config* config){

	if(logger != NULL){
		log_destroy(logger);
	}

	if(config != NULL){
		config_destroy(config);
	}

	liberar_conexion(conexion);
}

//---------------------------------------------------------Manejo de mensaje---------------------------------

void* serializar_paquete(t_paquete* paquete, int *bytes){

	int size_serializado = sizeof(int) + sizeof(int) + paquete->buffer->size;
	void *buffer = malloc(size_serializado);

	int bytes_escritos = 0;

	memcpy(buffer + bytes_escritos, &(paquete->codigo_operacion), sizeof(paquete->codigo_operacion));
	bytes_escritos += sizeof(paquete->codigo_operacion);

	memcpy(buffer + bytes_escritos, &(paquete->buffer->size), sizeof(paquete->buffer->size));
	bytes_escritos += sizeof(paquete->buffer->size);

	memcpy(buffer + bytes_escritos, paquete->buffer->stream, paquete->buffer->size);
	bytes_escritos += paquete->buffer->size; 


	bytes = size_serializado;
	return buffer;

}

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		printf("error");

	freeaddrinfo(server_info);

	return socket_cliente;
}


void enviar_mensaje(char* mensaje, int socket_cliente){

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->stream = mensaje;
	paquete->buffer->size = strlen(mensaje)+1;
	int size_serializado;
	void *serializado = serializar_paquete(paquete, &size_serializado);
	send(socket_cliente, serializado, size_serializado, 0);
	free(serializado);

}

char* recibir_mensaje(int socket_cliente){

	op_code operacion;
	recv(socket_cliente, &operacion, sizeof(operacion),0);
	int buffer_size;
	recv(socket_cliente, &buffer_size, sizeof(buffer_size), 0);
	char *buffer = malloc(buffer_size);
	recv(socket_cliente, buffer, buffer_size, 0);
	return buffer;
}

//--------------------------------------------------Colas de Mensajes------------------------------------

typedef struct{
	New_Pokemon mensaje;
	cola_new_pokemon *head;
	cola_new_pokemon *tail;
}cola_new_pokemon;

typedef struct{
	Localized_Pokemon mensaje;
	cola_localized_pokemon *head;
	cola_localized_pokemon *tail;
}cola_localized_pokemon;

typedef struct{
	Get_Pokemon mensaje;
	cola_get_pokemon *head;
	cola_get_pokemon *tail;
}cola_get_pokemon;

typedef struct{
	Appeared_Pokemon mensaje;
	cola_appeared_pokemon *head;
	cola_appeared_pokemon *tail;
}cola_appeared_pokemon;

typedef struct{
	Catch_Pokemon mensaje;
	cola_catch_pokemon *head;
	cola_catch_pokemon *tail;
}cola_catch_pokemon;

typedef struct{
	Caught_Pokemon mensaje;
	cola_caught_pokemon *head;
	cola_caught_pokemon *tail;
}cola_caught_pokemon;


//--------------------------------------------------Estructuras de mensajes------------------------------


typedef struct{
	uint32_t posicion_X;
	uint32_t posicion_Y;
}t_posicion;


typedef struct{
	uint32_t size_nombre;
	char* nombre;
}t_nombre_pokemon;


typedef struct{
	t_nombre_pokemon nombre;
	t_posicion posicion;
	uint32_t cantidad;

}New_Pokemon;

/* Para enviar el mensaje localized hay que serializar la siguiente estructura, la cual sera introducida
 en el void* stream del t_buffer, cuando la deserializemos la cargamos en el struct Localized_Pokemon
 el formateo del string a seguir es nombre-cantidad_coordenadas-coordenadaX1-coordenadaY1-coordenadaX2-coordenadaY2...
 sin los "-"
*/

typedef struct{
	t_nombre_pokemon nombre;
	uint32_t cantidad_coordenadas;
	t_posicion posiciones[];

}Localized_Pokemon;


typedef struct{
	t_nombre_pokemon nombre;
}Get_Pokemon;


typedef struct{
	t_nombre_pokemon nombre;
	t_posicion posicion;
}Appeared_Pokemon,Catch_Pokemon;


typedef struct{
	uint32_t valor;

}Caught_Pokemon;

//--------------------------------------------------Estructuras de suscribers y listas de suscribers---------------------------------------

typedef struct{
	uint32_t id_suscriptor;
	uint32_t lista_suscripta;
	int socket_suscriptor;
}Suscriber; //me lo manda el suscriber por mje

typedef struct{
	Suscriber un_suscriptor;
	Suscribers_New_Pokemon* siguiente_suscriptor;

}Suscribers_New_Pokemon;

typedef struct{
	Suscriber un_suscriptor;
	Suscribers_Localized_Pokemon* siguiente_suscriptor;

}Suscribers_Localized_Pokemon;

typedef struct{
	Suscriber un_suscriptor;
	Suscribers_Get_Pokemon* siguiente_suscriptor;

}Suscribers_Get_Pokemon;


typedef struct{
	Suscriber un_suscriptor;
	Suscribers_Appeared_Pokemon* siguiente_suscriptor;

}Suscribers_Appeared_Pokemon;

typedef struct{
	Suscriber un_suscriptor;
	Suscribers_Catch_Pokemon* siguiente_suscriptor;

}Suscribers_Catch_Pokemon;

typedef struct{
	Suscriber un_suscriptor;
	Suscribers_Caught_Pokemon* siguiente_suscriptor;

}Suscribers_Caught_Pokemon;

//--------------------------------------------------Estructuras del Broker------------------------------------------
//son las estructuras que se van a guardar en el broker (cachear)

typedef struct{
	int id_unico_mensaje; //tiene que ser UNICO
	void *mensaje; // <------ Mensaje sin el op_code, lo cargamos desde "atender_cliente"
	//suscriptores_mensaje_eviado* un_suscriptor;
	//suscriptores_respondieron_ACK* un_suscriptor;

}info_mensaje;

typedef struct{
	info_mensaje un_mensaje;
	administrador_mensajes* siguiente_info;

}administrador_mensajes;


#endif
