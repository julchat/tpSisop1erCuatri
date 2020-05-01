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


int tamanioListaEntrenadores(listaentrenadores* entrenadores);
bool listaVacia(void* lista);
infoInicializacion obtenerConfiguracion(FILE* configfile);


#endif /* TEAM_SRC_UTILS_H_ */
