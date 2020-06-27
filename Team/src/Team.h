/*
 * Team.h
 *
 *  Created on: 30 abr. 2020
 *      Author: utnso
 */
#include "libbase.h"
#include <semaphore.h>
#include "commons/collections/queue.h"
#ifndef TEAM_SRC_TEAM_H_
#define TEAM_SRC_TEAM_H_

typedef struct{
	int posicion_X;
	int posicion_Y;
}t_posicion;

typedef struct{
	uint32_t posicionX;
	uint32_t posicionY;
}t_posicion32;

typedef struct{
	char* nombre;
	uint32_t posicionX;
	uint32_t posicionY;
	bool atrapadoConExito;
}PokemonEnMapa;

typedef enum{
	NEW = 0,
	READY = 1,
	EXEC = 2,
	BLOCKED = 3,
	TERM = 4
}nombreEstado;

typedef struct{
	int identificadorEstado;
	nombreEstado tipo;
	t_list* entrenadores;
}Estado;

typedef struct
{
	bool estoyLibre;
	int identificador;
	nombreEstado estadoActual;
	pthread_t* hilo;
	t_list* objetivos;
	t_list* poseidos;
	t_posicion posicion;
	pthread_mutex_t* miMutex;
	sem_t* permisoParaMoverme;
	sem_t* resultadoCatchPokemon;
	PokemonEnMapa* objetivoActual;
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

typedef struct{
	uint32_t size_nombre;
	char* nombre;
}t_nombre_pokemon;

typedef struct{
	t_nombre_pokemon nombre;
	uint32_t id_mensaje;
}Get_Pokemon;

typedef struct{
	t_nombre_pokemon nombre;
	t_posicion posicion;
	uint32_t id_mensaje;
}Catch_Pokemon;

typedef struct{
	t_nombre_pokemon nombre;
	t_posicion32 posicion;
}Appeared_Pokemon;

typedef struct{
	t_nombre_pokemon nombre;
	t_list* posiciones;
}Localized_Pokemon;



t_list* armarEntrenadores(infoInicializacion);

trainer* crearYAsignarHilo(trainer* , int);

void buscarPokemones(int*);

t_list* armarEntrenadores(infoInicializacion configuracion);

t_buffer* serializarGetPokemon(Get_Pokemon pokemon);

void reconectar();

void limpiarObjetivosCumplidos();

void asignarObjetivosGlobales(infoInicializacion);

bool termine(int);

nombreEstado cambiarDesdeAEstado (nombreEstado estadoViejo, nombreEstado estadoNuevo, int idEntrenador, char* mensajeCambio);

trainer* decidirFIFO();

trainer* obtenerSiguienteEntrenador();

void planificadorAReady();

void asignarPokemonAMejorEntrenador(PokemonEnMapa* pokemon);

trainer* entrenadorMasCercano(trainer* entrenadorA, trainer* entrenadorB, PokemonEnMapa* pokemon);

uint32_t valorAbsoluto(uint32_t numero);

bool estaLibre(trainer* unEntrenador);

bool hayEntrenadoresLibres();

void mandarGets();

t_buffer* serializarCatchPokemon(Catch_Pokemon pokemon);

int posicionEncontrada (PokemonEnMapa* nuevoPokemon, char* criterio);

void recibirAppeared();

void recibirLocalized();

#endif /* TEAM_SRC_TEAM_H_ */
