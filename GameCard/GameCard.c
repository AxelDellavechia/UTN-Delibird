/*
 * Broker.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */
#include "GameCard.h"
#include "Inicializacion.h"


int main(){//int argc, char **argv) {

	leer_configFile();

	iniciar_log();
	crearBloques();

	crearHilos();


	printf("Hola Mundo");
	//return EXIT_SUCCESS;
}



