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
	infoInicializacion config;
	char* buffer = {'\0'};
	int posicionX, posicionY;
	t_list* pokemonesObjetivos = list_create();
	t_list* pokemonesPoseidos = list_create();
	t_list* objetivos = list_create();
	t_list* poseidos = list_create();
	t_list* posicionesX = list_create();
	t_list* posicionesY = list_create();
	entrenador trainer = crearEntrenador();
	char c;
	while(!feof(configfile)){
	while(c!='=' && c!=']'){
		c = fgetc(configfile);
		while(c!=']'){
			c = fgetc(configfile);
			agregarCaracter(&buffer, c);
			posicionX = atoi(buffer);
			list_add(posicionesX,&posicionX);
			char* buffer = {'\0'};
			if(fgetc(configfile)!='|'){
				printf("error en parsing posiciones |");
			}
			c = fgetc(configfile);
			agregarCaracter(&buffer,c);
			posicionY = atoi(buffer);
			list_add(posicionesY,&posicionY);
			char* buffer = {'\0'};
			fgetc(configfile);
		}

	}

	while(c!='=' && c!=']'){
			c = fgetc(configfile);
			while(c!=']'){
				while(c!='|'){
					buffer = NULL;
					buffer = (char*)malloc(sizeof(char));

				}
				c = fgetc(configfile);
				buffer = &c;
				posicionX = atoi(buffer);
				list_add(posicionesX,&posicionX);
				if(fgetc(configfile)!='|'){
					printf("error en parsing posiciones |");
				}
				c = fgetc(configfile);
				buffer = &c;
				posicionY = atoi(buffer);
				list_add(posicionesY,&posicionY);
				fgetc(configfile);
			}

		}


//TODO: Meter toda la info de configFile.txt en configcargada, ordenando bien las posiciones //
	}return config;
}
void inicializarListas(infoInicializacion* configuracion){
	configuracion->objetivos = list_create();
	configuracion->posiciones = list_create();
	configuracion->poseidos = list_create();
}

entrenador crearEntrenador(){
	entrenador trainer;
	trainer.objetivos = list_create();
	trainer.poseidos = list_create();
	return trainer;
}


void agregarCaracter(char** buffer, char c){
	size_t len = strlen(*buffer);
	char* buffer2 = malloc(len+1+1);
	buffer2[len] = c;
	buffer2[len+1] = '\0';
	*buffer = buffer2;
}
