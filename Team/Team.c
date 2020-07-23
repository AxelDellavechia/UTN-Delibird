#include <stdio.h>
#include <stdlib.h>
#include "Team.h"

int main(void) {
	//crearLogger("Team.log",LOG_PATH_INTERNO);
	//crearLogger("CatedraTeam.log",LOG_PATH);
	//fdBroker = nuevoSocket();
	//inicializar_semaforos();
	crearEstructuras();
	iniciar_log();
	leerArchivoDeConfiguracion(RUTA_CONFIG_TEAM);
	iniciar_logCatedra();
	inicializar_semaforos();
	obtenerEntrenadores();

	for (int i = 0; i < list_size(configFile->posicionEntrenadores); i++){
		pthread_mutex_lock(&mutexColaReady);
		entrenadorPokemon * entrenador = list_get(colaReady,i);
		pthread_mutex_unlock(&mutexColaReady);
		//sem_wait(&entrenador->semafoContador);
		sem_post(&semEntrenadores);
		pthread_create(&hilo, NULL, (void*) thread_Entrenador,entrenador);
		//list_add(misHilos,process_get_thread_id());
		//log_info(logger,"Cree un hilo para el entrenador %d y tiene ID %d",i,hilo=pthread_self());
	}
	crearHilos();
	return EXIT_SUCCESS;
}
