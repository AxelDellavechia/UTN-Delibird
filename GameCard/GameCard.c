/*
 * Broker.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */
#include "GameCard.h"
#include "Inicializacion.h"
#include "MetodosGC.h"


int main(){//int argc, char **argv) {
	iniciar_log();
	leer_configFile(CONFIG_PATH);

//chequear que el punto de montaje del archivo esté creado, sino crearlo
//YA ESTAN CREADOS LOS BLOQUES y bitmap VER SI DESPUES PUEDO HACER ALGO DETECTANDO SI EXISTEN O NO, O INGRESAR UN COMANDO POR CONSOLA
		//crearBloques();
	//creacionDeArchivoBitmap(PuntoMontaje->BITMAP,config_MetaData->cantidad_bloques);


	crearBitmap();
	//leerFiles();

	/* CASO DE PRUEBA */
	cola_NEW_POKEMON* Pokemon;
	Pokemon = reservarMemoria(sizeof(cola_NEW_POKEMON));

	Pokemon->nombre_pokemon=string_duplicate("Charmander");
	Pokemon->id_mensaje = 1;
	Pokemon->cantidad = 10;
	Pokemon->posicion_x = 6;
	Pokemon->posicion_y = 5;

	NewPokemon(Pokemon);
//	crearHilos();

	//return EXIT_SUCCESS;
}



