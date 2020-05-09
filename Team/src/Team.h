/*
 * Team.h
 *
 *  Created on: 30 abr. 2020
 *      Author: utnso
 */
#include "libbase.h"
#ifndef TEAM_SRC_TEAM_H_
#define TEAM_SRC_TEAM_H_

typedef struct{
	char* nombre;
} especie;

/*typedef struct{
	especie pokemon;
	listapokemon* sig;
} listapokemon;*/
typedef struct{
	uint32_t posicion_X;
	uint32_t posicion_Y;
}t_posicion;

typedef struct
{
	pthread_t hilo;
	t_list* objetivos;
	t_list* poseidos;
	t_posicion posicion;
} entrenador;



/*typedef struct{
	entrenador trainer;
	listaentrenadores* sig;
} listaentrenadores;*/




typedef struct
{
	t_list* posicionesX;
	t_list* posicionesY;
	t_list* poseidos;
	t_list* objetivos;
	t_list* entrenadores;
	int contimer;
	int retardo;
	char* algoritmo;
	int quantum;
	int estim;
	char* ip;
	char* puerto;
	char* logpath;
} infoInicializacion;


//typedef struct{
	//pthread_t hiloentrenador;
	//pthread_nodo* sig;
//}pthread_nodo;

t_list* armarEntrenadores(infoInicializacion);

void buscarPokemones(entrenador);

void asignarObjetivosGlobales(t_list*);

#endif /* TEAM_SRC_TEAM_H_ */
