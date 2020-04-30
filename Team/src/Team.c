
#include "lib.h"
#include <stdio.h>
#include <stdlib.h>
#include "Team.h"
especie* objetivosGlobales;
pthread_t* entrenadores;

int main(){
	FILE* configfile;
	configfile = fopen("configFile.txt","r");
	int cantEntrenadores;
	infoInicializacion configuracion;
	entrenadores = (pthread_t*)malloc(cantEntrenadores*sizeOf(entrenadores));
	configuracion = obtenerConfiguracion(&configfile);
	return 0;

}
