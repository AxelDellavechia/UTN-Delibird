/*
 ============================================================================
 Name        : GameBoy.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "GameBoy.h"


int main(void) {

	crearLogger("CatedraGB.log",LOG_PATH);
	crearLogger("GameBoy.log",LOG_PATH_INTERNO);

	configGB = reservarMemoria(sizeof(archivoConfigGB));

	fdBroker = nuevoSocket();

	fdTeam = nuevoSocket() ;

	fdGameCard = nuevoSocket() ;

	inicializar_semaforos();

	log_info(logger, "Por setear los valores del archivo de configuracion");

	leerArchivoDeConfiguracion(RUTA_CONFIG_MEM,logger);


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
