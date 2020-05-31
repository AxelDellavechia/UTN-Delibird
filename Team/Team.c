#include <stdio.h>
#include <stdlib.h>
#include "Team.h"

int main(void) {
	//crearLogger("Team.log",LOG_PATH_INTERNO);
	//crearLogger("CatedraTeam.log",LOG_PATH);
	//fdBroker = nuevoSocket();
	//inicializar_semaforos();
	iniciar_log();
	//inicializar_semaforos();
	crearEstructuras();
	leerArchivoDeConfiguracion(RUTA_CONFIG_MEM,logger);
	/*iniciar_logCatedra();
	crearHilos();
	obtenerEntrenadores(logger);
	localizarPokemones();*/
	entrenadorPokemon* entrenador = reservarMemoria(sizeof(entrenadorPokemon));
	entrenador->pokemonesAtrapados = list_create();
	entrenador->pokemonesObjetivo = list_create();
	entrenador->idEntrenador = 0;
	entrenador->posicion_x = 1;
	entrenador->posicion_y = 2;
	entrenador->ciclosEnCPU = 0;
	entrenador->proximaAccion = "AtraparPokemon Pikachu 3 4";
	realizarAccion(entrenador, 0);

	//./gameboy BROKER NEW_POKEMON [POKEMON] [POSX] [POSY] [CANTIDAD]

	//./gameboy BROKER APPEARED_POKEMON [POKEMON] [POSX] [POSY] [ID_MENSAJE]

	//./gameboy BROKER CATCH_POKEMON [POKEMON] [POSX] [POSY]

	//./gameboy BROKER CAUGHT_POKEMON [ID_MENSAJE] [OK/FAIL]

	//./gameboy BROKER GET_POKEMON [POKEMON]

	//./gameboy TEAM APPEARED_POKEMON [POKEMON] [POSX] [POSY]

	//./gameboy GAMECARD NEW_POKEMON [POKEMON] [POSX] [POSY] [CANTIDAD]

	//./gameboy GAMECARD CATCH_POKEMON [POKEMON] [POSX] [POSY]

	//./gameboy GAMECARD GET_POKEMON [POKEMON]

	//./gameboy SUSCRIPTOR [COLA_DE_MENSAJES] [TIEMPO]

	/*
	  	Logs obligatorios
	 	Conexión a cualquier proceso.
		Suscripción a una cola de mensajes.
		Llegada de un nuevo mensaje a una cola de mensajes.
		Envío de un mensaje a un suscriptor específico.
	 */
	return EXIT_SUCCESS;
}


