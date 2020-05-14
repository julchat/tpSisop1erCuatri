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
	char* buffer = (char*) malloc(sizeof(char));
	*buffer = '\0';
	int posicionX, posicionY;
	t_list* pokemonesObjetivos = list_create();
	t_list* pokemonesPoseidos = list_create();
	t_list* objetivos = list_create();
	t_list* poseidos = list_create();
	t_list* posicionesX = list_create();
	t_list* posicionesY = list_create();
	void (*punteroAFree)(void*);
	punteroAFree = &free;
	void (*punteroADestruirListaYSublistas)(void*);
	punteroADestruirListaYSublistas = &(destruir_sublistas_y_sus_elementos);
	char c= '\0';

	// PRIMERA LÍNEA

	while(c!='=' && (!feof(configfile))){
		c = fgetc(configfile);
	}
		while(c!=']' &&  c!='=' && !feof(configfile)){
			c = fgetc(configfile);
			agregarCaracter(&buffer, c);
			posicionX = atoi(buffer);
			list_add(posicionesX,&posicionX);
			buffer = vaciarBuffer(buffer);
			if(fgetc(configfile)!='|'){
				printf("error en parsing posiciones |");
			}
			c = fgetc(configfile);
			agregarCaracter(&buffer,c);
			posicionY = atoi(buffer);
			list_add(posicionesY,&posicionY);
			buffer = vaciarBuffer(buffer);
			fgetc(configfile);
		}

	// SEGUNDA LÍNEA


	while(c!='=' && (!feof(configfile))){
			c = fgetc(configfile);
		}
	while(c!=']' && !feof(configfile)){
		c = fgetc(configfile);
		while(c!=',' && c!=']'){
			c = fgetc(configfile);
			while(c!='|' && c!= ',' && c!=']'){
				agregarCaracter(&buffer,c);
				c= fgetc(configfile);
			}
		list_add(pokemonesPoseidos,buffer);
		buffer = vaciarBuffer(buffer);
		}
		list_add(poseidos, pokemonesPoseidos);
		list_destroy_and_destroy_elements(pokemonesPoseidos,punteroAFree);
		pokemonesPoseidos = list_create();
	}

	// TERCERA LÍNEA

	while(c!='=' && (!feof(configfile))){
				c = fgetc(configfile);
			}
		while(c!=']' && !feof(configfile)){
			c = fgetc(configfile);
			while(c!=',' && c!=']'){
				c = fgetc(configfile);
				while(c!='|' && c!= ',' && c!=']'){
					agregarCaracter(&buffer,c);
					c= fgetc(configfile);
				}
			list_add(pokemonesObjetivos,buffer);
			buffer = vaciarBuffer(buffer);
			}
			list_add(objetivos, pokemonesObjetivos);
			list_destroy_and_destroy_elements(pokemonesObjetivos,punteroAFree);
			pokemonesObjetivos = list_create();
		}

		//CUARTA LÍNEA

	while(c!='=' && (!feof(configfile))){
	c = fgetc(configfile);
	}
	fscanf(configfile,"%i",&(config.contimer));

		// QUINTA LÍNEA

	while(c!='=' && (!feof(configfile))){
	c = fgetc(configfile);
	}
	fscanf(configfile,"%i",&(config.retardo));

		// SEXTA LÍNEA

	while(c!='=' && (!feof(configfile))){
	c = fgetc(configfile);
	}
	fscanf(configfile,"%s",(config.algoritmo));

		// SÉPTIMA LÍNEA

	while(c!='=' && (!feof(configfile))){
	c = fgetc(configfile);
	}
	fscanf(configfile,"%i",&(config.quantum));

		// OCTAVA LÍNEA

	while(c!='=' && (!feof(configfile))){
	c = fgetc(configfile);
	}
	fscanf(configfile,"%i",&(config.estim));

		// NOVENA LÍNEA

	while(c!='=' && (!feof(configfile))){
	c = fgetc(configfile);
	}
	fscanf(configfile,"%s",(config.ip));

		// DÉCIMA LÍNEA

	while(c!='=' && (!feof(configfile))){
	c = fgetc(configfile);
	}
	fscanf(configfile,"%s",(config.puerto));

		// UNDÉCIMA LÍNEA

	while(c!='=' && (!feof(configfile))){
	c = fgetc(configfile);
	}
	fscanf(configfile,"%s",(config.logpath));

		// ASIGNACIONES RESTANTES
	!feof(configfile)?printf("parser todo bien"):printf("ripeo el parser");
	config.posicionesX = posicionesX;
	config.posicionesY = posicionesY;
	config.poseidos = poseidos;
	config.objetivos = objetivos;
	list_destroy(pokemonesObjetivos); //Estas dos están vacías
	list_destroy(pokemonesPoseidos); // por eso no uso la otra destroy
	list_destroy(posicionesX);
	list_destroy(posicionesY);
	list_destroy_and_destroy_elements(poseidos,punteroADestruirListaYSublistas);
	list_destroy_and_destroy_elements(objetivos,punteroADestruirListaYSublistas);
	return config;
}

void inicializarListas(infoInicializacion* configuracion){
	configuracion->objetivos = list_create();
	configuracion->posicionesX = list_create();
	configuracion->posicionesY = list_create();
	configuracion->poseidos = list_create();
}



void agregarCaracter(char** buffer, char c){
	size_t len = strlen(*buffer);
	free(*buffer);
	char* buffer2 = malloc(len+1+1);
	buffer2[len] = c;
	buffer2[len+1] = '\0';
	*buffer = buffer2;
}

char* vaciarBuffer (char* bufferViejo){
	free(bufferViejo);
	char* bufferNuevo = (char*) malloc(sizeof(char));
	*bufferNuevo = '\0';
	return bufferNuevo;
}

void destruir_sublistas_y_sus_elementos(void* element){
	list_destroy_and_destroy_elements(element,free);
}



 bool existeDichoEntrenador(int indice, t_list* obj, t_list* pos, t_list* posX, t_list* posY){
	 bool existe;
	 existe = (list_get(obj,indice)!=NULL)&&(list_get(pos,indice)!=NULL)&&(list_get(posX,indice)!=NULL)&&(list_get(posY,indice)!=NULL);
	 return existe;
 }

