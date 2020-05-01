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



infoInicializacion obtenerConfiguracion(FILE* configfile){
	infoInicializacion configcargada;
//TODO: Meter toda la info de configFile.txt en configcargada, ordenando bien las posiciones //
	return configcargada;
}

int tamanioListaEntrenadores(listaentrenadores* entrenadores){
	int contador = 0;
	if(!listaVacia(entrenadores)){
		contador++;
		while(entrenadores->sig != NULL){
			contador++;
			entrenadores = entrenadores->sig;
			}
	}
	return contador;
}

bool listaVacia(void* lista){
	return lista==NULL?1:0;
}


