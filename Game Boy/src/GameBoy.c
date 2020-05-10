#include <stdio.h>
#include <stdlib.h>
#include "GameBoy.h"
#include <pthread.h>
#include <sys/socket.h>

int main(void) {

char* config;
t_log* logger;
char* puerto_game_boy;
char* ip_game_boy;
int socket_cliente;

//Bloque memoria -> para los mensajes (malloquear lo que dice el config que nos dan)




config = leer_config("/home/utnso/TP OPERATIVOS/tp-2020-1c-CheckPoint/GameBoy");

config_set_value(config,"IP_GAME_BOY","127.0.0.1");
config_set_value(config,"PUERTO_GAME_BOY",35111);//el video decía que le mande un valor entre 35k y 40 k :)

//config_set_value(config,"FRECUENCIA_COMPACTACION",aca va el valor de la frecuencia de compactacion[Numerico]);
config_set_value(config,"LOG_FILE","/home/utnso/TP OPERATIVOS/tp-2020-1c-CheckPoint/GameBoy");



puerto_game_boy = config_get_string_value(config,"PUERTO_GAME_BOY");
ip_game_boy = config_get_string_value(config,"IP_GAME_BOY");

logger = iniciar_logger_de_nivel_minimo(LOG_LEVEL_ERROR,"/home/utnso/TP OPERATIVOS/tp-2020-1c-CheckPoint/GameBoy");


socket_cliente= crear_conexion(ip_game_boy,puerto_game_boy);

}



