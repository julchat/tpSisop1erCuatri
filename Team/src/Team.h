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
	int identificadorEstado;
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
	pthread_mutex_t miSemaforo;
} trainer;

typedef struct
{
	t_list* posicionesX;
	t_list* posicionesY;
	t_list* poseidos;
	t_list* poseidosAplanados;
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

typedef struct{
	uint32_t size_nombre;
	char* nombre;
}t_nombre_pokemon;

typedef struct{
	t_nombre_pokemon nombre;
	uint32_t id_mensaje;
}Get_Pokemon;


t_list* armarEntrenadores(infoInicializacion);

trainer* crearYAsignarHilo(trainer* , int);

void buscarPokemones(int*);

t_list* armarEntrenadores(infoInicializacion configuracion);

t_buffer* serializarGetPokemon(Get_Pokemon pokemon);

void reconectar();

void asignarObjetivosGlobales(infoInicializacion);

#endif /* TEAM_SRC_TEAM_H_ */
