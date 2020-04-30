/*
 * Team.h
 *
 *  Created on: 30 abr. 2020
 *      Author: utnso
 */

#ifndef TEAM_SRC_TEAM_H_
#define TEAM_SRC_TEAM_H_

typedef struct{
	especie* objetivos;
	especie* capturados;
	double ubicacionX;
	double ubicacionY;
} entrenador;

typedef struct{
	char* nombre;
} especie;

typedef struct{
	double* coordenadasX;
	double* coordenadasY;

} infoInicializacion;


#endif /* TEAM_SRC_TEAM_H_ */
