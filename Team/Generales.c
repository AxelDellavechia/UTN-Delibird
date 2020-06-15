#include "Generales.h"

void iniciar_logCatedra(){
/*	aux = string_split(configFile->logFile,"/") ;
	int i = 0 ;
	while (aux[i] != NULL) {
		if (string_contains(aux[i],"log")) {
			char * archivoLogCatedra = strdup(aux[i]);
			loggerCatedra = log_create(configFile->logFile, archivoLogCatedra, FALSE, LOG_LEVEL_INFO);
			free(archivoLogCatedra);
			archivoLogCatedra = NULL;
		}
		i++;
	}*/
	//free(aux);
}

void iniciar_log(){
	char * archivoLog = string_duplicate("Team.log");
	logger = log_create(LOG_PATH_INTERNO, archivoLog, FALSE, LOG_LEVEL_INFO);
	free(archivoLog);
	archivoLog = NULL;
}

void leerArchivoDeConfiguracion(char *ruta, t_log *logger) {
	int posicionLista = 0;
	configFile = reservarMemoria(sizeof(archivoConfigFile));
	t_config *config;
	config = config_create(ruta);
	char** posicionEntrenadoresAux = config_get_array_value(config, "POSICIONES_ENTRENADORES");
	char** pokemonEntrenadoresAux = config_get_array_value(config, "POKEMON_ENTRENADORES");
	char** objetivosEntrenadoresAux = config_get_array_value(config, "OBJETIVOS_ENTRENADORES");
	configFile->tiempoReconexion = config_get_int_value(config, "TIEMPO_RECONEXION");
	configFile->retardoCicloCPU = config_get_int_value(config, "RETARDO_CICLO_CPU");
	configFile->algoritmoPlanificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	configFile->quantum = config_get_int_value(config, "QUANTUM");
	configFile->ipBroker = config_get_string_value(config, "IP_BROKER");
	configFile->estimacionInicial = config_get_int_value(config, "ESTIMACION_INICIAL");
	configFile->puertoBroker = config_get_int_value(config, "PUERTO_BROKER");
	configFile->puertoTeam = config_get_int_value(config, "PUERTO_TEAM");
	configFile->logFile = config_get_string_value(config, "LOG_FILE");
	configFile->posicionEntrenadores = list_create();
	configFile->pokemonEntrenadores = list_create();
	configFile->objetivosEntrenadores = list_create();
	while(posicionEntrenadoresAux[posicionLista] != NULL) {
		list_add(configFile->posicionEntrenadores, string_duplicate(posicionEntrenadoresAux[posicionLista]));
		posicionLista++;
	}
	posicionLista = 0;
	while(pokemonEntrenadoresAux[posicionLista] != NULL) {
		list_add(configFile->pokemonEntrenadores, string_duplicate(pokemonEntrenadoresAux[posicionLista]));
		posicionLista++;
	}
	posicionLista = 0;
	while(objetivosEntrenadoresAux[posicionLista] != NULL) {
		list_add(configFile->objetivosEntrenadores, string_duplicate(objetivosEntrenadoresAux[posicionLista]));
		posicionLista++;
	}
	config_destroy(config); // Libero la estructura archivoConfig
	free(posicionEntrenadoresAux);
	free(pokemonEntrenadoresAux);
	free(objetivosEntrenadoresAux);
}

void* reservarMemoria(int size) {
		void *puntero = malloc(size);
		if(puntero == NULL) {
			fprintf(stderr, "Error al reservar %d bytes de memoria", size);
			exit(ERROR);
		}
		return puntero;
}

void crearEstructuras() {
	objetivoTeam = list_create();
	colaNew = list_create();
	colaReady = list_create();
	colaBlocked = list_create();
	colaExit = list_create();
	entrenadoresEnDeadlock = list_create();
}

void obtenerEntrenadores() {
	for(int listaEntrenadores = 0; listaEntrenadores < list_size(configFile->posicionEntrenadores); listaEntrenadores++) {
		char* posicionEntrenador = list_get(configFile->posicionEntrenadores, listaEntrenadores);
		char* pokemonesEntrenador = list_get(configFile->pokemonEntrenadores, listaEntrenadores);
		char* objetivosEntrenador = list_get(configFile->objetivosEntrenadores, listaEntrenadores);
		char* arrayCortado = strtok(posicionEntrenador, "|");
		char* posicionXStr = arrayCortado;
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
		//log_info(logger, "Entrenador %i creado y en cola de New", listaEntrenadores);
		printf("Entrenador %i creado y en cola de New\n", listaEntrenadores);
		list_add_all(objetivoTeam, entrenador->pokemonesObjetivo);
	}
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

void seleccionarEntrenadorMasCercano(cola_APPEARED_POKEMON *pokemonAparecido) {
	entrenadorPokemon* proximoEntrenadorAReady;
	int movimientoDeProximoEntrenadorAReady = 0;
	for(int posicionEntrenador = 0; posicionEntrenador < list_size(colaNew); posicionEntrenador++) {
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
		}
	}
	//proximoEntrenadorAReady->proximaAccion = "AtraparPokemon %s %i %i", pokemonAparecido->nombre_pokemon, pokemonAparecido->posicion_x, pokemonAparecido->posicion_y;
	strcpy(proximoEntrenadorAReady->proximaAccion, "AtraparPokemon %s %i %i", pokemonAparecido->nombre_pokemon, pokemonAparecido->posicion_x, pokemonAparecido->posicion_y);
	list_add(colaReady, proximoEntrenadorAReady);
	//log_info(logger, "Entrenador %i a cola de Ready por ser el más cercano", proximoEntrenadorAReady->idEntrenador);
	printf("Entrenador %i a cola de Ready por ser el más cercano\n", proximoEntrenadorAReady->idEntrenador);
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
					int idEntrenador2 = entrenador2->idEntrenador;
					int posicionXEntrenador2 = entrenador2->posicion_x;
					int posicionYEntrenador2 = entrenador2->posicion_y;
					entrenador1->proximaAccion = "HacerIntercambio %i %i %i %s %s", idEntrenador2, posicionXEntrenador2, posicionYEntrenador2, atrapadoInnecesarioEntrenador1, atrapadoInnecesarioEntrenador2;
					//realizarIntercambio(entrenador1, entrenador2, atrapadoInnecesarioEntrenador1, atrapadoInnecesarioEntrenador2);
					/*verificarDeadlock(entrenador1);
					verificarDeadlock(entrenador2);
					verificarIntercambios();*/
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

void ejecutar() {
	char* algoritmo = configFile->algoritmoPlanificacion;
	char* accion;
	int quantum;
	char* accionAComparar;
	int posicionProximoAEjecutar;
	int rafagaCPUAccion;
	entrenadorPokemon* entrenador = list_get(colaReady, 0);
	accion = entrenador->proximaAccion;
	if (strcmp("FIFO", algoritmo)) {
		exec = entrenador;
		pthread_mutex_unlock(&exec->semaforMutex);
		list_remove(colaReady, 0);
		realizarAccion(entrenador, 0);
	} else if (strcmp("RR", algoritmo)) {
		quantum = configFile->quantum;
		exec = entrenador;
		pthread_mutex_unlock(&exec->semaforMutex);
		list_remove(colaReady, 0);
		realizarAccion(entrenador, quantum);
	}else if (strcmp("SJF sin desalojo", algoritmo)) {
		rafagaCPUAccion = calcularRafagaCPU(accion);
		for (int posicionEntrenador = 1; posicionEntrenador < list_size(colaReady); posicionEntrenador++) {
			entrenadorPokemon* entrenador = list_get(colaReady, posicionEntrenador);
			accionAComparar = entrenador->proximaAccion;
			int rafagaCPUComparacion = calcularRafagaCPU(accionAComparar);
			if(rafagaCPUComparacion > rafagaCPUAccion) {
				rafagaCPUAccion = rafagaCPUComparacion;
				accion = accionAComparar;
				posicionProximoAEjecutar = posicionEntrenador;
			}
		}
		exec = entrenador;
		pthread_mutex_unlock(&exec->semaforMutex);
		list_remove(colaReady, posicionProximoAEjecutar);
		realizarAccion(entrenador, 0);
	} else if (strcmp("SJF con desalojo", algoritmo)){
		rafagaCPUAccion = calcularRafagaCPU(accion);
		for (int posicionEntrenador = 1; posicionEntrenador < list_size(colaReady); posicionEntrenador++) {
			entrenadorPokemon* entrenador = list_get(colaReady, posicionEntrenador);
			accionAComparar = entrenador->proximaAccion;
			int rafagaCPUComparacion = calcularRafagaCPU(accionAComparar);
			if(rafagaCPUComparacion > rafagaCPUAccion) {
				rafagaCPUAccion = rafagaCPUComparacion;
				accion = accionAComparar;
				posicionProximoAEjecutar = posicionEntrenador;
			}
		}
		if (exec != NULL) {
			list_add(colaReady, exec);
		}
		exec = entrenador;
		pthread_mutex_unlock(&exec->semaforMutex);
		list_remove(colaReady, posicionProximoAEjecutar);
		realizarAccion(entrenador, 0);
	} else {
		printf("Algoritmo incorrecto/n");
	}
}

int calcularRafagaCPU(accion) {

}

void realizarAccion(entrenadorPokemon* entrenador, int tiempo) {
	char arrayAccion[50];
	char* accion = entrenador->proximaAccion;
	strcpy(arrayAccion,accion);
	char* accionStr = strtok(arrayAccion, " ");
	if (strcmp("AtraparPokemon", accionStr) == 0) {
		char* pokemonAAtrapar = strtok(NULL, " ");
		char* posicionXPokemon = strtok(NULL, " ");
		char* posicionYPokemon = strtok(NULL, " ");
		int posicionXInt = atoi(posicionXPokemon);
		int posicionYInt = atoi(posicionYPokemon);
		printf("La accion es %s\n", accion);
		moverEntrenador(entrenador, posicionXInt, posicionYInt);
		//catch_pokemon();
	} else if(strcmp("HacerIntercambio", accionStr) == 0) {
		char* idEntrenador2 = strtok(NULL, " ");
		char* posicionXEntrenador2 = strtok(NULL, " ");
		char* posicionYEntrenador2 = strtok(NULL, " ");
		char* atrapadoInnecesarioEntrenador1 = strtok(NULL, " ");
		char* atrapadoInnecesarioEntrenador2 = strtok(NULL, " ");
		int idEntrenador2Int = atoi(idEntrenador2);
		int posicionXEntrenador2Int = atoi(posicionXEntrenador2);
		int posicionYEntrenador2Int = atoi(posicionYEntrenador2);
		_Bool buscarEntrenadorPorID(entrenadorPokemon* entrenadorABuscar) {
			return entrenadorABuscar->idEntrenador == idEntrenador2Int;
		}
		entrenadorPokemon* entrenador2 = list_find(entrenadoresEnDeadlock, (void*) buscarEntrenadorPorID);
		moverEntrenador(entrenador, posicionXEntrenador2Int, posicionYEntrenador2Int);
		realizarIntercambio(entrenador, entrenador2, atrapadoInnecesarioEntrenador1, atrapadoInnecesarioEntrenador2);
		verificarDeadlock(entrenador);
		verificarDeadlock(entrenador2);
		verificarIntercambios();
	}
	pthread_mutex_lock(&entrenador->semaforMutex);
}

void catch_pokemon(posicionPokemon* pokemon, int entrenador) {
	if (validar_conexion) {
		//aplicar_protocolo_enviar(fdBroker, 0, CATCH_POKEMON(pokemon));
		//void* resultadoCatch = aplicar_protocolo_recibir(fdBroker, 0);
		///caught_pokemon(pokemon, resultadoCatch);
		list_add(colaBlocked, entrenador);
	} else {
		caught_pokemon(pokemon, "Capturado");
	}
}

void caught_pokemon(posicionPokemon* pokemon, void* resultadoCatch) {
	//FALTA AGREGAR LA LOGICA DE QUE CUANDO RECIBO UN CAUGHT BUSCO AL ENTRENADOR QUE LE INTERESA ESE MENSAJE
	//verificarDeadlock();
}

/*void localized_pokemon(posicionPokemon* pokemonEncontrado) {
	int posicion = 0;
	while (mapaPokemon[posicion].head->next != NULL) {
		_Bool buscarPokemon(posicionPokemon* pokemonEncontrado) {
			return pokemonEncontrado == mapaPokemon[posicion].head->data;
		}
		void* pokemonYaConocido = list_find(mapaPokemon, (void*) buscarPokemon);
	}
}*/

void inicializar_semaforos(){
	//inicializo semaforos de nodos
	pthread_mutex_init(&mxSocketsFD, NULL);
	pthread_mutex_init(&mxHilos, NULL);
}

void crearHilos() {
	hilo_servidor = 0;
	hilo_consola = 0;
	pthread_create(&hilo_servidor, NULL, (void*) planificador, NULL);
	pthread_create(&hilo_consola, NULL, (void*) consola, NULL);
	pthread_create(&hilo_conexion, NULL, (void*) reconectar, NULL);
	pthread_mutex_init(&hilo_conexion,NULL);
	pthread_mutex_lock(&hilo_conexion);
	pthread_join(hilo_servidor, NULL);
	pthread_join(hilo_consola, NULL);
	pthread_join(hilo_conexion, NULL);
	//tener un thread para manejar la reconexión cada x segundos , es un connect por cada msj
}

void consola() {
	printf("Hola! Ingresá \"salir\" para finalizar módulo\n");
	size_t buffer_size = 100; //por el momento restringido a 100 caracteres
	char* comando = (char *) calloc(1, buffer_size);
	while (!string_equals_ignore_case(comando, "salir\n")) {
		printf(">");
		int bytes_read = getline(&comando, &buffer_size, stdin);
		if (bytes_read == -1) {
			log_error(logger,"Error en getline");
		}
		if (bytes_read == 1) {
			continue;
		}
	}
	log_destroy(logger);
	free(comando);
	pthread_detach(hilo_servidor);
	for ( int i = 0 ; i < list_size(configFile->posicionEntrenadores) ; i++){
		pthread_detach(hilo);
		pthread_cancel(hilo);
	}
	pthread_cancel(hilo_servidor);
	pthread_detach( pthread_self() );
}

void servidor() {
	fdTeam = nuevoSocket();
	asociarSocket(fdTeam, configFile->puertoTeam);
	escucharSocket(fdTeam, CONEXIONES_PERMITIDAS);
	log_info(logger," Escuchando conexiones");
	while(TRUE) {
		int conexionNueva = 0;
		int comandoNuevo;//= reservarMemoria(INT);
		while(conexionNueva == 0) {
			comandoNuevo = aceptarConexionSocket(fdTeam);
			conexionNueva = handshake_servidor ( comandoNuevo,"Team" , "Broker",logger);
			if(!validar_conexion(conexionNueva, 0,logger) ) {
				pthread_mutex_lock(&mxSocketsFD); //desbloquea el semaforo
				cerrarSocket(fdTeam);
				pthread_mutex_unlock(&mxSocketsFD);
			}
		}
	pthread_t hilo;
	pthread_mutex_lock(&mxHilos);
	pthread_create(&hilo, NULL, (void*) thread_Team,comandoNuevo);
	pthread_mutex_unlock(&mxHilos);
	}
}

int thread_Team(int fdCliente) {
	//aplicar_protocolo_recibir(fdCliente , logger); // recibo mensajes
	pthread_mutex_lock(&mxHilos);
	pthread_detach( pthread_self() );
	pthread_mutex_unlock(&mxHilos);
	return FALSE;
}

void bloquearSemaforoEnt( t_list * lista , int indice ) {
	entrenadorPokemon * elEntrenador = list_get(lista,indice);
	//log_info(logger,"El thread_Entrenador %d tiene su semaforo en valor: %d antes del wait",elEntrenador->idEntrenador,elEntrenador->semafoContador)
	list_remove(lista,indice);
	pthread_mutex_lock(&elEntrenador->semaforMutex);
	//log_info(logger,"El thread_Entrenador %d tiene su semaforo en valor: %d despues del signal",elEntrenador->idEntrenador,elEntrenador->semafoContador);
	list_add_in_index(lista,indice,elEntrenador);
}

void desBloquearSemaforoEnt( t_list * lista , int indice ) {
	entrenadorPokemon * elEntrenador = list_get(lista,indice);
	list_remove(lista,indice);
	pthread_mutex_unlock(&elEntrenador->semaforMutex);
	list_add_in_index(lista,indice,elEntrenador);
}

void planificador() {
	//log_info(logger,"Obteniendo los Entrenadores");
	printf("Obteniendo los entrenadores\n");
	obtenerEntrenadores();
	fdTeam = nuevoSocket();
	asociarSocket(fdTeam, configFile->puertoTeam);
	escucharSocket(fdTeam, CONEXIONES_PERMITIDAS);
	//log_info(logger,"Escuchando conexiones");
	printf("Escuchando conexiones\n");
	int head , bufferTam;
	for (int i = 0; i < list_size(configFile->posicionEntrenadores); i++){
		entrenadorPokemon * entrenador = list_get(colaNew,i) ;
		//sem_wait(&entrenador->semafoContador);
		pthread_create(&hilo, NULL, (void*) thread_Entrenador,entrenador);
		//list_add(misHilos,process_get_thread_id());
		//log_info(logger,"Cree un hilo para el entrenador %d y tiene ID %d",i,hilo=pthread_self());
	}
	while(TRUE) {
		int conexionNueva = 0;
		int comandoNuevo;//= reservarMemoria(INT);
		while(conexionNueva == 0) {
			comandoNuevo = aceptarConexionSocket(fdTeam);
			conexionNueva = handshake_servidor(comandoNuevo,"Team", "Broker", logger);
			if(!validar_conexion(conexionNueva, 0,logger) ) {
				cerrarSocket(fdTeam);
			}
		}
		recibirProtocolo(&head,&bufferTam,comandoNuevo); // recibo head y tamaño de msj
		void * mensaje = malloc(bufferTam);
		recibirMensaje(comandoNuevo , bufferTam , mensaje ); // recibo msj serializado para el tratamiento deseado
		/*log_info(logger,"aplicar_protocolo_recibir -> recibió el HEAD #%d",head);
		log_info(logger,"aplicar_protocolo_recibir -> recibió un tamaño de -> %d",bufferTam);
		log_info(logger,"aplicar_protocolo_recibir -> comienza a deserealizar");*/
		printf("aplicar_protocolo_recibir -> recibió el HEAD #%d\n",head);
		printf("aplicar_protocolo_recibir -> recibió un tamaño de -> %d\n",bufferTam);
		printf("aplicar_protocolo_recibir -> comienza a deserealizar\n");
		switch( head ){
		case APPEARED_POKEMON :{
			cola_APPEARED_POKEMON app_poke;
			deserealizar_APPEARED_POKEMON(head, mensaje, bufferTam, & app_poke);
			seleccionarEntrenadorMasCercano(&app_poke);
			//responder por localized_pokemon
			//log_info(logger,"Recibí en la cola APPEARED_POKEMON . POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",app_poke.nombre_pokemon,app_poke.posicion_x,app_poke.posicion_y);
			printf("Recibí en la cola APPEARED_POKEMON . POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d \n",app_poke.nombre_pokemon,app_poke.posicion_x,app_poke.posicion_y);
			free(app_poke.nombre_pokemon);
		break;
		}
		case CAUGHT_POKEMON :{
			cola_CAUGHT_POKEMON caug_poke ;
			//responde por caught_pokemon
			deserealizar_CAUGHT_POKEMON(head, mensaje, bufferTam, & caug_poke);
			//log_info(logger,"Recibí en la cola CAUGHT_POKEMON . MENSAJE ID: %d  , ATRAPO: %d",caug_poke.id_mensaje,caug_poke.atrapo_pokemon);
			printf("Recibí en la cola CAUGHT_POKEMON . MENSAJE ID: %d  , ATRAPO: %d\n",caug_poke.id_mensaje,caug_poke.atrapo_pokemon);
		break;
		}
		case LOCALIZED_POKEMON :{
			cola_LOCALIZED_POKEMON loc_poke ;
			deserealizar_LOCALIZED_POKEMON(head, mensaje, bufferTam, & loc_poke);
			for (int i = 0 ; i < list_size(loc_poke.lista_posiciones); i++){
			//log_info(logger,"Recibí en la cola LOCALIZED_POKEMON . POKEMON: %s  , CANTIDAD: %d , POSICIÓN X: %d , POSICIÓN Y: %d",loc_poke.nombre_pokemon,loc_poke.cantidad,list_get(loc_poke.lista_posiciones,i),list_get(loc_poke.lista_posiciones,i + 1));
			printf("Recibí en la cola LOCALIZED_POKEMON . POKEMON: %s  , CANTIDAD: %d , POSICIÓN X: %d , POSICIÓN Y: %d\n",loc_poke.nombre_pokemon,loc_poke.cantidad,list_get(loc_poke.lista_posiciones,i),list_get(loc_poke.lista_posiciones,i + 1));
			i++;
		}
			desBloquearSemaforoEnt(colaNew,1);
			free(loc_poke.nombre_pokemon);
			list_destroy(loc_poke.lista_posiciones);
		break;
		}
		default:
			//log_info(logger, "Instrucción no reconocida");
			printf("Instrucción no reconocida");
		break;
		}
		/*
											case NEW_POKEMON :{
												cola_NEW_POKEMON  new_poke ;
												deserealizar_NEW_POKEMON ( head, mensaje, bufferTam, & new_poke);
												log_info(logger,"Recibí en la cola NEW_POKEMON . POKEMON: %s  , CANTIDAD: %d  , CORDENADA X: %d , CORDENADA Y: %d ",new_poke.nombre_pokemon,new_poke.cantidad,new_poke.posicion_x,new_poke.posicion_y);
												break;
											}
											case CATCH_POKEMON :{
																	cola_CATCH_POKEMON cath_poke;
																	deserealizar_CATCH_POKEMON( head, mensaje, bufferTam, & cath_poke);
																	log_info(logger,"Recibí en la cola CATCH_POKEMON . POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",cath_poke.nombre_pokemon,cath_poke.posicion_x,cath_poke.posicion_y);
																	break;
											}
											case GET_POKEMON :{
												cola_GET_POKEMON get_poke ;
												deserealizar_GET_POKEMON ( head, mensaje, bufferTam, & get_poke);
												log_info(logger,"Recibí en la cola GET_POKEMON . POKEMON: %s",get_poke.nombre_pokemon);
												break;
											}
											*/
		ejecutar();
	}
}

void thread_Entrenador(entrenadorPokemon * elEntrenador) {
	/*pthread_mutex_lock( (pthread_t) & elEntrenador->semaforMutex);
	//log_info(logger,"Trabajando con el Entrenador Nro %d",elEntrenador->idEntrenador);
	//realizarAccion(elEntrenador);
	//EN EL CATCH -> APLICAR_PROTOCOLO_ENVIAR -> CATCH
	// RECIBIR EL ID DE MSJ Y GUARDARLO EN UNA VARIABLE
	//CUANDO RECIBO EL CAUGHT VALIDO MIS ENTRENADORES Y DESPIERTO AL QUE ESPERA EL MSJ
	// VER SITUACIÓN DE EJECUTACIÓN.
	pthread_mutex_unlock(&elEntrenador->semaforMutex);*/
}

void reconectar(){
	void reconectar(){
		pthread_mutex_lock(&h_reconectar);
		while (conBroker != 1) {
			sleep(configFile->tiempoReconexion);
			conBroker = conectarCon(fdBroker,configFile->ipBroker,configFile->puertoBroker,logger);
		}
		handshake_cliente(fdBroker,"Team","Broker",logger);
		pthread_mutex_unlock(&h_reconectar);
	}
}
