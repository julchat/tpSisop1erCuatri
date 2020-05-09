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


//--------------------------------------------------------------------------------- Estructura de paquetes


typedef enum
{
	MENSAJE = 1,
}op_code;

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


// -------------------------------------------------------------------------- Manejo de loggers y conexiones 


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

//---------------------------------------------------------------------- Manejo de mensaje

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

#endif
