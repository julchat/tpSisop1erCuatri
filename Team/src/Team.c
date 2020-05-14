
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
	asignarObjetivosGlobales(configuracion);
	t_log* loggerTeam;
	t_log* loggerErrores;
	t_log_level level = LOG_LEVEL_INFO;
	t_log_level levelErrores = LOG_LEVEL_WARNING;
	loggerTeam = iniciar_logger_de_nivel_minimo(level, configuracion.logpath);
	loggerErrores = iniciar_logger_de_nivel_minimo(levelErrores,"/tp-2020-1c-CheckPoint/Team/src/loggersErroresPrueba.txt");
	int resultadoExec = planificar(entrenadores,loggerTeam,loggerErrores, configuracion);
	if(resultadoExec==0){
		return -4;
	}
	return 0;
}

int planificar(t_list* entrenadores, t_log* loggerPosta, t_log* loggerPrueba, infoInicializacion configuracion){
	return 1;
}

void* buscarPokemones(void* entrenador){
return entrenador;
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
		unEntrenador->estadoActual = NEW;
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
	t_list* (*punteroACombinarListas)(t_list*,t_list*);
	punteroACombinarListas = &combinarListas;
	t_list* objetivos = configuracion.objetivos;
	objetivos = list_fold(objetivos,NULL,punteroACombinarListas);

}


