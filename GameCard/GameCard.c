/*
 * Broker.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */
#include "GameCard.h"
#include "Inicializacion.h"


int main(){//int argc, char **argv) {
	iniciar_log();
	leer_configFile(CONFIG_PATH);


//YA ESTAN CREADOS LOS BLOQUES y bitmap VER SI DESPUES PUEDO HACER ALGO DETECTANDO SI EXISTEN O NO, O INGRESAR UN COMANDO POR CONSOLA
		//crearBloques();
	//creacionDeArchivoBitmap(PuntoMontaje->BITMAP,config_MetaData->cantidad_bloques);
	crearBitmap();
	leerFiles();
//	crearHilos();

	//return EXIT_SUCCESS;
}



