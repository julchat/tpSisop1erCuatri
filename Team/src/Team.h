/*
 * Team.h
 *
 *  Created on: 30 abr. 2020
 *      Author: utnso
 */

#ifndef TEAM_SRC_TEAM_H_
#define TEAM_SRC_TEAM_H_

typedef struct{
	especie pokemon;
	listapokemon* sig;
} listapokemon;

typedef struct{
	listapokemon* capturados;
	listapokemon* objetivos;
	t_posicion posicion;
} entrenador;

typedef struct{
	entrenador trainer;
	listaentrenadores* sig;
} listaentrenadores;

typedef struct{
	char* nombre;
} especie;



typedef struct{
	listaposicion* posiciones;
	listapokemon* poseidos;
	listapokemon* objetivos;
	int contimer;
	int retardo;
	char* algoritmo;
	int quantum;
	int estim;
	char* ip;
	char* puerto;
	char* logpath;
} infoInicializacion;


typedef struct{
	uint32_t posicion_X;
	uint32_t posicion_Y;
}t_posicion;

typedef struct{
	t_posicion posicion;
	listaposicion* sig;
} listaposicion ;

typedef struct{
	pthread_t hiloentrenador;
	pthread_nodo* sig;
}pthread_nodo;

listaentrenadores* armarEntrenadores(infoInicializacion);
void buscarPokemones(entrenador);
void asignarObjetivosGlobales(listaentrenadores* , int );

#endif /* TEAM_SRC_TEAM_H_ */
