
#include <stdio.h>
#include <stdlib.h>
#include "Team.h"
#include <pthread.h>
#include "libbase.h"
#include "utils.h"
#include "semaphore.h"
#include "commons/collections/list.h"
#include <string.h>
t_list* objetivosGlobales;
Estado new;
Estado ready;
Estado exec;
Estado blocked;
Estado term;
bool modoDeadlock = 0;
GodStruct* info;
t_list* entrenadores;
t_list* mensajesGet;
pthread_t hiloAppeared;
pthread_t hiloLocalised;
pthread_t hiloCaught;
pthread_t hiloReconexion;
bool reconectando = 0;
sem_t* getsMandados;
t_list* pokemonesEnMapa;


int main(){
	new.tipo = NEW;
	ready.tipo = READY;
	exec.tipo = EXEC;
	blocked.tipo = BLOCKED;
	term.tipo = TERM;
	sem_init(getsMandados,0,0);
	entrenadores = list_create();
	pokemonesEnMapa = list_create();
	t_log* loggerTeam;
	infoInicializacion configuracion;
	inicializarListas(&configuracion,&new,&ready,&exec,&blocked,&term);
	configuracion = obtenerConfiguracion("configFile.txt");
	loggerTeam = iniciar_logger_de_nivel_minimo(LOG_LEVEL_INFO, configuracion.logpath);
	asignarObjetivosGlobales(configuracion);
	info->logger = loggerTeam;
	info->configuracion = configuracion;
	entrenadores = armarEntrenadores(configuracion);
	info->configuracion.entrenadores = entrenadores;
	configuracion.entrenadores = entrenadores;
	limpiarObjetivosCumplidos();
}

void limpiarObjetivosCumplidos(){
	char* unPokemon;
	char* pokemonObjetivo;
	t_list* (*punteroACombinarListas)(t_list*,t_list*);
	punteroACombinarListas = &combinarListas;
	t_list* poseidosAplanados = list_duplicate(info->configuracion.poseidos);
	bool encontrado;
	void (*punteroAFree)(void*);
	punteroAFree = &free;

	poseidosAplanados = list_fold(poseidosAplanados,NULL,punteroACombinarListas);
	for(int i=0; i<poseidosAplanados->elements_count;i++){
		unPokemon = list_get(poseidosAplanados,i);
		encontrado = false;
		for(int j = 0; i<objetivosGlobales->elements_count && !(encontrado);j++){
			pokemonObjetivo = list_get(objetivosGlobales,j);
			if(strcmp(unPokemon,pokemonObjetivo)==0){
				encontrado = true;
				list_remove_and_destroy_element(objetivosGlobales,j,punteroAFree);
			}
		}
	}
	list_destroy_and_destroy_elements(poseidosAplanados,punteroAFree);
}

void mandarGets(){
	mensajesGet = list_create();
	int socket;
	t_paquete* paquete = malloc(sizeof(t_paquete));
	t_buffer* buffer;
	Get_Pokemon pokemon;
	for(int i=0;i<objetivosGlobales->elements_count;i++){
		socket = crear_conexion(info->configuracion.ip,info->configuracion.puerto);
		if(socket < 0){
			printf("error de conexion, reconectando en %d segundos", info->configuracion.contimer);
			if(!(reconectando)){
				reconectando = 1;
				pthread_create(&hiloReconexion,NULL,reconectar,NULL);
			}
		}
		else{
			pokemon.nombre.nombre = list_get(objetivosGlobales, i);
			pokemon.nombre.size_nombre = strlen(pokemon.nombre.nombre) + 1;
			buffer = serializarGetPokemon(pokemon);

			paquete->codigo_operacion = 4;
			paquete->buffer = buffer;

			void* a_enviar = malloc(sizeof(uint8_t) + sizeof(uint32_t) + buffer->size );
			int offset = 0;
			memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(uint8_t));
			offset = offset + sizeof(uint8_t);
			memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
			offset = offset + sizeof(uint32_t);
			memcpy(a_enviar + offset, paquete->buffer->stream, buffer->size);
			offset = offset + buffer->size;

			send(socket, a_enviar, buffer->size + sizeof(uint8_t) + sizeof(uint32_t), 0);
			free(a_enviar);
			free(paquete->buffer->stream);
			free(paquete->buffer);
			free(paquete);

			recv(socket, &(pokemon.id_mensaje), sizeof(uint32_t) , 0);
			list_add(mensajesGet, &(pokemon));
		}
	}
	for(int u = 0; u<info->configuracion.entrenadores->elements_count; u++){
		sem_post(getsMandados);
	}
}

t_buffer* serializarGetPokemon(Get_Pokemon pokemon){
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = pokemon.nombre.size_nombre + sizeof(uint32_t);
	void* stream = malloc(buffer->size);
	int offset = 0;
	memcpy(stream + offset, &pokemon.nombre.size_nombre, sizeof(uint32_t));
	offset = offset + sizeof(uint32_t);
	memcpy(stream + offset, pokemon.nombre.nombre, strlen(pokemon.nombre.nombre)+1);
	offset = offset + strlen(pokemon.nombre.nombre)+1;

	buffer->stream= stream;
	return buffer;
}


void buscarPokemones(int* id){
	sem_wait(getsMandados);
	infoInicializacion configuracion = info->configuracion;
	trainer* yo = list_get(info->configuracion.entrenadores,*id);
	sem_init(yo->permisoParaMoverme,0,0);
	pthread_mutex_init(yo->miMutex,NULL);
	t_list* misPokemones = yo->poseidos;
	t_list* misObjetivos = yo->objetivos;
	yo->identificador = *id;
	int cantMax = misObjetivos->elements_count;
	int cantActual = misPokemones->elements_count;
	char* unPokemon;
	nombreEstado estadoAnterior;
	bool estoyEnDeadlock = 0;
	while(!(termine(*id))){
    if(!modoDeadlock){
    	sem_wait(yo->permisoParaMoverme);
    	if(strcmp(configuracion.algoritmo,"FIFO")==0||strcmp(configuracion.algoritmo,"SJFSD")==0){
    		if(!estoyEnDeadlock){
    			switch(yo->estadoActual){
    				case 0:
    					yo->estadoActual = READY;
    					estadoAnterior = NEW;
    					break;
    				case 3:
    					yo->estadoActual = READY;
    					estadoAnterior = BLOCKED;
    					break;
    				default:
    				break;
    			}
    			pthread_mutex_lock(yo->miMutex);
    			yo->estadoActual = EXEC;
    			estadoAnterior = READY;
    			meMuevo();
    			intentoCapturar();
    			yo->estadoActual = BLOCKED;
    			estadoAnterior = EXEC;
    			replanifico(); // SIGNAL
    			wait(mensajecaught);
    				if(verificarCatch()){
    					unPokemon = obtenerPokemon();
    					list_add(misPokemones,unPokemon);
    						if(cantActual == cantMax){
    						if(termine(*id)){
    							yo->estadoActual = TERM;
    							estadoAnterior = BLOCKED;
    							desalojoMisRecursos();
    						}else{
    							estoyEnDeadlock = 1;
    					}
    				}
    			}
    		}
    	}
    }
}
}

void reconectar(){
usleep(info->configuracion.contimer);
int socket;
while (true){

	socket = crear_conexion(info->configuracion.ip,info->configuracion.puerto);
	if(socket<0){
		printf("error de conexion, reconectando en %d segundos", info->configuracion.contimer);
		usleep(info->configuracion.contimer);
	}else{
		reconectando = 0;
		exit(0);
		}
}
}

t_list* armarEntrenadores(infoInicializacion configuracion){
	t_list* entrenadores = list_create();
	t_list* objetivosTodos = configuracion.objetivos;
	t_list* poseidosTodos = configuracion.poseidos;
	t_list* posicionesXTodos = configuracion.posicionesX;
	t_list* posicionesYTodos  = configuracion.posicionesY;
	trainer* unEntrenador;
	t_posicion* unaPosicion;
	int* unaPosicionX;
	int* unaPosicionY;
	for(int i = 0;
	existeDichoEntrenador(i,objetivosTodos,poseidosTodos, posicionesXTodos,posicionesYTodos);
			i++){
		unEntrenador->identificador = i;
		unaPosicionX = list_get(posicionesXTodos,i);
		unaPosicionY = list_get(posicionesYTodos,i);
		unaPosicion->posicion_X = *unaPosicionX;
		unaPosicion->posicion_Y = *unaPosicionY;
		unEntrenador->posicion = *unaPosicion;
		unEntrenador->objetivos = list_get(objetivosTodos,i);
		unEntrenador->poseidos = list_get(poseidosTodos,i);
		unEntrenador = crearYAsignarHilo(unEntrenador,i);
		list_add(entrenadores,unEntrenador);
	}
		return entrenadores;

}

trainer* crearYAsignarHilo(trainer* unEntrenador,int i){
	int* id;
	*id = i;
	pthread_t hiloEntrenador;
	pthread_create(&hiloEntrenador,NULL,buscarPokemones,id);
	unEntrenador->hilo = &hiloEntrenador;
	unEntrenador->estadoActual = NEW;
	list_add(new.entrenadores,unEntrenador);
	new.cantHilos++;
	return unEntrenador;
}

void asignarObjetivosGlobales(infoInicializacion configuracion){
	t_list* (*punteroACombinarListas)(t_list*,t_list*);
	punteroACombinarListas = &combinarListas;
	t_list* objetivos = configuracion.objetivos;
	objetivosGlobales = list_duplicate(objetivos);
	objetivosGlobales = list_fold(objetivosGlobales,NULL,punteroACombinarListas);
}

trainer decidirFIFO(){
	trainer* aEjecutar;
 t_list* entrenadores = ready.entrenadores;
 if(ready.cantHilos !=0){
	 aEjecutar = list_get(entrenadores,0);
	 return *aEjecutar;
 }
 else{
	 printf("no hay entrenadores para decidir cual ejecuta");
	 exit(-50);
 }
}

bool termine(int id){
	t_list* listaEntrenadores = info->configuracion.entrenadores;
	trainer* yo;
	yo = list_get(listaEntrenadores, id);
	bool encontrado = 0;
	char* pokemonABuscar;
	for(int i = 0; i<yo->objetivos->elements_count;i++){
		pokemonABuscar = list_get(yo->objetivos,i);
		for(int j = 0; i<yo->poseidos->elements_count && encontrado == 0; j++){
			if(strcmp(pokemonABuscar,list_get(yo->poseidos,j))==0){
				encontrado = 1;
			}
		}
		if(encontrado == 0){
			return 0;
		}
		encontrado = 0;
	}
	return 1;
}
