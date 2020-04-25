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




	printf("Hola Mundo");
	//return EXIT_SUCCESS;
}



void crearHilos() {


	hilo_servidor= 0;
	hilo_consola= 0;

	pthread_create(&hilo_servidor, NULL, (void*) servidor, NULL);
	pthread_create(&hilo_consola, NULL, (void*) consola, NULL);

	pthread_join(hilo_servidor, NULL);
	pthread_join(hilo_consola, NULL);

}
