#include <stdio.h>
#include <stdlib.h>
#include "Team.h"

int main(void) {
	//crearLogger("Team.log",LOG_PATH_INTERNO);
	//crearLogger("CatedraTeam.log",LOG_PATH);
	//fdBroker = nuevoSocket();
	//inicializar_semaforos();
	inicializar_semaforos();
	crearEstructuras();
	leerArchivoDeConfiguracion(RUTA_CONFIG_TEAM);
	iniciar_log();
	iniciar_logCatedra();

	obtenerEntrenadores();
	getPokemon();

	for (int i = 0; i < list_size(configFile->posicionEntrenadores); i++){
			entrenadorPokemon * entrenador = list_get(colaNew,i) ;
			//sem_wait(&entrenador->semafoContador);
			pthread_create(&hilo, NULL, (void*) thread_Entrenador,entrenador);
			//list_add(misHilos,process_get_thread_id());
			//log_info(logger,"Cree un hilo para el entrenador %d y tiene ID %d",i,hilo=pthread_self());
		}


	crearHilos();
	//obtenerEntrenadores(logger);
	//localizarPokemones();

	/*entrenadorPokemon* entrenador = reservarMemoria(sizeof(entrenadorPokemon));
	entrenador->pokemonesAtrapados = list_create();
	entrenador->pokemonesObjetivo = list_create();
	entrenador->idEntrenador = 0;
	entrenador->posicion_x = 1;
	entrenador->posicion_y = 2;
	entrenador->ciclosEnCPU = 0;
	entrenador->proximaAccion = "HacerIntercambio 1 3 4 Squirtle Pikachu";
	realizarAccion(entrenador, 0);*/
	return EXIT_SUCCESS;
}
