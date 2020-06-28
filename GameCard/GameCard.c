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

	inicializar_semaforos();
	crearBloques();  //Verifico si los bloques existen y si coinciden con la cantidad de bloques del archivo de configuración.
	creacionDeArchivoBitmap(PuntoMontaje->BITMAP,config_MetaData->cantidad_bloques); //Verifico la existencia del Bitmap.
	crearBitmap(); //Cargo el contenigo del bitmap.
	loadPokemons();
	crearHilos();

	/*

	// ------ CASO DE PRUEBA NEW POKEMON-----------------------------
	cola_NEW_POKEMON* Pokemon;
	Pokemon = reservarMemoria(sizeof(cola_NEW_POKEMON));

	Pokemon->nombre_pokemon=string_duplicate("Pikachu");
	Pokemon->id_mensaje = 1;
	Pokemon->cantidad = 10;
	Pokemon->posicion_x = 8;
	Pokemon->posicion_y = 5;

	NewPokemon(Pokemon);


	cola_NEW_POKEMON* Pokemon2;
	Pokemon2 = reservarMemoria(sizeof(cola_NEW_POKEMON));

	Pokemon2->nombre_pokemon=string_duplicate("Charmander");
	Pokemon2->id_mensaje = 1;
	Pokemon2->cantidad = 10;
	Pokemon2->posicion_x = 71;
	Pokemon2->posicion_y = 5;

	NewPokemon(Pokemon2);
//-----------------------------------------------------------
	cola_GET_POKEMON* Pokemon2;
	Pokemon2 = reservarMemoria(sizeof(cola_GET_POKEMON));

	Pokemon2->nombre_pokemon=string_duplicate("Charmander");
	Pokemon2->id_mensaje = 1;
	Pokemon2->tamanio_nombre=string_length(Pokemon2->nombre_pokemon);

	cola_LOCALIZED_POKEMON* locPokemon;
	locPokemon = reservarMemoria(sizeof(cola_LOCALIZED_POKEMON));
	GetPokemon(Pokemon2, locPokemon);

	for(int i = 0;i<list_size(locPokemon->lista_posiciones);i++){
		t_positions* pos = malloc (sizeof(t_positions));
		pos = list_get(locPokemon->lista_posiciones,i);
		printf("x: %i  y:%i  cant:%i\n",pos->Pos_x, pos->Pos_y, pos->Cantidad);
	}*/



	//return EXIT_SUCCESS;
}



