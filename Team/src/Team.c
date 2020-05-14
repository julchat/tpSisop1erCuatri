
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
	}
	return 0;

}



void* buscarPokemones(void* entrenador){
return entrenador;
}

void asignarObjetivosGlobales(t_list* entrenadores){

}

t_list* armarEntrenadores(infoInicializacion configuracion){
	t_list* entrenadores = list_create();
	t_list* objetivosTodos = configuracion.objetivos;
	t_list* poseidosTodos = configuracion.poseidos;
	t_list* posicionesXTodos = configuracion.posicionesX;
	t_list* posicionesYTodos  = configuracion.posicionesY;
	trainer* unEntrenador;
	t_posicion* unaPosicion;
	int* unaPosicionX;
	int* unaPosicionY;
	for(int i = 0;
	existeDichoEntrenador(i,objetivosTodos,poseidosTodos, posicionesXTodos,posicionesYTodos);
			i++){
		unEntrenador->identificador = i;
		unaPosicionX = list_get(posicionesXTodos,i);
		unaPosicionY = list_get(posicionesYTodos,i);
		unaPosicion->posicion_X = *unaPosicionX;
		unaPosicion->posicion_Y = *unaPosicionY;
		unEntrenador->posicion = *unaPosicion;
		unEntrenador->objetivos = list_get(objetivosTodos,i);
		unEntrenador->poseidos = list_get(poseidosTodos,i);
		unEntrenador = crearYAsignarHilo(unEntrenador);
		list_add(entrenadores,unEntrenador);
	}
	return entrenadores;

}

trainer* crearYAsignarHilo(trainer* unEntrenador){
	pthread_t hiloEntrenador;
	pthread_create(&hiloEntrenador,NULL,buscarPokemones,unEntrenador);
	unEntrenador->hilo = &hiloEntrenador;
	return unEntrenador;
}

void asignarObjetivosGlobales(infoInicializacion configuracion){

}

t_list* aplanarLista(t_list* primeraLista, t_list* segundaLista){

}
