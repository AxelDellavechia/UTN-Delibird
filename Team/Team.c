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
	iniciar_log();
	leerArchivoDeConfiguracion(RUTA_CONFIG_TEAM);
	iniciar_logCatedra();
	obtenerEntrenadores();
	for (int i = 0; i < list_size(configFile->posicionEntrenadores); i++){
		entrenadorPokemon * entrenador = list_get(colaNew,i);
		//sem_wait(&entrenador->semafoContador);
		pthread_create(&hilo, NULL, (void*) thread_Entrenador,entrenador);
		//list_add(misHilos,process_get_thread_id());
		//log_info(logger,"Cree un hilo para el entrenador %d y tiene ID %d",i,hilo=pthread_self());
	}
	crearHilos();
	return EXIT_SUCCESS;
}
