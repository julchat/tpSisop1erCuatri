
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
t_list* pokemonesQueFaltanAceptar;
t_list* pokemonesCapturados;
t_list* pokemonesObtenidos;
t_list* especiesAceptadas;
Estado new;
Estado ready;
Estado exec;
Estado blocked;
Estado term;
bool modoDeadlock = 0;
GodStruct* info;
t_list* entrenadores;
t_list* mensajesGet;
t_list* mensajesCatch;
t_queue* nuevosPokemones;
t_queue* pokemonesPorAsignar;
pthread_t hiloAppeared;
pthread_t hiloLocalized;
pthread_t hiloCaught;
pthread_t hiloReconexion;
bool reconectando = 0;
pthread_mutex_t* syncPokemones;
pthread_mutex_t* mutexSocketGlobal;
pthread_mutex_t* mutexListaCatch;
pthread_mutex_t* mutexColaMapa;
sem_t* getsMandados;
sem_t* semNuevosPokemones;
t_list* pokemonesEnMapa;
int socketCatchPokemon;



int main(){
	new.tipo = NEW;
	ready.tipo = READY;
	exec.tipo = EXEC;
	blocked.tipo = BLOCKED;
	term.tipo = TERM;
	sem_init(getsMandados,0,0);
	sem_init(semNuevosPokemones,0,0);
	pthread_mutex_init(syncPokemones,NULL);
	pthread_mutex_init(mutexSocketGlobal,NULL);
	pthread_mutex_init(mutexListaCatch,NULL);
	pthread_mutex_init(mutexColaMapa,NULL);
	entrenadores = list_create();
	pokemonesEnMapa = list_create();
	pokemonesCapturados = list_create();
	nuevosPokemones = queue_create();
	pokemonesPorAsignar = queue_create();
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
	pokemonesQueFaltanAceptar = list_duplicate(objetivosGlobales);
	pthread_create(&hiloAppeared,0,recibirAppeared,NULL);
	pthread_create(&hiloLocalized,0,recibirLocalized,NULL);
	pthread_create(&hiloCaught,0,recibirCaught,NULL);
	mandarGets();
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
		encontrado = 0;
		for(int j = 0; i<objetivosGlobales->elements_count && !(encontrado);j++){
			pokemonObjetivo = list_get(objetivosGlobales,j);
			if(strcmp(unPokemon,pokemonObjetivo)==0){
				encontrado = 1;
				list_remove_and_destroy_element(objetivosGlobales,j,punteroAFree);
			}
		}
	}
	pokemonesObtenidos = list_duplicate(poseidosAplanados);
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
			log_error(info->logger, "Ha fallado la conexion con el broker, por comportamiento default no hay instancias para el pokemon %s ", list_get(objetivosGlobales,i));
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
			liberar_conexion(socket);
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

t_buffer* serializarCatchPokemon(Catch_Pokemon pokemon){
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = pokemon.nombre.size_nombre + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);
	void* stream = malloc(buffer->size);
	int offset = 0;
	memcpy(stream + offset, &pokemon.nombre.size_nombre, sizeof(uint32_t));
	offset = offset + sizeof(uint32_t);
	memcpy(stream + offset, pokemon.nombre.nombre, strlen(pokemon.nombre.nombre)+1);
	offset = offset + strlen(pokemon.nombre.nombre)+1;
	memcpy(stream + offset, &pokemon.posicion.posicion_X, sizeof(uint32_t));
	offset = offset + sizeof(uint32_t);
	memcpy(stream + offset, &pokemon.posicion.posicion_Y, sizeof(uint32_t));
	offset = offset + sizeof(uint32_t);

	buffer->stream= stream;
	return buffer;
}


void buscarPokemones(int* id){
	sem_wait(getsMandados);
	infoInicializacion configuracion = info->configuracion;
	trainer* yo = list_get(info->configuracion.entrenadores,*id);
	sem_init(yo->permisoParaMoverme,0,0);
	sem_init(yo->resultadoCatchPokemon,0,0);
	pthread_mutex_init(yo->miMutex,NULL);
	t_list* misPokemones = yo->poseidos;
	t_list* misObjetivos = yo->objetivos;
	yo->identificador = *id;
	yo->objetivoActual = NULL;
	int cantMax = misObjetivos->elements_count;
	int cantActual = misPokemones->elements_count;
	nombreEstado estadoAnterior;
	bool estoyEnDeadlock = 0;
	bool atrapadoBien = 0;
	trainer* entrenadorProximo;


	void catchPokemon(PokemonEnMapa* pokemonACapturar){
		mensajesCatch = list_create();
		t_paquete* paquete = malloc(sizeof(t_paquete));
		t_buffer* buffer;
		Catch_Pokemon pokemon;
			if(socketCatchPokemon < 0){
				log_error(info->logger, "Ha fallado la conexion con el broker, por comportamiento default el pokemon %s %d %d sera capturado ",
						pokemonACapturar->nombre, pokemonACapturar->posicionX, pokemonACapturar->posicionY);
				if(!(reconectando)){
					reconectando = 1;
					pthread_create(&hiloReconexion,NULL,reconectar,NULL);
				}
				atrapadoBien = 1;
				sem_post(yo->resultadoCatchPokemon);

			}
			else{
				pthread_mutex_lock(mutexSocketGlobal);

				pokemon.nombre.nombre = pokemonACapturar->nombre;
				pokemon.nombre.size_nombre = strlen(pokemon.nombre.nombre) + 1;
				pokemon.posicion.posicion_X = pokemonACapturar->posicionX;
				pokemon.posicion.posicion_Y = pokemonACapturar->posicionY;
				buffer = serializarCatchPokemon(pokemon);

				paquete->codigo_operacion = 6;
				paquete->buffer = buffer;

				void* a_enviar = malloc(sizeof(uint8_t) + sizeof(uint32_t) + buffer->size );
				int offset = 0;
				memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(uint8_t));
				offset = offset + sizeof(uint8_t);
				memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
				offset = offset + sizeof(uint32_t);
				memcpy(a_enviar + offset, paquete->buffer->stream, buffer->size);
				offset = offset + buffer->size;

				send(socketCatchPokemon, a_enviar, buffer->size + sizeof(uint8_t) + sizeof(uint32_t), 0);
				free(a_enviar);
				free(paquete->buffer->stream);
				free(paquete->buffer);
				free(paquete);

				recv(socketCatchPokemon, &(pokemon.id_mensaje), sizeof(uint32_t) , 0);

				pthread_mutex_unlock(mutexSocketGlobal);
				pthread_mutex_lock(mutexListaCatch);
				list_add(mensajesCatch, &(pokemon));
				pthread_mutex_unlock(mutexListaCatch);
			}
		}

	while(yo->estadoActual != TERM){
    if(!modoDeadlock){
    	yo->estoyLibre = 1;
    	sem_wait(yo->permisoParaMoverme);
    	if(strcmp(configuracion.algoritmo,"FIFO")==0||strcmp(configuracion.algoritmo,"SJFSD")==0){
    		if(!estoyEnDeadlock){
    			switch(yo->estadoActual){
    				case 0:
    					yo->estadoActual = cambiarDesdeAEstado(NEW, READY, *id, "Entrenador %d pasa de NEW a READY porque fue elegido para atrapar a un pokemon");
    					estadoAnterior = NEW;
    					break;
    				case 3:
    					yo->estadoActual = cambiarDesdeAEstado(BLOCKED, READY, *id, "Entrenador libre %d pasa de BLOCKED a READY porque fue elegido para atrapar a un pokemon");
    					estadoAnterior = BLOCKED;
    					break;
    				default:
    				break;
    			}
    			pthread_mutex_lock(yo->miMutex);
    			yo->estadoActual = cambiarDesdeAEstado(READY, EXEC, *id, "Entrenador %d fue elegido por el algoritmo de planificacion para ejecutar");
    			estadoAnterior = READY;

    			while(yo->posicion.posicion_X != yo->objetivoActual->posicionX){
    				if(yo->posicion.posicion_X < yo->objetivoActual->posicionX){
    					sleep(info->configuracion.retardo);
    					yo->posicion.posicion_X++;
    					log_info(info->logger, "Entrenador %d se movio hacia %d %d ", yo->identificador, yo->posicion.posicion_X, yo->posicion.posicion_Y);
    				}
    				if(yo->posicion.posicion_X > yo->objetivoActual->posicionX){
    					sleep(info->configuracion.retardo);
    					yo->posicion.posicion_X--;
    					log_info(info->logger, "Entrenador %d se movio hacia %d %d ", yo->identificador, yo->posicion.posicion_X, yo->posicion.posicion_Y);
    				}
    			}

    			while(yo->posicion.posicion_Y != yo->objetivoActual->posicionY){
    			    if(yo->posicion.posicion_Y < yo->objetivoActual->posicionY){
    			    	sleep(info->configuracion.retardo);
    			    	yo->posicion.posicion_Y++;
    					log_info(info->logger, "Entrenador %d se movio hacia %d %d ", yo->identificador, yo->posicion.posicion_X, yo->posicion.posicion_Y);
    			    	}
    			    if(yo->posicion.posicion_Y > yo->objetivoActual->posicionY){
    			    	sleep(info->configuracion.retardo);
    			    	yo->posicion.posicion_Y--;
    					log_info(info->logger, "Entrenador %d se movio hacia %d %d ", yo->identificador, yo->posicion.posicion_X, yo->posicion.posicion_Y);
    			    	}
    			    }

    			catchPokemon(yo->objetivoActual);
    			log_info(info->logger, "Entrenador %d intenta capturar en %d %d al pokemon %s", yo->identificador,
    					yo->objetivoActual->posicionX, yo->objetivoActual->posicionY, yo->objetivoActual->nombre);
    			yo->estadoActual = cambiarDesdeAEstado(EXEC,BLOCKED, *id, "Entrenador %d es bloqueado a espera de conseguir el pokemon capturado");
    			estadoAnterior = EXEC;
    			entrenadorProximo = obtenerSiguienteEntrenador();
    			pthread_mutex_unlock(entrenadorProximo->miMutex);
    			sem_wait(yo->resultadoCatchPokemon);
    				if(atrapadoBien){
    					log_info(info->logger, "Entrenador %d capturo con exito en %d %d al pokemon %s", yo->identificador,
    					   yo->objetivoActual->posicionX, yo->objetivoActual->posicionY, yo->objetivoActual->nombre);
    					list_add(misPokemones, yo->objetivoActual->nombre);
    					list_add(pokemonesCapturados, yo->objetivoActual);
    					list_add(pokemonesObtenidos, yo->objetivoActual);
    					quitarDeObjetivosGlobales(yo->objetivoActual->nombre);
    				}else{
    					log_info(info->logger, "Entrenador %d fracaso en capturar en %d %d al pokemon %s", yo->identificador,
    					    yo->objetivoActual->posicionX, yo->objetivoActual->posicionY, yo->objetivoActual->nombre);
    						pthread_mutex_lock(mutexColaMapa);
    						queue_push(pokemonesPorAsignar,yo->objetivoActual);
    						pthread_mutex_unlock(mutexColaMapa);
    				}
    				atrapadoBien = 0;
    				if(cantActual == cantMax){
    					if(termine(*id)){
    						estadoAnterior = BLOCKED;
    						yo->estadoActual = cambiarDesdeAEstado(BLOCKED, TERM, *id, "Entrenador %d consiguio los pokemones que buscaba");
    					}else{
    						estoyEnDeadlock = 1;
    					}
    				}else{
    					pthread_mutex_lock(mutexColaMapa);
    					yo->objetivoActual=queue_pop(pokemonesPorAsignar);
    					if(yo->objetivoActual==NULL){
    						yo->estoyLibre=1;
    					}
    					pthread_mutex_unlock(mutexColaMapa);
    				}
    			}
    		}
    	}else{


    	}
    }
	list_destroy(yo->poseidos);
	list_destroy(yo->objetivos);
	list_destroy(misPokemones);
	list_destroy(misObjetivos);
	}




void reconectar(){
log_error(info->logger, "error al conectar con el broker, reintentando en %d segundos", info->configuracion.contimer);
usleep(info->configuracion.contimer);
int socket;
while (true){

	socket = crear_conexion(info->configuracion.ip,info->configuracion.puerto);
	if(socket<0){
		log_error(info->logger, "error al reconectar con el broker, reintentando en %d segundos", info->configuracion.contimer);
		sleep(info->configuracion.contimer);
	}else{
		reconectando = 0;
		socketCatchPokemon = socket;
		log_info(info->logger,"reconexion con el broker con exito");
		exit(0);
		}
}
}

void planificadorAReady(){
	PokemonEnMapa* pokemonNuevo;
	PokemonEnMapa* pokemonPorAsignar;
	pokemonesPorAsignar = queue_create();
	while(!(list_is_empty(objetivosGlobales))){
		sem_wait(semNuevosPokemones);
		pthread_mutex_lock(syncPokemones);
		pokemonNuevo = queue_pop(nuevosPokemones);
		pthread_mutex_unlock(syncPokemones);
		pthread_mutex_lock(mutexColaMapa);
		queue_push(pokemonesPorAsignar,pokemonNuevo);
		if(hayEntrenadoresLibres()){
			for(int i = 0; i<queue_size(pokemonesPorAsignar); i++){
				pokemonPorAsignar = queue_pop(pokemonesPorAsignar);
				asignarPokemonAMejorEntrenador(pokemonPorAsignar);
			}
		}
		pthread_mutex_unlock(mutexColaMapa);

	}
}

void asignarPokemonAMejorEntrenador(PokemonEnMapa* pokemon){
	t_list* entrenadores = info->configuracion.entrenadores;
	t_list* entrenadoresLibres;
	trainer* mejorEntrenador;
	entrenadoresLibres = list_filter(entrenadores,estaLibre);
	mejorEntrenador = list_get(entrenadoresLibres,0);
	for(int i = 1; i<entrenadoresLibres->elements_count;i++){
		mejorEntrenador = entrenadorMasCercano(mejorEntrenador, list_get(entrenadoresLibres,i), pokemon);
	}
	list_destroy(entrenadoresLibres);
	mejorEntrenador->estoyLibre = 0;
	mejorEntrenador->objetivoActual = pokemon;
	sem_post(mejorEntrenador->permisoParaMoverme);
}

trainer* entrenadorMasCercano(trainer* entrenadorA, trainer* entrenadorB, PokemonEnMapa* pokemon){
	uint32_t distanciaA;
	uint32_t distanciaB;
	distanciaA = valorAbsoluto(entrenadorA->posicion.posicion_X - pokemon->posicionX) +
			valorAbsoluto(entrenadorA->posicion.posicion_Y - pokemon->posicionY);
	distanciaB = valorAbsoluto(entrenadorB->posicion.posicion_X - pokemon->posicionX) +
			valorAbsoluto(entrenadorB->posicion.posicion_Y - pokemon->posicionY);

	return distanciaA<=distanciaB?entrenadorA:entrenadorB;
	}

uint32_t valorAbsoluto(uint32_t numero){
	return numero<=0?numero*(-1):numero;
}

bool estaLibre(trainer* unEntrenador){
	return unEntrenador->estoyLibre;
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
		unEntrenador->estoyLibre = 0;
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
	log_info(info->logger,"El entrenador %d es creado y entra a estado NEW",id);
	return unEntrenador;
}

void asignarObjetivosGlobales(infoInicializacion configuracion){
	t_list* (*punteroACombinarListas)(t_list*,t_list*);
	punteroACombinarListas = &combinarListas;
	t_list* objetivos = configuracion.objetivos;
	objetivosGlobales = list_duplicate(objetivos);
	objetivosGlobales = list_fold(objetivosGlobales,NULL,punteroACombinarListas);
}

trainer* obtenerSiguienteEntrenador(){
	trainer* sigEntr;
	if(strcmp(info->configuracion.algoritmo,"FIFO")==0){
		sigEntr = decidirFIFO();
		return sigEntr;
	}
	return NULL;
}

bool hayEntrenadoresLibres(){
	t_list* entrenadores = info->configuracion.entrenadores;
	t_list* entrenadoresLibres;
	entrenadoresLibres = list_filter(entrenadores,estaLibre);
	return !(list_is_empty(entrenadoresLibres));
}

trainer* decidirFIFO(){
	trainer* aEjecutar;
 t_list* entrenadores = ready.entrenadores;
 if(ready.entrenadores->elements_count!=0){
	 aEjecutar = list_get(entrenadores,0);
	 return aEjecutar;
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

nombreEstado cambiarDesdeAEstado (nombreEstado estadoViejo, nombreEstado estadoNuevo, int idEntrenador, char* mensaje){
	Estado* estadoPostaViejo;
	Estado* estadoPostaNuevo;
	trainer* unEntrenador;

	switch(estadoViejo){
	case 0:
		estadoPostaViejo = &new;
		break;
	case 1:
		estadoPostaViejo = &ready;
		break;
	case 2:
		estadoPostaViejo = &exec;
		break;
	case 3:
		estadoPostaViejo = &blocked;
		break;
	case 4:
		estadoPostaViejo = &term;
		break;

	}

	switch(estadoNuevo){
	case 0:
		estadoPostaNuevo = &new;
		break;
	case 1:
		estadoPostaNuevo = &ready;
		break;
	case 2:
		estadoPostaNuevo = &exec;
		break;
	case 3:
		estadoPostaNuevo = &blocked;
		break;
	case 4:
		estadoPostaNuevo = &term;
		break;

	}

	for (int i = 0; i<estadoPostaViejo->entrenadores->elements_count; i++){
		unEntrenador = list_get(estadoPostaViejo->entrenadores,i);
		if(unEntrenador->identificador == idEntrenador){
			list_remove(estadoPostaViejo->entrenadores,i);
			list_add(estadoPostaNuevo->entrenadores,unEntrenador);
			log_info(info->logger, mensaje, idEntrenador);
			}
	}

	return estadoNuevo;
}

void recibirAppeared(){
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(uint8_t);
	t_paquete* paquete = malloc(sizeof(t_paquete));
	void* stream = malloc(buffer->size);
	void* a_enviar;
	int offset = 0;
	uint32_t listaASuscribirme = 11;
	int socketAppeared;
	Appeared_Pokemon* unPokemon;
	PokemonEnMapa* nuevoPokemon;
	int posicionPokemon;

	socketAppeared = crear_conexion(info->configuracion.ip,info->configuracion.puerto);
	memcpy(stream + offset, &listaASuscribirme, sizeof(uint8_t));
	offset += sizeof(uint8_t);
	buffer->stream = stream;

	paquete->codigo_operacion = 14;
	paquete->buffer = buffer;
	a_enviar = malloc(sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint8_t));
	offset = 0;
	memcpy(a_enviar + offset, &paquete->codigo_operacion, sizeof(uint8_t));
	offset += sizeof(uint8_t);
	memcpy(a_enviar + offset, &paquete->buffer->size, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);
	offset += paquete->buffer->size;

	send(socketAppeared, a_enviar, sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint8_t), 0);
	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	while(true){
		paquete = malloc(sizeof(t_paquete));
		paquete->buffer = malloc(sizeof(t_buffer));
		recv(socketAppeared,&paquete->codigo_operacion,sizeof(uint8_t),0);
		recv(socketAppeared,&paquete->buffer->size, sizeof(uint32_t),0);
		paquete->buffer->stream = malloc(paquete->buffer->size);
		recv(socketAppeared,paquete->buffer->stream, paquete->buffer->size,0);

		unPokemon = deserializar_appeared_pokemon(paquete->buffer);
		nuevoPokemon = malloc(sizeof(PokemonEnMapa));
		nuevoPokemon->nombre = unPokemon->nombre.nombre;
		nuevoPokemon->posicionX = unPokemon->posicion.posicionX;
		nuevoPokemon->posicionY = unPokemon->posicion.posicionY;
		nuevoPokemon->atrapadoConExito = 0;

		free(paquete->buffer->stream);
		free(paquete->buffer);
		free(paquete);
		free(unPokemon);

		pthread_mutex_lock(syncPokemones);
		posicionPokemon = posicionEncontrada(nuevoPokemon, "appeared");
		if(posicionPokemon>=0){
		list_remove(pokemonesQueFaltanAceptar,posicionPokemon);
		queue_push(nuevosPokemones, nuevoPokemon);
		pthread_mutex_unlock(syncPokemones);
		}else{
		pthread_mutex_unlock(syncPokemones);
		free(nuevoPokemon->nombre);
		}
	}
}

int posicionEncontrada (PokemonEnMapa* nuevoPokemon, char* criterio){
	bool encontrado = 0;
	int i = 0;
	char* unPoke;
	char* otroPoke = nuevoPokemon->nombre;

	if(strcmp(criterio,"appeared")==0){
		for(int i = 0; i<pokemonesQueFaltanAceptar->elements_count && !encontrado;i++){
			unPoke = list_get(pokemonesQueFaltanAceptar,i);
			if(strcmp(unPoke, otroPoke)==0){
				encontrado = 1;
			}
		}
		return encontrado?i-1:-1;
	}else{
		return 0;
	}
}

void recibirLocalized(){
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(uint8_t);
	t_paquete* paquete = malloc(sizeof(t_paquete));
	void* stream = malloc(buffer->size);
	void* a_enviar;
	int offset = 0;
	uint32_t listaASuscribirme = 9;
	int socketLocalized;
	Localized_Pokemon* grupoDePokemon;
	PokemonEnMapa* nuevoPokemon;
	int posicionPokemon;

	socketLocalized = crear_conexion(info->configuracion.ip,info->configuracion.puerto);
	memcpy(stream + offset, &listaASuscribirme, sizeof(uint8_t));
	offset += sizeof(uint8_t);
	buffer->stream = stream;

	paquete->codigo_operacion = 14;
	paquete->buffer = buffer;
	a_enviar = malloc(sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint8_t));
	offset = 0;
	memcpy(a_enviar + offset, &paquete->codigo_operacion, sizeof(uint8_t));
	offset += sizeof(uint8_t);
	memcpy(a_enviar + offset, &paquete->buffer->size, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);
	offset += paquete->buffer->size;

	send(socketLocalized, a_enviar, sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint8_t), 0);
	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	while(true){
		paquete = malloc(sizeof(t_paquete));
		paquete->buffer = malloc(sizeof(t_buffer));
		recv(socketLocalized,&paquete->codigo_operacion,sizeof(uint8_t),0);
		recv(socketLocalized,&paquete->buffer->size, sizeof(uint32_t),0);
		paquete->buffer->stream = malloc(paquete->buffer->size);
		recv(socketLocalized,paquete->buffer->stream, paquete->buffer->size,0);

		grupoDePokemon = deserializar_localized_pokemon(paquete->buffer);
		/*nuevoPokemon = malloc(sizeof(PokemonEnMapa));
		nuevoPokemon->nombre = grupoDePokemon->nombre.nombre;
		nuevoPokemon->posicionX = grupoDePokemon->posicion.posicionX;
		nuevoPokemon->posicionY = grupoDePokemon->posicion.posicionY;
		nuevoPokemon->atrapadoConExito = 0;

		free(grupoDePokemon);
		free(paquete->buffer->stream);
		free(paquete->buffer);
		free(paquete);

		pthread_mutex_lock(syncPokemones);
		posicionPokemon = aceptoPokemon(nuevoPokemon, "localized");
		if(posicionPokemon>=0){
		list_remove(pokemonesQueFaltanAceptar,posicionPokemon);
		queue_push(nuevosPokemones, nuevoPokemon);
		pthread_mutex_unlock(syncPokemones);
		}else{
		pthread_mutex_unlock(syncPokemones);
		free(nuevoPokemon->nombre);
		}*/
		//falta corregir esto
	}
}
