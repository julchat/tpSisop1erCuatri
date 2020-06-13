/*
 * utils.h
 *
 *  Created on: 1 may. 2020
 *      Author: utnso
 */

#ifndef TEAM_SRC_UTILS_H_
#define TEAM_SRC_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include "Team.h"
#include <pthread.h>
#include "libbase.h"
#include "utils.c"
trainer crearEntrenador();
void inicializarListas(infoInicializacion*, Estado*, Estado*, Estado*, Estado*, Estado*);
infoInicializacion obtenerConfiguracion(char*);
void destruir_sublistas_y_sus_elementos(void* element);
t_list* combinarListas(t_list* primeraLista, t_list* segundaLista);
bool existeDichoEntrenador(int indice, t_list* obj, t_list* pos, t_list* posX, t_list* posY);
bool esNumeroPar(int);
#endif /* TEAM_SRC_UTILS_H_ */
