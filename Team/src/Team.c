
#include <stdio.h>
#include <stdlib.h>
#include "Team.h"
#include <pthread.h>
#include "libbase.h"
#include "utils.c"
#include "utils.h"
#include "semaphore.h"
t_list* objetivosGlobales;
Estado new;
Estado ready;
Estado exec;
Estado blocked;
Estado term;
bool modoDeadlock = 0;
GodStruct* info;
t_list* entrenadores;
int main(){
	new.tipo = NEW;
	ready.tipo = READY;
	exec.tipo = EXEC;
	blocked.tipo = BLOCKED;
	term.tipo = TERM;
	objetivosGlobales = list_create();
	FILE* configfile;
	entrenadores = list_create();
	pthread_t planificador;
	char* ip;
	char* puerto;
	int socket;
	t_log* loggerTeam;

	configfile = fopen("configFile.txt","r");
	infoInicializacion configuracion;
	inicializarListas(&configuracion,&new,&ready,&exec,&blocked,&term);
	configuracion = obtenerConfiguracion(configfile);
	fclose(configfile);
	ip = configuracion.ip;
	puerto = configuracion.puerto;
	socket = crear_conexion(ip,puerto);
	loggerTeam = iniciar_logger_de_nivel_minimo(LOG_LEVEL_INFO, configuracion.logpath);
	asignarObjetivosGlobales(configuracion);
	info->logger = loggerTeam;
	entrenadores = armarEntrenadores(configuracion);
	info->configuracion.entrenadores = entrenadores;
	configuracion.entrenadores = entrenadores;
}
void buscarPokemones(int* id){

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
		unEntrenador = crearYAsignarHilo(unEntrenador,i);
		list_add(entrenadores,unEntrenador);
	}
		return entrenadores;

}

trainer* crearYAsignarHilo(trainer* unEntrenador,i){
	int* id;
	*id = i;
	pthread_t hiloEntrenador;
	pthread_create(&hiloEntrenador,NULL,buscarPokemones,id);
	unEntrenador->hilo = &hiloEntrenador;
	unEntrenador->estadoActual = NEW;
	list_add(new.entrenadores,unEntrenador);
	new.cantHilos++;
	return unEntrenador;
}

void asignarObjetivosGlobales(infoInicializacion configuracion){
	t_list* (*punteroACombinarListas)(t_list*,t_list*);
	punteroACombinarListas = &combinarListas;
	t_list* objetivos = configuracion.objetivos;
	objetivos = list_fold(objetivos,NULL,punteroACombinarListas);

}

trainer decidirFIFO(){
	trainer* aEjecutar;
 t_list* entrenadores = ready.entrenadores;
 if(ready.cantHilos !=0){
	 aEjecutar = list_get(entrenadores,0);
	 return *aEjecutar;
 }
 else{
	 printf("no hay entrenadores para decidir cual ejecuta");
	 exit(-50);
 }
}
