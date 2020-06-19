/*
 * operaciones.c
 *
 *  Created on: 27 may. 2020
 *      Author: utnso
 */


#include "Generales.h"

//Si el pokemon que aparecio esta en la lista del objetivo le mando un get al broker

void localizarPokemones() {
	//if (validar_conexion) {
	int existePokemonEnListaAux;
	t_list* listaAux = list_create();
	char* pokemonObjetivo = list_get(objetivoTeam, 0);
	list_add(listaAux, string_duplicate(pokemonObjetivo));
	for(int posicionObjetivo = 1; posicionObjetivo < list_size(objetivoTeam); posicionObjetivo++) {
		existePokemonEnListaAux = FALSE;
		char* pokemonObjetivo = list_get(objetivoTeam, posicionObjetivo);
		for (int posicionListaAux = 0; posicionListaAux < list_size(listaAux); posicionListaAux++) {
			char* elementoListaAux = list_get(listaAux, posicionListaAux);
			if(strcmp(elementoListaAux, pokemonObjetivo) == 0) {
				existePokemonEnListaAux = TRUE;
				break;
			}
		}
		if (existePokemonEnListaAux == FALSE) {
			list_add(listaAux, string_duplicate(pokemonObjetivo));
		}
	}
	for(int posicionLista = 0; posicionLista < list_size(listaAux); posicionLista++) {
		//ENVIAR GET_POKEMON CON CADA ELEMENTO DE LA LISTA
	}
	/*} else {
		void* posicionesPokemon = NULL;
	}*/
}

// RECIBE EL OBJETO CATCH Y UN ENTRENADOR , SE CONECTA ENVIA LA SOLICITUD , ESPERA EL ID DE MS Y LO GUARDA EN SU VARIABLE

void catch_pokemon( cola_CATCH_POKEMON * pokemon , entrenadorPokemon * entrenador) {

	int fdServer = nuevoSocket();

	if ( conectarCon( fdServer ,configFile->ipBroker,configFile->puertoBroker,logger) ) {

	log_info(loggerCatedra,"Me conecte correctamente con el %s IP: %s y Puerto: %d","BROKER",configFile->ipBroker,configFile->puertoBroker);

	handshake_cliente(fdServer,"Team","Broker",logger);

	aplicar_protocolo_enviar(fdServer,CATCH_POKEMON,pokemon);

	entrenador->idMsjEsperado = recibirACK(fdServer);

	list_add(colaBlocked, entrenador);

	}else{
		log_info(loggerCatedra,"No se pudo realizar correctamente la conexión con el %s IP: %s y Puerto: %d","BROKER",configFile->ipBroker,configFile->puertoBroker);
	}
}


void caught_pokemon(cola_CAUGHT_POKEMON * pokemon, entrenadorPokemon * entrenador) {
	if (entrenador->idMsjEsperado == pokemon->id_mensaje){
		//EJECUTAR ALGO ACCIÓN
	}
	//verificarDeadlock();
}

void localized_pokemon(cola_LOCALIZED_POKEMON * pokemonEncontrado) {
	int posicion = 0;
	while (mapaPokemon[posicion].head->next != NULL) {
		_Bool buscarPokemon(posicionPokemon* pokemonEncontrado) {
			return pokemonEncontrado == mapaPokemon[posicion].head->data;
		}
		void* pokemonYaConocido = list_find(mapaPokemon, (void*) buscarPokemon);
	}
}


void seleccionarEntrenadorMasCercano(cola_APPEARED_POKEMON *pokemonAparecido) {
	entrenadorPokemon* proximoEntrenadorAReady;
	int movimientoDeProximoEntrenadorAReady = 0;
	int posicionEntrenador = 0;
	for( posicionEntrenador ; posicionEntrenador < list_size(colaNew); posicionEntrenador++) {
		entrenadorPokemon* entrenador = list_get(colaNew, posicionEntrenador);
		int posicionXEntrenador = entrenador->posicion_x;
		int posicionYEntrenador = entrenador->posicion_y;
		int posicionXPokemon = pokemonAparecido->posicion_x;
		int posicionYPokemon = pokemonAparecido->posicion_y;
		int movimientoEnX = fabs(posicionXEntrenador - posicionXPokemon);
		int movimientoEnY = fabs(posicionYEntrenador - posicionYPokemon);
		int movimientoEntrenador = movimientoEnX + movimientoEnY;
		if((movimientoEntrenador < movimientoDeProximoEntrenadorAReady) || (posicionEntrenador == 0)) {
			movimientoDeProximoEntrenadorAReady = movimientoEntrenador;
			proximoEntrenadorAReady = entrenador;
			break;
		}
	}
	list_remove(colaNew,posicionEntrenador);
	list_add(colaReady, proximoEntrenadorAReady);
	log_info(logger, "Entrenador %i a cola de Ready por ser el más cercano", proximoEntrenadorAReady->idEntrenador);
}

void moverEntrenador(entrenadorPokemon* entrenador, int posicionXDestino, int posicionYDestino) {
	_Bool moverEntrenadorEnX = TRUE;
	_Bool moverEntrenadorEnY = TRUE;
	int posicionXEntrenador = entrenador->posicion_x;
	int posicionYEntrenador = entrenador->posicion_y;
	int ciclosEnCPUEntrenador = entrenador->ciclosEnCPU;
	while (moverEntrenadorEnX == TRUE) {
		if (posicionXEntrenador < posicionXDestino) {
			posicionXEntrenador++;
			ciclosEnCPUEntrenador++;
		} else if (posicionXEntrenador > posicionXDestino) {
			posicionXEntrenador--;
			ciclosEnCPUEntrenador++;
		} else {
			moverEntrenadorEnX = FALSE;
		}
	}
	while (moverEntrenadorEnY == TRUE) {
		if (posicionYEntrenador < posicionYDestino) {
			posicionYEntrenador++;
			ciclosEnCPUEntrenador++;
		} else if (posicionYEntrenador > posicionYDestino) {
			posicionYEntrenador--;
			ciclosEnCPUEntrenador++;
		} else {
			moverEntrenadorEnY = FALSE;
		}
	}
	entrenador->ciclosEnCPU = ciclosEnCPUEntrenador;
	entrenador->posicion_x = posicionXEntrenador;
	entrenador->posicion_y = posicionYEntrenador;
	log_info(logger, "Entrenador %i se movió a la posición %i %i", entrenador->idEntrenador, entrenador->posicion_x, entrenador->posicion_y);
}

void obtenerEntrenadores(t_log *logger) {
	for(int listaEntrenadores = 0; listaEntrenadores < list_size(configFile->posicionEntrenadores); listaEntrenadores++) {
		char* posicionEntrenador = list_get(configFile->posicionEntrenadores, listaEntrenadores);
		char* pokemonesEntrenador = list_get(configFile->pokemonEntrenadores, listaEntrenadores);
		char* objetivosEntrenador = list_get(configFile->objetivosEntrenadores, listaEntrenadores);

		//The C library function char *strtok(char *str, const char *delim)
		//breaks string str into a series of tokens using the delimiter delim.

		//get the first token
		char* arrayCortado = strtok(posicionEntrenador, "|");

		char* posicionXStr = arrayCortado;

		/* walk through other tokens */
		char*posicionYStr = strtok(NULL, "|");

		int posicionX = atoi(posicionXStr);
		int posicionY = atoi(posicionYStr);

		entrenadorPokemon* entrenador = reservarMemoria(sizeof(entrenadorPokemon));

		entrenador->pokemonesAtrapados = list_create();
		entrenador->pokemonesObjetivo = list_create();
		entrenador->idEntrenador = listaEntrenadores;
		entrenador->posicion_x = posicionX;
		entrenador->posicion_y = posicionY;
		entrenador->ciclosEnCPU = 0;
		entrenador->proximaAccion = "AtraparPokemon";
		entrenador->idMsjEsperado = -1 ;
		pthread_mutex_init(& entrenador->semaforMutex,NULL);
		pthread_mutex_lock(& entrenador->semaforMutex);
		char* pokemonAtrapado = strtok(pokemonesEntrenador, "|");
		while (pokemonAtrapado != NULL) {
			list_add(entrenador->pokemonesAtrapados, string_duplicate(pokemonAtrapado));
			pokemonAtrapado = strtok(NULL, "|");
		}
		char* pokemonObjetivo = strtok(objetivosEntrenador, "|");
		while (pokemonObjetivo != NULL) {
			list_add(entrenador->pokemonesObjetivo, string_duplicate(pokemonObjetivo));
			pokemonObjetivo = strtok(NULL, "|");
		}
		list_add(colaNew, entrenador);
		log_info(logger, "Entrenador %i creado y en cola de New", listaEntrenadores);
		list_add_all(objetivoTeam, entrenador->pokemonesObjetivo);
	}

}

void appeared_pokemon(posicionPokemon pokemonAparecido) {
	list_add(mapaPokemon, pokemonAparecido.tipoPokemon);
	//verificarPokemonesNecesarios(pokemonAparecido.tipoPokemon);
}

/*void verificarPokemonesNecesarios(char* especiePokemon) {
	for (int listaPokemon = 0; listaPokemon < sizeof(pokemonesNecesarios); listaPokemon++) {
		t_list* objetivosEntrenadores = list_get(pokemonesNecesarios, listaPokemon);
		for (int objetivoEntrenador = 0; objetivoEntrenador < sizeof(objetivosEntrenadores); objetivoEntrenador++) {
			char* objetivoEntrenadorStr = list_get(objetivosEntrenadores, objetivoEntrenador);
			if (objetivoEntrenadorStr == especiePokemon) {
				list_add(pokemonesRequeridos, especiePokemon);
				break;
			}
		}
		break;
	}
}*/


void realizarIntercambio(entrenadorPokemon* entrenador1, entrenadorPokemon* entrenador2, char* pokemonEntrenador1, char* pokemonEntrenador2) {
	list_add(entrenador1->pokemonesAtrapados, pokemonEntrenador2);
	list_add(entrenador2->pokemonesAtrapados, pokemonEntrenador1);
	quitarPokemonDeAtrapados(entrenador1, pokemonEntrenador1);
	quitarPokemonDeAtrapados(entrenador2, pokemonEntrenador2);
}

void quitarPokemonDeAtrapados(entrenadorPokemon* entrenador, char* pokemon) {
	for (int pokemonPosition = 0; pokemonPosition < list_size(entrenador->pokemonesAtrapados); pokemonPosition++) {
		char* pokemonEntrenador = list_get(entrenador->pokemonesAtrapados, pokemonPosition);
		if (strcmp(pokemonEntrenador, pokemon)) {
			list_remove(entrenador->pokemonesAtrapados, pokemonPosition);
		}
	}
}

void verificarDeadlock(entrenadorPokemon* entrenador) {
	int pokemonEncontrado;
	int entrenadorEnDeadlock;
	t_list* listaObjetivosAuxiliar = list_duplicate(entrenador->pokemonesObjetivo);
	int cantidadPokemonesAtrapados = list_size(entrenador->pokemonesAtrapados);
	int cantidadPokemonesObjetivo = list_size(entrenador->pokemonesObjetivo);
	int cantidadObjetivosAuxiliar = cantidadPokemonesObjetivo;
	if (cantidadPokemonesAtrapados == cantidadPokemonesObjetivo) {
		for (int posicionAtrapados = 0; posicionAtrapados < cantidadPokemonesAtrapados; posicionAtrapados++) {
			pokemonEncontrado = FALSE;
			char* pokemonAtrapado = list_get(entrenador->pokemonesAtrapados, posicionAtrapados);
			for (int posicionObjetivos = 0; posicionObjetivos < cantidadObjetivosAuxiliar; posicionObjetivos++) {
				char* pokemonObjetivo = list_get(listaObjetivosAuxiliar, posicionObjetivos);
				if (strcmp(pokemonAtrapado, pokemonObjetivo)) {
					list_remove(listaObjetivosAuxiliar, posicionObjetivos);
					cantidadObjetivosAuxiliar = list_size(listaObjetivosAuxiliar);
					pokemonEncontrado = TRUE;
					break;
				}
			}
			if (pokemonEncontrado == FALSE) {
				entrenadorEnDeadlock = FALSE;
				for (int posicionEnListaDeadlock = 0; posicionEnListaDeadlock < list_size(entrenadoresEnDeadlock); posicionEnListaDeadlock++) {
					entrenadorPokemon* entrenadorEnListaDeadlock = list_get(entrenadoresEnDeadlock, posicionEnListaDeadlock);
					if (entrenador->idEntrenador == entrenadorEnListaDeadlock->idEntrenador) {
						entrenadorEnDeadlock = TRUE;
						break;
					}
				}
				if (entrenadorEnDeadlock == FALSE) {
					list_add(entrenadoresEnDeadlock, entrenador);
					verificarIntercambios();
				}
			}
		}
		if (cantidadObjetivosAuxiliar == 0) {
			for (int posicionEnListaDeadlock = 0; posicionEnListaDeadlock < list_size(entrenadoresEnDeadlock); posicionEnListaDeadlock++) {
				entrenadorPokemon* entrenadorEnListaDeadlock = list_get(entrenadoresEnDeadlock, posicionEnListaDeadlock);
				if (entrenador->idEntrenador == entrenadorEnListaDeadlock->idEntrenador) {
					list_remove(entrenadoresEnDeadlock, posicionEnListaDeadlock);
					break;
				}
			}
			quitarDeColaBlocked(entrenador);
			list_add(colaExit, entrenador);
		}
	}
}

void quitarDeColaBlocked(entrenadorPokemon* entrenador) {
	for(int posicionEnListaBlocked = 0; posicionEnListaBlocked < list_size(colaBlocked); posicionEnListaBlocked++) {
		entrenadorPokemon* entrenadorBlockeado = list_get(colaBlocked, posicionEnListaBlocked);
		if (entrenadorBlockeado->idEntrenador == entrenador->idEntrenador) {
			list_remove(colaBlocked, posicionEnListaBlocked);
			break;
		}
	}
}

void verificarIntercambios() {
	if (list_size(entrenadoresEnDeadlock) > 1) {
		for (int posicionEnListaDeadlock = 0; posicionEnListaDeadlock < list_size(entrenadoresEnDeadlock); posicionEnListaDeadlock++) {
			entrenadorPokemon* entrenador1 = list_get(entrenadoresEnDeadlock, posicionEnListaDeadlock);
			char* objetivoFaltanteEntrenador1 = obtenerPokemonObjetivoFaltante(entrenador1);
			char* atrapadoInnecesarioEntrenador1 = obtenerPokemonAtrapadoInnecesario(entrenador1);
			for (int posicionEntrenadorParaComparar = posicionEnListaDeadlock; posicionEntrenadorParaComparar < list_size(entrenadoresEnDeadlock); posicionEntrenadorParaComparar++) {
				entrenadorPokemon* entrenador2 = list_get(entrenadoresEnDeadlock, posicionEntrenadorParaComparar);
				char* objetivoFaltanteEntrenador2 = obtenerPokemonObjetivoFaltante(entrenador2);
				char* atrapadoInnecesarioEntrenador2 = obtenerPokemonAtrapadoInnecesario(entrenador2);
				if ((strcmp(objetivoFaltanteEntrenador1, atrapadoInnecesarioEntrenador2)) || (strcmp(atrapadoInnecesarioEntrenador1, objetivoFaltanteEntrenador2))) {
					realizarIntercambio(entrenador1, entrenador2, atrapadoInnecesarioEntrenador1, atrapadoInnecesarioEntrenador2);
					verificarDeadlock(entrenador1);
					verificarDeadlock(entrenador2);
					verificarIntercambios();
				}
			}
		}
	}
}

char* obtenerPokemonObjetivoFaltante(entrenadorPokemon* entrenador) {
	_Bool pokemonObjetivoFaltanteEncontrado;
	char* pokemonAtrapado;
	char* pokemonObjetivo;
	int cantidadPokemonesAtrapados = list_size(entrenador->pokemonesAtrapados);
	int cantidadPokemonesObjetivo = list_size(entrenador->pokemonesObjetivo);
	t_list* listaAtrapadosAuxiliar = list_duplicate(entrenador->pokemonesAtrapados);
	int cantidadAtrapadosAuxiliar = cantidadPokemonesAtrapados;
	for (int posicionObjetivo = 0; posicionObjetivo < cantidadPokemonesObjetivo; posicionObjetivo++) {
		pokemonObjetivoFaltanteEncontrado = FALSE;
		pokemonObjetivo = list_get(entrenador->pokemonesObjetivo, posicionObjetivo);
		for (int posicionAtrapado = 0; posicionAtrapado < cantidadAtrapadosAuxiliar; posicionAtrapado++) {
			pokemonAtrapado = list_get(listaAtrapadosAuxiliar, posicionAtrapado);
			if (strcmp(pokemonAtrapado, pokemonObjetivo)) {
				list_remove(listaAtrapadosAuxiliar, posicionAtrapado);
				cantidadAtrapadosAuxiliar = list_size(listaAtrapadosAuxiliar);
				pokemonObjetivoFaltanteEncontrado = TRUE;
				break;
			}
		}
		if (pokemonObjetivoFaltanteEncontrado == FALSE) {
			return pokemonObjetivo;
			break;
		}
	}
	return pokemonObjetivo;
}

char* obtenerPokemonAtrapadoInnecesario(entrenadorPokemon* entrenador) {
	_Bool pokemonAtrapadoInnecesarioEncontrado;
	char* pokemonAtrapado;
	char* pokemonObjetivo;
	t_list* listaObjetivosAuxiliar = list_duplicate(entrenador->pokemonesObjetivo);
	int cantidadPokemonesAtrapados = list_size(entrenador->pokemonesAtrapados);
	int cantidadPokemonesObjetivo = list_size(entrenador->pokemonesObjetivo);
	int cantidadObjetivosAuxiliar = cantidadPokemonesObjetivo;
	for (int posicionAtrapados = 0; posicionAtrapados < cantidadPokemonesAtrapados; posicionAtrapados++) {
		pokemonAtrapadoInnecesarioEncontrado = FALSE;
		pokemonAtrapado = list_get(entrenador->pokemonesAtrapados, posicionAtrapados);
		for (int posicionObjetivos = 0; posicionObjetivos < cantidadObjetivosAuxiliar; posicionObjetivos++) {
			pokemonObjetivo = list_get(listaObjetivosAuxiliar, posicionObjetivos);
			if (strcmp(pokemonAtrapado, pokemonObjetivo)) {
				list_remove(listaObjetivosAuxiliar, posicionObjetivos);
				cantidadObjetivosAuxiliar = list_size(listaObjetivosAuxiliar);
				pokemonAtrapadoInnecesarioEncontrado = TRUE;
				break;
			}
		}
		if (pokemonAtrapadoInnecesarioEncontrado == FALSE) {
			return pokemonAtrapado;
		}
	}
	return pokemonAtrapado;
}
