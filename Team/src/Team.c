
#include <stdio.h>
#include <stdlib.h>
#include "Team.h"
#include <pthread.h>
#include "libbase.h"
#include "utils.c"
#include "utils.h"
t_list* objetivosGlobales;


int main(){
	objetivosGlobales = list_create();
	t_list* hilosentrenadores = list_create();
	FILE* configfile;
	t_list* entrenadores = list_create();
	char* ip;
	char* puerto;
	int socket;
	configfile = fopen("configFile.txt","r");
	infoInicializacion configuracion;
	inicializarListas(&configuracion);
	configuracion = obtenerConfiguracion(configfile);
	fclose(configfile);
	ip = configuracion.ip;
	puerto = configuracion.puerto;
	socket = crear_conexion(ip,puerto);
	entrenadores = armarEntrenadores(configuracion);
	asignarObjetivosGlobales(entrenadores);
	/*hilosentrenadores = (pthread_nodo*)malloc(cantEntrenadores*sizeof(pthread_nodo));
	for(int i=0; i<cantEntrenadores && !listaVacia(hilosentrenadores) && !listaVacia(entrenadores);i++ ){
		pthread_create(&(hilosentrenadores->hiloentrenador),NULL,(void*)buscarPokemones,&(entrenadores->trainer));
		hilosentrenadores = hilosentrenadores->sig;
		entrenadores = entrenadores->sig;
	}*/
	//TODO: El resto del main va a ser a) actuar de planificador b) administrar colas de mensajes
	return 0;

}



void buscarPokemones(entrenador trainer){
 //TODO: banda de cosas indescriptibles //
}

void asignarObjetivosGlobales(t_list* entrenadores){

}

t_list* armarEntrenadores(infoInicializacion configuracion){
	return 0; //TODO: A partir de las coordenadas y los pokemones poseidos y atrapados, armar una lista de entrenadores//
}



