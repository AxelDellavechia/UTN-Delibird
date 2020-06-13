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
	crearBloques();
	creacionDeArchivoBitmap(PuntoMontaje->BITMAP,config_MetaData->cantidad_bloques);


	crearBitmap();

	//leerFiles();

	// CASO DE PRUEBA
	cola_NEW_POKEMON* Pokemon;
	Pokemon = reservarMemoria(sizeof(cola_NEW_POKEMON));

	Pokemon->nombre_pokemon=string_duplicate("Charmander");
	Pokemon->id_mensaje = 1;
	Pokemon->cantidad = 10;
	Pokemon->posicion_x = 7;
	Pokemon->posicion_y = 5;

	NewPokemon(Pokemon);

	Pokemon->posicion_x = 8;
	Pokemon->posicion_y = 5;

	NewPokemon(Pokemon);


	Pokemon->posicion_x = 9;
	Pokemon->posicion_y = 5;

	NewPokemon(Pokemon);

	Pokemon->posicion_x = 10;
	Pokemon->posicion_y = 5;

	NewPokemon(Pokemon);

	Pokemon->posicion_x = 11;
	Pokemon->posicion_y = 5;

	NewPokemon(Pokemon);

	Pokemon->posicion_x = 12;
	Pokemon->posicion_y = 5;

	NewPokemon(Pokemon);

	Pokemon->posicion_x = 13;
	Pokemon->posicion_y = 5;

	NewPokemon(Pokemon);

	Pokemon->posicion_x = 14;
	Pokemon->posicion_y = 5;

	NewPokemon(Pokemon);

	Pokemon->posicion_x = 15;
	Pokemon->posicion_y = 5;

	NewPokemon(Pokemon);

	Pokemon->posicion_x = 16;
	Pokemon->posicion_y = 5;

		NewPokemon(Pokemon);

		Pokemon->posicion_x = 17;
		Pokemon->posicion_y = 5;

		NewPokemon(Pokemon);
		//crearHilos();

	//return EXIT_SUCCESS;
}



