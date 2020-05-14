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
	int posicion_X;
	int posicion_Y;
}t_posicion;

typedef struct
{
	int identificador;
	pthread_t* hilo;
	t_list* objetivos;
	t_list* poseidos;
	t_posicion posicion;
} trainer;

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

t_list* armarEntrenadores(infoInicializacion);

trainer* crearYAsignarHilo(trainer* unEntrenador);

void* buscarPokemones(void*);

t_list* armarEntrenadores(infoInicializacion configuracion);

void asignarObjetivosGlobales(infoInicializacion);

#endif /* TEAM_SRC_TEAM_H_ */
