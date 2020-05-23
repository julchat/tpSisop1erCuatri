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

typedef enum{
	NEW,
	READY,
	EXEC,
	BLOCKED,
	TERM,
}nombreEstado;

typedef struct{
	nombreEstado tipo;
	t_list* entrenadores;
	int cantHilos;
}Estado;

typedef struct
{
	int identificador;
	nombreEstado estadoActual;
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
	int alpha;
	int estim;
	char* ip;
	char* puerto;
	char* logpath;
} infoInicializacion;

typedef struct{
	infoInicializacion configuracion;
	t_log* logger;
}GodStruct;

int planificar(t_list*,t_log*,t_log*,infoInicializacion);

t_list* armarEntrenadores(infoInicializacion);

trainer* crearYAsignarHilo(trainer* , int);

void buscarPokemones(int*);

t_list* armarEntrenadores(infoInicializacion configuracion);

void asignarObjetivosGlobales(infoInicializacion);

#endif /* TEAM_SRC_TEAM_H_ */
