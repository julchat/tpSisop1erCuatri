#ifndef LIB_H_
#define LIB_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/collections/list.h>
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


t_log* iniciar_logger_de_nivel_minimo(t_log_level level, char* ruta);


t_config* leer_config(char* ruta);



void liberar_conexion(int socket_cliente);

void terminar_programa(int conexion, t_log* logger, t_config* config);

//---------------------------------------------------------------------- Manejo de mensaje

void* serializar_paquete(t_paquete* paquete, int *bytes);


int crear_conexion(char *ip, char* puerto);



void enviar_mensaje(char* mensaje, int socket_cliente);


#endif
