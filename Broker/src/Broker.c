#include <stdio.h>
#include <stdlib.h>
#include "lib.h"


int main(void) {


void recbir_mensaje(int socket_cliente){

	op_code codigo_mensaje;
	recv(socket_cliente,&codigo_mensaje,sizeof(codigo_mensaje),0);

	switch(codigo_mensaje){
		case 0: New_Pokemon mensaje_new = descerializar_new_pokemon(socket_cliente);
				//encolar(punterotanto,mensaje_new)
				break;

		case 1: Localized_Pokemon mensaje_localized = descerializar_localized_pokemon(socket_cliente);
				//encolar(punterotanto,mensaje_localized)
			    break;

		case 2: Get_Pokemon mensaje_get = descerializar_get_pokemon(socket_cliente);
				//encolar(punterotanto,mensaje_get)
				break;

		case 3: Appeared_Pokemon mensaje_appeared = descerializar_appeared_pokemon(socket_cliente);
				//encolar(punterotanto,mensaje_appeared)
				break;

		case 4: Catch_Pokemon mensaje_catch = descerializar_catch_pokemon(socket_cliente);
				//encolar(punterotanto,mensaje_catch)
				break;

		case 5: Caught_Pokemon mensaje_caught = descerializar_caught_pokemon(socket_cliente);
				//encolar(punterotanto,mensaje_caught)
				break;
	}
}


return EXIT_SUCCESS;




}
