/*
 * utils.c
 *
 *  Created on: 1 may. 2020
 *      Author: utnso
 */

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "Team.h"
#include <pthread.h>
#include "libbase.h"


infoInicializacion obtenerConfiguracion(char* configpath){
infoInicializacion inicializacion;
t_config* configuracion = config_create(configpath);
char** read_array;
t_list* listPokemonesDeUnEntrenador = list_create();
t_list* listObjetivosDeUnEntrenador = list_create();
t_list* listCoordenadas = list_create();
int* unaCoordenadaEntera;
uint32_t* coordenadaConvertida;



//PRIMERA LÍNEA
read_array = config_get_array_value(configuracion,"POSICIONES_ENTRENADORES");
void transformarAEnteroYagregarAListCoordenadas(char* unaCoordenada){
	*unaCoordenadaEntera = atoi(unaCoordenada);
	*coordenadaConvertida = *unaCoordenadaEntera;
	list_add(listCoordenadas,coordenadaConvertida);
}
void armarListaCoordenadas(char* coordenadasDeUnEntrenador){
	if(coordenadasDeUnEntrenador != NULL){
		char** coordenadas = string_split(coordenadasDeUnEntrenador, "|");
		string_iterate_lines(coordenadas,transformarAEnteroYagregarAListCoordenadas);
	}
}

string_iterate_lines(read_array,armarListaCoordenadas);
for(int i=0; i<listCoordenadas->elements_count;i++){
	if(esPar(i)){
	list_add(inicializacion.posicionesX, list_get(listCoordenadas,i));
	}else{
	list_add(inicializacion.posicionesY, list_get(listCoordenadas,i));
	}
}
list_destroy(listCoordenadas);

//SEGUNDA LÍNEA
read_array = config_get_array_value(configuracion,"POKEMON_ENTRENADORES");
void agregarAListPokemonesDeUnEntrenador(char* unPokemon){
	list_add(listPokemonesDeUnEntrenador, unPokemon);
}
void armarListaPoseidos(char* pokemonesDeUnEntrenador){
	if(pokemonesDeUnEntrenador != NULL){
		char** pokes = string_split(pokemonesDeUnEntrenador,"|");
		string_iterate_lines(pokes,agregarAListPokemonesDeUnEntrenador);
		list_add(inicializacion.poseidos, listPokemonesDeUnEntrenador);
		list_destroy(listPokemonesDeUnEntrenador);
		list_create(listPokemonesDeUnEntrenador);
	}
}
string_iterate_lines(read_array, armarListaPoseidos);
list_destroy(listPokemonesDeUnEntrenador);

//TERCERA LÍNEA
read_array = config_get_array_value(configuracion,"OBJETIVOS_ENTRENADORES");
void agregarAListObjetivosDeUnEntrenador(char* unPokemon){
	list_add(listObjetivosDeUnEntrenador, unPokemon);
}
void armarListaObjetivos(char* objetivosDeUnEntrenador){
	if(objetivosDeUnEntrenador != NULL){
		char** pokes = string_split(objetivosDeUnEntrenador,"|");
		string_iterate_lines(pokes,agregarAListObjetivosDeUnEntrenador);
		list_add(inicializacion.objetivos, listObjetivosDeUnEntrenador);
		list_destroy(listObjetivosDeUnEntrenador);
		list_create(listObjetivosDeUnEntrenador);
	}
}
string_iterate_lines(read_array, armarListaObjetivos);
list_destroy(listObjetivosDeUnEntrenador);

//DEMÁS LÍNEAS
inicializacion.contimer = config_get_int_value(configuracion,"TIEMPO_RECONEXION");
inicializacion.retardo = config_get_int_value(configuracion, "RETARDO_CICLO_CPU");
inicializacion.algoritmo= config_get_string_value(configuracion,"ALGORITMO_PLANIFICACION");
inicializacion.quantum = config_get_int_value(configuracion, "QUANTUM");
inicializacion.alpha = config_get_int_value(configuracion, "ALPHA");
inicializacion.estim = config_get_int_value(configuracion, "ESTIMACION_INICIAL");
inicializacion.ip = config_get_string_value(configuracion, "IP_BROKER");
inicializacion.puerto = config_get_string_value(configuracion, "PUERTO_BROKER");
inicializacion.logpath = config_get_string_value(configuracion, "LOG_FILE");
return inicializacion;
}








void inicializarListas(infoInicializacion* configuracion, Estado* n, Estado* r, Estado* e, Estado* b, Estado* t){
	configuracion->objetivos = list_create();
	configuracion->posicionesX = list_create();
	configuracion->posicionesY = list_create();
	configuracion->poseidos = list_create();
	n->entrenadores = list_create();
	r->entrenadores = list_create();
	e->entrenadores = list_create();
	b->entrenadores = list_create();
	t->entrenadores = list_create();
	n->cantHilos=0;
	r->cantHilos=0;
	e->cantHilos= 0;
	b->cantHilos = 0;
	t->cantHilos = 0;
}



void destruir_sublistas_y_sus_elementos(void* element){
	list_destroy_and_destroy_elements(element,free);
}



 bool existeDichoEntrenador(int indice, t_list* obj, t_list* pos, t_list* posX, t_list* posY){
	 bool existe;
	 existe = (list_get(obj,indice)!=NULL)&&(list_get(pos,indice)!=NULL)&&(list_get(posX,indice)!=NULL)&&(list_get(posY,indice)!=NULL);
	 return existe;
 }

 t_list* combinarListas(t_list* primeraLista, t_list* segundaLista){
 int i = 0;
 void* elemento;
 while(list_get(segundaLista,i)!=NULL){
 	elemento = list_get(segundaLista,i);
 	list_add(primeraLista,elemento);
 	i++;
 }
 //destroy segundaLista?
 return primeraLista;
 }


bool esPar(uint32_t numero){
	return (numero%2)==0;
}
