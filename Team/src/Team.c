
#include <stdio.h>
#include <stdlib.h>
#include "Team.h"
#include <pthread.h>
#include "libbase.h"
#include "utils.c"
#include "utils.h"
listapokemon* objetivosGlobales;


int main(){
	pthread_nodo* hilosentrenadores;
	FILE* configfile;
	listaentrenadores* entrenadores;
	char* ip;
	char* puerto;
	int socket;
	configfile = fopen("configFile.txt","r");
	int cantEntrenadores;
	infoInicializacion configuracion;
	configuracion = obtenerConfiguracion(configfile);
	fclose(configfile);
	ip = configuracion.ip;
	puerto = configuracion.puerto;
	socket = crear_conexion(ip,puerto);
	entrenadores = armarEntrenadores(configuracion);
	cantEntrenadores = tamanioListaEntrenadores(entrenadores);
	asignarObjetivosGlobales(entrenadores,cantEntrenadores);
	hilosentrenadores = (pthread_nodo*)malloc(cantEntrenadores*sizeof(pthread_nodo));
	for(int i=0; i<cantEntrenadores && !listaVacia(hilosentrenadores) && !listaVacia(entrenadores);i++ ){
		pthread_create(&(hilosentrenadores->hiloentrenador),NULL,buscarPokemones,&(entrenadores->trainer));
		hilosentrenadores = hilosentrenadores->sig;
		entrenadores = entrenadores->sig;
	}
	//TODO: El resto del main va a ser a) actuar de planificador b) administrar colas de mensajes
	return 0;

}
void buscarPokemones(entrenador trainer){
 //TODO: banda de cosas indescriptibles //
}

void asignarObjetivosGlobales(listaentrenadores* entrenadores, int cantEntrenadores){
	for(int i=0; i<cantEntrenadores && !(listaVacia(entrenadores)) ;i++){

	}
}

listaentrenadores* armarEntrenadores(infoInicializacion){
	return 0; //TODO: A partir de las coordenadas y los pokemones poseidos y atrapados, armar una lista de entrenadores//
}



