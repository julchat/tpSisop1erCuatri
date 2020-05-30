
#include <stdio.h>
#include <stdlib.h>
#include "Team.h"
#include <pthread.h>
#include "libbase.h"
#include "utils.h"
#include "semaphore.h"
#include "commons/collections/list.h"
#include <string.h>
t_list* objetivosGlobales;
Estado new;
Estado ready;
Estado exec;
Estado blocked;
Estado term;
bool modoDeadlock = 0;
GodStruct* info;
t_list* entrenadores;
pthread_t hiloGets;
pthread_t hiloAppeared;
pthread_t hiloLocalised;
pthread_t hiloCaught;
pthread_t hiloMantenimientoConexion;

int main(){
	new.tipo = NEW;
	ready.tipo = READY;
	exec.tipo = EXEC;
	blocked.tipo = BLOCKED;
	term.tipo = TERM;
	FILE* configfile;
	entrenadores = list_create();
	t_log* loggerTeam;
	configfile = fopen("configFile.txt","r");
	infoInicializacion configuracion;
	inicializarListas(&configuracion,&new,&ready,&exec,&blocked,&term);
	configuracion = obtenerConfiguracion(configfile);
	fclose(configfile);
	pthread_create(&hiloGets,NULL,mandarGets,NULL);
	loggerTeam = iniciar_logger_de_nivel_minimo(LOG_LEVEL_INFO, configuracion.logpath);
	asignarObjetivosGlobales(configuracion);
	info->logger = loggerTeam;
	info->configuracion = configuracion;
	entrenadores = armarEntrenadores(configuracion);
	info->configuracion.entrenadores = entrenadores;
	configuracion.entrenadores = entrenadores;
	limpiarObjetivosCumplidos();
}

void limpiarObjetivosCumplidos(){
	char* unPokemon;
	char* pokemonObjetivo;
	t_list* (*punteroACombinarListas)(t_list*,t_list*);
	punteroACombinarListas = &combinarListas;
	t_list* poseidosAplanados = list_duplicate(info->configuracion.poseidos);
	bool encontrado;
	void (*punteroAFree)(void*);
	punteroAFree = &free;

	poseidosAplanados = list_fold(poseidosAplanados,NULL,punteroACombinarListas);
	for(int i=0; i<poseidosAplanados->elements_count;i++){
		unPokemon = list_get(poseidosAplanados,i);
		encontrado = false;
		for(int j = 0; i<objetivosGlobales->elements_count && !(encontrado);j++){
			pokemonObjetivo = list_get(objetivosGlobales,j);
			if(strcmp(unPokemon,pokemonObjetivo)==0){
				encontrado = true;
				list_remove_and_destroy_element(objetivosGlobales,j,punteroAFree);
			}
		}
	}
	list_destroy_and_destroy_elements(poseidosAplanados,punteroAFree);
}

void mandarGets(){
	int socket;
	for(int i=0;i<objetivosGlobales->elements_count;i++){
		socket = crear_conexion(info->configuracion.ip,info->configuracion.puerto);
		if(socket < 0){
			printf("error de conexion");
		}
	}
}


void buscarPokemones(int* id){
	infoInicializacion configuracion = info->configuracion;
	trainer* yo = list_get(info->configuracion.entrenadores,*id);
	t_list* misPokemones = yo->poseidos;
	t_list* misObjetivos = yo->objetivos;
	yo->identificador = *id;
	int cantMax = misObjetivos->elements_count;
	int cantActual = misPokemones->elements_count;
	char* unPokemon;

	nombreEstado estadoAnterior;
	bool soyLibre;
	bool estoyEnDeadlock = 0;
	wait(mandarGet);
	while(noTermine()){
    if(!modoDeadlock){
    	wait(esteEsMio);
    	if(configuracion.algoritmo=="FIFO"||"SJFSD"){
    		if(!estoyEnDeadlock){
    			switch(yo->estadoActual){
    				case 0:
    					yo->estadoActual = READY;
    					estadoAnterior = NEW;
    					break;
    				case 3:
    					yo->estadoActual = READY;
    					estadoAnterior = BLOCKED;
    					break;
    			}
    			wait(miTurno);
    			yo->estadoActual = EXEC;
    			estadoAnterior = READY;
    			meMuevo();
    			intentoCapturar();
    			yo->estadoActual = BLOCKED;
    			estadoAnterior = EXEC;
    			replanifico(); // SIGNAL
    			wait(mensajecaught);
    				if(verificarCatch()){
    					unPokemon = obtenerPokemon();
    					list_add(misPokemones,unPokemon);
    						if(cantActual == cantMax){
    						if(tengoLosQueNecesito()){
    							yo->estadoActual = TERM;
    							estadoAnterior = BLOCKED;
    							desalojoMisRecursos();
    						}else{
    							estoyEnDeadlock = 1;
    					}
    				}
    			}
    		}
    	}
    }
}
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

trainer* crearYAsignarHilo(trainer* unEntrenador,int i){
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
	objetivosGlobales = list_duplicate(objetivos);
	objetivosGlobales = list_fold(objetivosGlobales,NULL,punteroACombinarListas);
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

