#include "Generales.h"

void iniciar_logCatedra(){
	aux = string_split(configFile->logFile,"/") ;
	int i = 0 ;
	while (aux[i] != NULL) {
		if (string_contains(aux[i],"log") ) {
			//char * archivoLogCatedra = string_duplicate(aux[i]);
			char * archivoLogCatedra = string_duplicate("logCatedra.log");
			pthread_mutex_init(&mutexLogCatedra, NULL);
			pthread_mutex_lock(&mutexLogCatedra);
			loggerCatedra = log_create(configFile->logFile, archivoLogCatedra, false, LOG_LEVEL_INFO);
			pthread_mutex_unlock(&mutexLogCatedra);
			free(archivoLogCatedra);
			archivoLogCatedra = NULL;
			return;
		}
		i++;
	}
	//free(aux);
}

void iniciar_log(){
	char * archivoLog = string_duplicate("Team.log");
	pthread_mutex_init(&mutexLog, NULL);
	pthread_mutex_lock(&mutexLog);
	logger = log_create(LOG_PATH_INTERNO, archivoLog, false, LOG_LEVEL_INFO);
	pthread_mutex_unlock(&mutexLog);
	free(archivoLog);
	archivoLog = NULL;
}

void leerArchivoDeConfiguracion(char *ruta) {
	int posicionLista = 0;
	configFile = reservarMemoria(sizeof(archivoConfigFile));
	t_config *config;
	config = config_create(ruta);
	configFile->tiempoReconexion = config_get_int_value(config, "TIEMPO_RECONEXION");
	configFile->retardoCicloCPU = config_get_int_value(config, "RETARDO_CICLO_CPU");
	configFile->algoritmoPlanificacion = malloc(1 + string_length(config_get_string_value(config, "ALGORITMO_PLANIFICACION")));
	configFile->algoritmoPlanificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	configFile->quantum = config_get_int_value(config, "QUANTUM");
	configFile->ipBroker = malloc(1 + string_length(config_get_string_value(config, "IP_BROKER")));
	configFile->ipBroker = config_get_string_value(config, "IP_BROKER");
	configFile->estimacionInicial = config_get_int_value(config, "ESTIMACION_INICIAL");
	configFile->puertoBroker = config_get_int_value(config, "PUERTO_BROKER");
	configFile->puertoTeam = config_get_int_value(config, "PUERTO_TEAM");
	configFile->logFile = malloc(1 + string_length(config_get_string_value(config, "LOG_FILE")));
	configFile->logFile = config_get_string_value(config, "LOG_FILE");
	configFile->alpha = config_get_double_value(config, "ALPHA");
	char** posicionEntrenadoresAux = config_get_array_value(config, "POSICIONES_ENTRENADORES");
	char** pokemonEntrenadoresAux = config_get_array_value(config, "POKEMON_ENTRENADORES");
	char** objetivosEntrenadoresAux = config_get_array_value(config, "OBJETIVOS_ENTRENADORES");
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
	//config_destroy(config); // Libero la estructura archivoConfig
	/*free(posicionEntrenadoresAux);
	free(pokemonEntrenadoresAux);
	free(objetivosEntrenadoresAux);*/
}

void* reservarMemoria(int size) {
	void *puntero = malloc(size);
	if(puntero == NULL) {
		fprintf(stderr, "Error al reservar %d bytes de memoria\n", size);
		exit(ERROR);
	}
	return puntero;
}

void crearEstructuras() {
	//listaCatchPokemon = list_create();
	objetivoTeam = list_create();
	colaNew = list_create();
	colaReady = list_create();
	colaBlocked = list_create();
	colaExit = list_create();
	entrenadoresEnDeadlock = list_create();
	idMensajeEsperado = 0;
	crearLista = 1;
	ciclosEnCPU = 0;
	cantCambiosContexto = 0;
	cantDeadlocks = 0;
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
		pthread_mutex_init(&entrenador->semaforMutex, NULL);
		entrenador->ciclosEnCPU = 0;
		entrenador->proximaAccion = "";
		entrenador->estimacionUltimaRafaga = 0;
		pthread_mutex_lock(&entrenador->semaforMutex);
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
		log_info(loggerCatedra, "Entrenador %i creado y en cola de New", listaEntrenadores);
		list_add_all(objetivoTeam, entrenador->pokemonesObjetivo);
		list_add(colaReady, entrenador);
		log_info(loggerCatedra, "Entrenador %i movido a cola de Ready", listaEntrenadores);
		list_remove(colaNew, 0);
	}
	cantEntrenadores = list_size(colaReady);
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

entrenadorPokemon* seleccionarEntrenadorMasCercano(cola_APPEARED_POKEMON *pokemonAparecido) {
	entrenadorPokemon* proximoEntrenadorEnEjecutar;
	int movimientoDeProximoEntrenadorAReady = 0;
	int posicionProximoEntrenador = 0;
	int posicionXPokemon = pokemonAparecido->posicion_x;
	int posicionYPokemon = pokemonAparecido->posicion_y;
	for(int posicionEntrenador = 0; posicionEntrenador < list_size(colaReady); posicionEntrenador++) {
		entrenadorPokemon* entrenador = list_get(colaReady, posicionEntrenador);
		char* proximaAccionEntrenador = entrenador->proximaAccion;
		if (string_equals_ignore_case(proximaAccionEntrenador, "")) {
			int posicionXEntrenador = entrenador->posicion_x;
			int posicionYEntrenador = entrenador->posicion_y;
			int movimientoEnX = fabs(posicionXEntrenador - posicionXPokemon);
			int movimientoEnY = fabs(posicionYEntrenador - posicionYPokemon);
			int movimientoEntrenador = movimientoEnX + movimientoEnY;
			if((movimientoEntrenador < movimientoDeProximoEntrenadorAReady) || (posicionEntrenador == 0)) {
				movimientoDeProximoEntrenadorAReady = movimientoEntrenador;
				proximoEntrenadorEnEjecutar = entrenador;
				posicionProximoEntrenador = posicionEntrenador;
			}
		}
	}
	list_add_in_index(colaReady, 0, proximoEntrenadorEnEjecutar);
	list_remove(colaReady, posicionProximoEntrenador+1);
	log_info(loggerCatedra, "Entrenador %i a cola de Ready por ser el más cercano", proximoEntrenadorEnEjecutar->idEntrenador);
	//printf("Entrenador %i es el más cercano\n", proximoEntrenadorEnEjecutar->idEntrenador);
	return proximoEntrenadorEnEjecutar;
}

int moverEntrenador(entrenadorPokemon* entrenador, int posicionXDestino, int posicionYDestino, int tiempo) {
	int moverEntrenadorEnX = TRUE;
	int moverEntrenadorEnY = TRUE;
	int posicionXEntrenador = entrenador->posicion_x;
	int posicionYEntrenador = entrenador->posicion_y;
	int ciclosEnCPUEntrenador = entrenador->ciclosEnCPU;
	int ciclosEnCPUEntrenadorMasTiempo = ciclosEnCPUEntrenador + tiempo;
	if (tiempo == 0) {
		ciclosEnCPUEntrenadorMasTiempo = -1;
	}
	while (moverEntrenadorEnX == TRUE) {
		if (ciclosEnCPUEntrenador != ciclosEnCPUEntrenadorMasTiempo) {
			if (posicionXEntrenador < posicionXDestino) {
				posicionXEntrenador++;
				ciclosEnCPUEntrenador++;
			} else if (posicionXEntrenador > posicionXDestino) {
				posicionXEntrenador--;
				ciclosEnCPUEntrenador++;
			} else {
				moverEntrenadorEnX = FALSE;
			}
		} else {
			moverEntrenadorEnX = FALSE;
		}
	}
	while (moverEntrenadorEnY == TRUE) {
		if(ciclosEnCPUEntrenador != ciclosEnCPUEntrenadorMasTiempo) {
			if (posicionYEntrenador < posicionYDestino) {
				posicionYEntrenador++;
				ciclosEnCPUEntrenador++;
			} else if (posicionYEntrenador > posicionYDestino) {
				posicionYEntrenador--;
				ciclosEnCPUEntrenador++;
			} else {
				moverEntrenadorEnY = FALSE;
			}
		} else {
			moverEntrenadorEnY = FALSE;
		}
	}
	entrenador->ciclosEnCPU = ciclosEnCPUEntrenador;
	entrenador->posicion_x = posicionXEntrenador;
	entrenador->posicion_y = posicionYEntrenador;
	ciclosEnCPU = ciclosEnCPU + ciclosEnCPUEntrenador;
	//printf("Entrenador %i se movió a la posición %i %i estando %i ciclos en la CPU\n", entrenador->idEntrenador, entrenador->posicion_x, entrenador->posicion_y, entrenador->ciclosEnCPU);
	log_info(loggerCatedra, "Entrenador %i se movió a la posición %i %i", entrenador->idEntrenador, entrenador->posicion_x, entrenador->posicion_y);
	if ((posicionXEntrenador == posicionXDestino) && (posicionYEntrenador == posicionYDestino)){
		return 1;
	} else {
		return 0;
	}
}

void realizarIntercambio(entrenadorPokemon* entrenador1, entrenadorPokemon* entrenador2, char* pokemonEntrenador1, char* pokemonEntrenador2) {
	quitarPokemonDeAtrapados(entrenador1, pokemonEntrenador1);
	quitarPokemonDeAtrapados(entrenador2, pokemonEntrenador2);
	list_add(entrenador1->pokemonesAtrapados, pokemonEntrenador2);
	list_add(entrenador2->pokemonesAtrapados, pokemonEntrenador1);
	log_info(loggerCatedra, "Se realizó el intercambio del pokemon %s del entrenador %i por el pokemon %s del entrenador %i", pokemonEntrenador1, entrenador1->idEntrenador, pokemonEntrenador2, entrenador2->idEntrenador);
	ciclosEnCPU = ciclosEnCPU + 5;
	//printf("Se realizó el intercambio del pokemon %s del entrenador %i por el pokemon %s del entrenador %i\n", pokemonEntrenador1, entrenador1->idEntrenador, pokemonEntrenador2, entrenador2->idEntrenador);
}

void quitarPokemonDeAtrapados(entrenadorPokemon* entrenador, char* pokemon) {
	for (int pokemonPosition = 0; pokemonPosition < list_size(entrenador->pokemonesAtrapados); pokemonPosition++) {
		char* pokemonEntrenador = list_get(entrenador->pokemonesAtrapados, pokemonPosition);
		if (string_equals_ignore_case(pokemonEntrenador, pokemon)) {
			list_remove(entrenador->pokemonesAtrapados, pokemonPosition);
			break;
		}
	}
}

//Verifica si el entrenador esta en deadlock. Si no lo esta, lo quita de blocked y lo pasa a ready seteandole "" como proxima accion
void verificarDeadlock(entrenadorPokemon* entrenador) {
	int pokemonEncontrado;
	int entrenadorEnDeadlock;
	t_list* listaObjetivosAuxiliar = list_duplicate(entrenador->pokemonesObjetivo);
	int cantidadPokemonesAtrapados = list_size(entrenador->pokemonesAtrapados);
	int cantidadPokemonesObjetivo = list_size(entrenador->pokemonesObjetivo);
	/*printf("Entrenador %i\n", entrenador->idEntrenador);
	for (int i=0; i < cantidadPokemonesAtrapados;i++) {
		char* pokemon = list_get(entrenador->pokemonesAtrapados, i);
		printf("Pokemon atrapado %s\n", pokemon);
	}
	for (int i=0; i < cantidadPokemonesObjetivo;i++) {
		char* pokemon = list_get(entrenador->pokemonesObjetivo, i);
		printf("Pokemon objetivo %s\n", pokemon);
	}*/
	int cantidadObjetivosAuxiliar = cantidadPokemonesObjetivo;
	if (cantidadPokemonesAtrapados == cantidadPokemonesObjetivo) {
		for (int posicionAtrapados = 0; posicionAtrapados < cantidadPokemonesAtrapados; posicionAtrapados++) {
			pokemonEncontrado = FALSE;
			char* pokemonAtrapado = list_get(entrenador->pokemonesAtrapados, posicionAtrapados);
			for (int posicionObjetivos = 0; posicionObjetivos < cantidadObjetivosAuxiliar; posicionObjetivos++) {
				char* pokemonObjetivo = list_get(listaObjetivosAuxiliar, posicionObjetivos);
				if (string_equals_ignore_case(pokemonAtrapado, pokemonObjetivo)) {
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
					//printf("Entrenador %i en deadlock\n", entrenador->idEntrenador);
				}
				verificarIntercambios();
			}
		}
		if (cantidadObjetivosAuxiliar == 0) {
			for (int posicionEnListaDeadlock = 0; posicionEnListaDeadlock < list_size(entrenadoresEnDeadlock); posicionEnListaDeadlock++) {
				entrenadorPokemon* entrenadorEnListaDeadlock = list_get(entrenadoresEnDeadlock, posicionEnListaDeadlock);
				if (entrenador->idEntrenador == entrenadorEnListaDeadlock->idEntrenador) {
					list_remove(entrenadoresEnDeadlock, posicionEnListaDeadlock);
					/*for (int i=0; i < list_size(entrenadoresEnDeadlock);i++) {
						entrenadorPokemon* entrenador = list_get(entrenadoresEnDeadlock, i);
						//printf("Entrenador en deadlock %i\n", entrenador->idEntrenador);
					}*/
					break;
				}
			}
			//quitarDeColaBlocked(entrenador);
			list_add(colaExit, entrenador);
			log_info(loggerCatedra, "Se movió al entrenador de id %i a Exit ya que atrapó a todos los Pokemon que tenía como objetivo", entrenador->idEntrenador);
			if (cantEntrenadores == list_size(colaExit)) {
				log_info(loggerCatedra, "Cantidad de ciclos de CPU totales: %i", ciclosEnCPU);
				log_info(loggerCatedra, "Cantidad de cambios de contexto realizados: %i", cantCambiosContexto);
				log_info(loggerCatedra, "Cantidad de ciclos de CPU realizados por entrenador:");
				for (int posicionEntrenador = 0; posicionEntrenador < list_size(colaExit); posicionEntrenador++) {
					entrenadorPokemon* entenador = list_get(colaExit, posicionEntrenador);
					log_info(loggerCatedra, "Entrenador %i - Ciclos de CPU: %i", entrenador->idEntrenador, entrenador->ciclosEnCPU);
				}
				log_info(loggerCatedra, "Deadlocks producidos y resueltos: %i", cantDeadlocks);
			}
			//printf("Se movio al entrenador de id %i a Exit\n", entrenador->idEntrenador);
		}
	} else {
		list_add(colaReady, entrenador);
		//printf("Se movio al entrenador de id %i a la cola de Ready\n", entrenador->idEntrenador);
		log_info("Se movió al entrenador de id %i de la cola de Blocked a la cola de Ready ya que no completó sus objetivos", entrenador->idEntrenador);
		/*for (int i=0; i < list_size(colaReady);i++) {
			entrenadorPokemon* entrenador = list_get(colaReady, i);
			printf("Entrenador posicion %i es %i\n", i, entrenador->idEntrenador);
		}*/
		quitarDeColaBlocked(entrenador);
		entrenador->proximaAccion = "";
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
	char* proximaAccionEntrenador = string_new();
	if (list_size(entrenadoresEnDeadlock) > 1) {
		log_info(loggerCatedra, "Inicia algoritmo de detección de deadlock");
		for (int posicionEnListaDeadlock = 0; posicionEnListaDeadlock < list_size(entrenadoresEnDeadlock); posicionEnListaDeadlock++) {
			entrenadorPokemon* entrenador1 = list_get(entrenadoresEnDeadlock, posicionEnListaDeadlock);
			char* objetivoFaltanteEntrenador1 = obtenerPokemonObjetivoFaltante(entrenador1);
			char* atrapadoInnecesarioEntrenador1 = obtenerPokemonAtrapadoInnecesario(entrenador1);
			for (int posicionEntrenadorParaComparar = posicionEnListaDeadlock + 1; posicionEntrenadorParaComparar < list_size(entrenadoresEnDeadlock); posicionEntrenadorParaComparar++) {
				entrenadorPokemon* entrenador2 = list_get(entrenadoresEnDeadlock, posicionEntrenadorParaComparar);
				char* objetivoFaltanteEntrenador2 = obtenerPokemonObjetivoFaltante(entrenador2);
				char* atrapadoInnecesarioEntrenador2 = obtenerPokemonAtrapadoInnecesario(entrenador2);
				if ((string_equals_ignore_case(objetivoFaltanteEntrenador1, atrapadoInnecesarioEntrenador2)) || (string_equals_ignore_case(atrapadoInnecesarioEntrenador1, objetivoFaltanteEntrenador2))) {
					int idEntrenador2 = entrenador2->idEntrenador;
					int posicionXEntrenador2 = entrenador2->posicion_x;
					int posicionYEntrenador2 = entrenador2->posicion_y;
					entrenador1->proximaAccion = "";
					log_info(loggerCatedra, "Se detectó deadlock entre los entrenadores %i y %i", entrenador1->idEntrenador, idEntrenador2);
					cantDeadlocks++;
					//log_info(loggerCatedra, "Se realizará un intercambio entre los entrenadores %i y %i", idEntrenador2, entrenador1->idEntrenador);
					string_append_with_format(&proximaAccionEntrenador, "HacerIntercambio %i %i %i %s %s", idEntrenador2, posicionXEntrenador2, posicionYEntrenador2, atrapadoInnecesarioEntrenador1, atrapadoInnecesarioEntrenador2);
					entrenador1->proximaAccion = proximaAccionEntrenador;
					quitarDeColaBlocked(entrenador1);
					list_add(colaReady, entrenador1);
					ejecutar();
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
	int pokemonObjetivoFaltanteEncontrado;
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
			if (string_equals_ignore_case(pokemonAtrapado, pokemonObjetivo)) {
				list_remove(listaAtrapadosAuxiliar, posicionAtrapado);
				cantidadAtrapadosAuxiliar = list_size(listaAtrapadosAuxiliar);
				pokemonObjetivoFaltanteEncontrado = TRUE;
				break;
			}
		}
		if (pokemonObjetivoFaltanteEncontrado == FALSE) {
			return pokemonObjetivo;
		}
	}
	return pokemonObjetivo;
}

char* obtenerPokemonAtrapadoInnecesario(entrenadorPokemon* entrenador) {
	int pokemonAtrapadoInnecesarioEncontrado;
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
			if (string_equals_ignore_case(pokemonAtrapado, pokemonObjetivo)) {
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

entrenadorPokemon* verificarMensajeRecibido(int idMensajeRecibido) {
	entrenadorPokemon* entrenador = NULL;
	for (int posicionEntrenador = 0; posicionEntrenador < list_size(colaBlocked); posicionEntrenador++) {
		entrenador = list_get(colaBlocked, posicionEntrenador);
		if (idMensajeRecibido == entrenador->idMsjEsperado) {
			break;
		}
	}
	return entrenador;
}

//Agrega el pokemon atrapado a la lista de pokemones atrapados del entrenador, quita al pokemon de la lista CatchPokemon y verifica si el entrenador esta en Deadlock
void pokemonAtrapado(entrenadorPokemon* entrenador, cola_CAUGHT_POKEMON* pokemonRecibido) {
	for(int posicionPokemon = 0; posicionPokemon < list_size(listaCatchPokemon); posicionPokemon++) {
		cola_CATCH_POKEMON* pokemon = list_get(listaCatchPokemon, posicionPokemon);
		if(pokemon->id_mensaje == pokemonRecibido->id_mensaje) {
			entrenador->proximaAccion = "";
			list_add(entrenador->pokemonesAtrapados, string_duplicate(pokemon->nombre_pokemon));
			for (int posicionObjetivo = 0; posicionObjetivo < list_size(objetivoTeam); posicionObjetivo++) {
				char* pokemonObjetivo = list_get(objetivoTeam, posicionObjetivo);
				if (string_equals_ignore_case(pokemonObjetivo, pokemon->nombre_pokemon)){
					list_remove(objetivoTeam, posicionObjetivo);
				}
			}
			list_remove(listaCatchPokemon, posicionPokemon);
			verificarDeadlock(entrenador);
			break;
		}
	}
}

int pokemonNecesario(cola_APPEARED_POKEMON* pokemonAparecido) {
	for (int posicionObjetivo = 0; posicionObjetivo < list_size(objetivoTeam); posicionObjetivo++) {
		char* pokemonObjetivo = list_get(objetivoTeam, posicionObjetivo);
		if (string_equals_ignore_case(pokemonObjetivo, pokemonAparecido->nombre_pokemon)){
			return TRUE;
		}
	}
	return FALSE;
}

void catchPokemon(entrenadorPokemon* entrenador, char* nombrePokemon, int posicionX, int posicionY) {
	if (crearLista == TRUE) {
		listaCatchPokemon = list_create();
		crearLista = FALSE;
	}
	//pokemon* pokemonEnLista;
	pokemon* pokemonEnCatch = reservarMemoria(sizeof(pokemon));
	pokemonEnCatch->id_mensaje = 0;
	pokemonEnCatch->nombre_pokemon = reservarMemoria(string_length(nombrePokemon));
	pokemonEnCatch->nombre_pokemon = string_duplicate(nombrePokemon);
	pokemonEnCatch->tamanio_nombre = string_length(nombrePokemon);
	pokemonEnCatch->posicion_x = posicionX;
	pokemonEnCatch->posicion_y = posicionY;
	//int enviado = conectar_y_enviar("BROKER", configFile->ipBroker, configFile->puertoBroker, "TEAM", "BROKER", CATCH_POKEMON, pokemon, logger, loggerCatedra);
	//if (enviado != ERROR) log_info(loggerCatedra,"Le envio a la cola APPEARED_POKEMON -> POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",app_poke->nombre_pokemon,app_poke->posicion_x,app_poke->posicion_y);

	//Aca deberia recibir el id del mensaje del broker y ponerselo al entrenador en edMsjEsperado y al pokemon en id_mensaje
	idMensajeEsperado++;
	entrenador->idMsjEsperado = idMensajeEsperado;
	pokemonEnCatch->id_mensaje = idMensajeEsperado;
	//printf("Se agrega el pokemon %s a la lista de Catch Pokemon\n", pokemon->nombre_pokemon);
	list_add(listaCatchPokemon, pokemonEnCatch);
	ciclosEnCPU++;
}

void getPokemon() {

}

void ejecutar() {
	char* algoritmo = configFile->algoritmoPlanificacion;
	char* accion;
	int quantum;
	int posicionProximoAEjecutar = 0;
	double rafagaCPUAccion;
	entrenadorPokemon* entrenador;
	while(true) {
		if (list_size(colaReady) > 0) {
			entrenador = list_get(colaReady, 0);
			accion = entrenador->proximaAccion;
			_Bool entrenadorSinAccion(entrenadorPokemon* entrenador) {
				return string_equals_ignore_case(entrenador->proximaAccion,"");
			}
			if (list_all_satisfy(colaReady, (void *) entrenadorSinAccion)) {
				break;
			}
			//printf("Entrenador en ready numero %i\n", entrenador->idEntrenador);
			if (string_equals_ignore_case("FIFO", algoritmo)) {
				exec = entrenador;
				cantCambiosContexto++;
				pthread_mutex_unlock(&exec->semaforMutex);
				list_remove(colaReady, 0);
					/*for (int i=0; i < list_size(colaReady);i++) {
						entrenadorPokemon* entrenador = list_get(colaReady, i);
						printf("Entrenador posicion %i es %i\n", i, entrenador->idEntrenador);
					}*/
				realizarAccion(entrenador, 0);
			} else if (string_equals_ignore_case("RR", algoritmo)) {
				quantum = configFile->quantum;
				exec = entrenador;
				cantCambiosContexto++;
				pthread_mutex_unlock(&exec->semaforMutex);
				list_remove(colaReady, 0);
				realizarAccion(entrenador, quantum);
			} else if (string_equals_ignore_case("SJF sin desalojo", algoritmo)) {
				rafagaCPUAccion = calcularRafagaCPU(entrenador);
				for (int posicionEntrenador = 1; posicionEntrenador < list_size(colaReady); posicionEntrenador++) {
					entrenadorPokemon* entrenador = list_get(colaReady, posicionEntrenador);
					if (string_equals_ignore_case(entrenador->proximaAccion, "")) {
						double rafagaCPUComparacion = calcularRafagaCPU(entrenador);
						if(rafagaCPUComparacion > rafagaCPUAccion) {
							rafagaCPUAccion = rafagaCPUComparacion;
							accion = entrenador->proximaAccion;
							posicionProximoAEjecutar = posicionEntrenador;
						}
					}
				}
				exec = entrenador;
				cantCambiosContexto++;
				entrenador->estimacionUltimaRafaga = rafagaCPUAccion;
				pthread_mutex_unlock(&exec->semaforMutex);
				list_remove(colaReady, posicionProximoAEjecutar);
				realizarAccion(entrenador, 0);
				} else if (string_equals_ignore_case("SJF con desalojo", algoritmo)){
					rafagaCPUAccion = calcularRafagaCPU(entrenador);
					for (int posicionEntrenador = 1; posicionEntrenador < list_size(colaReady); posicionEntrenador++) {
						entrenadorPokemon* entrenador = list_get(colaReady, posicionEntrenador);
						double rafagaCPUComparacion = calcularRafagaCPU(entrenador);
						if(rafagaCPUComparacion > rafagaCPUAccion) {
							rafagaCPUAccion = rafagaCPUComparacion;
							accion = entrenador->proximaAccion;
							posicionProximoAEjecutar = posicionEntrenador;
						}
					}
					if (exec != NULL) {
						list_add(colaReady, exec);
					}
					exec = entrenador;
					cantCambiosContexto++;
					entrenador->estimacionUltimaRafaga = rafagaCPUAccion;
					pthread_mutex_unlock(&exec->semaforMutex);
					list_remove(colaReady, posicionProximoAEjecutar);
					realizarAccion(entrenador, 0);
				}
		} else {
			break;
		}
	}
}

double calcularRafagaCPU(entrenadorPokemon* entrenador) {
	double rafagaActual = entrenador->estimacionUltimaRafaga;
	int tiempoCPUEntrenador = entrenador->ciclosEnCPU;
	double alpha = configFile->alpha;
	return (alpha * tiempoCPUEntrenador) + ((1 - alpha) * rafagaActual);
}

void realizarAccion(entrenadorPokemon* entrenador, int tiempo) {
	char* arrayAccion = string_new();
	int llegoADestino = 0;
	char* accion = entrenador->proximaAccion;
	if (!string_equals_ignore_case(accion,"")) {
		arrayAccion = string_duplicate(accion);
		char* accionStr = strtok(arrayAccion, " ");
		if (strcmp("AtraparPokemon", accionStr) == 0) {
			char* pokemonAAtrapar = strtok(NULL, " ");
			char* posicionXPokemon = strtok(NULL, " ");
			char* posicionYPokemon = strtok(NULL, " ");
			int posicionXInt = atoi(posicionXPokemon);
			int posicionYInt = atoi(posicionYPokemon);
			//printf("La accion es %s\n", accion);
			llegoADestino = moverEntrenador(entrenador, posicionXInt, posicionYInt, tiempo);
			if (llegoADestino == 1) {
				catchPokemon(entrenador, pokemonAAtrapar, posicionXInt, posicionYInt);
				pthread_mutex_lock(&entrenador->semaforMutex);
				exec = NULL;
				list_add(colaBlocked, entrenador);
				log_info(loggerCatedra, "Se movió al entrenador id %i a la cola de Blocked a la espera del resultado del catch", entrenador->idEntrenador);
				/*for (int i=0; i < list_size(colaBlocked);i++) {
					entrenadorPokemon* entrenador = list_get(colaBlocked, i);
					printf("Entrenador %i en lista blocked\n", entrenador->idEntrenador);
				}*/
			} else {
				log_info(loggerCatedra, "Se movió al entrenador id %i a la cola de Ready para continuar su ejecución proximamente", entrenador->idEntrenador);
				list_add(colaReady, entrenador);
			}
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
			moverEntrenador(entrenador, posicionXEntrenador2Int, posicionYEntrenador2Int, tiempo);
			realizarIntercambio(entrenador, entrenador2, atrapadoInnecesarioEntrenador1, atrapadoInnecesarioEntrenador2);
			entrenador->proximaAccion = "";
			entrenador2->proximaAccion = "";
			verificarDeadlock(entrenador);
			verificarDeadlock(entrenador2);
			//verificarIntercambios();
		}
	} else {
			list_add(colaReady, entrenador);
		}
	}

void inicializar_semaforos(){
	//inicializo semaforos de nodos
	pthread_mutex_init(&mxSocketsFD, NULL);
	pthread_mutex_init(&mxHilos, NULL);
}

void crearHilos() {
	hilo_servidor = 0;
	hilo_consola = 0;
	hilo_conexion = 0;
	pthread_create(&hilo_servidor, NULL, (void*) planificador, NULL);
	pthread_create(&hilo_consola, NULL, (void*) consola, NULL);
	pthread_create(&hilo_conexion, NULL, (void*) reconectar, NULL);
	/*pthread_mutex_init(&h_reconectar,NULL);
	pthread_mutex_lock(&h_reconectar);*/
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
	//free(comando);
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
	//pthread_mutex_lock(&mutexLog);
	log_info(logger,"Obteniendo los Entrenadores");
	//pthread_mutex_unlock(&mutexLog);
	//printf("Obteniendo los entrenadores\n");
	obtenerEntrenadores();
	getPokemon();
	fdTeam = nuevoSocket();
	asociarSocket(fdTeam, configFile->puertoTeam);
	escucharSocket(fdTeam, CONEXIONES_PERMITIDAS);
	pthread_mutex_lock(&mutexLog);
	log_info(logger,"Escuchando conexiones");
	pthread_mutex_unlock(&mutexLog);
	//printf("Escuchando conexiones\n");
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
		int recibido = recibirProtocolo(&head,&bufferTam,comandoNuevo); // recibo head y tamaño de msj
		void * mensaje = malloc(bufferTam);
		if (head < 1 || recibido <= 0){ // DESCONEXIÓN
			//printf("Error al recibir mensaje.\n");
			reconectar();
		}else{
			recibirMensaje(comandoNuevo , bufferTam , mensaje ); // recibo msj serializado para el tratamiento deseado
			log_info(logger,"aplicar_protocolo_recibir -> recibió el HEAD #%d",head);
			log_info(logger,"aplicar_protocolo_recibir -> recibió un tamaño de -> %d",bufferTam);
			log_info(logger,"aplicar_protocolo_recibir -> comienza a deserealizar");
		/*printf("aplicar_protocolo_recibir -> recibió el HEAD #%d\n",head);
		printf("aplicar_protocolo_recibir -> recibió un tamaño de -> %d\n",bufferTam);
		printf("aplicar_protocolo_recibir -> comienza a deserealizar\n");*/
		switch( head ){
		case APPEARED_POKEMON :{
			cola_APPEARED_POKEMON app_poke;
			deserealizar_APPEARED_POKEMON(head, mensaje, bufferTam, & app_poke);
			if (pokemonNecesario(&app_poke) == TRUE) {
				entrenadorPokemon* proximoEntrenadorEnEjecutar = seleccionarEntrenadorMasCercano(&app_poke);
				char* proximaAccionEntrenador = string_new();
				string_append_with_format(&proximaAccionEntrenador, "AtraparPokemon %s %i %i", app_poke.nombre_pokemon, app_poke.posicion_x, app_poke.posicion_y);
				proximoEntrenadorEnEjecutar->proximaAccion = proximaAccionEntrenador;
				//responder por localized_pokemon
				log_info(loggerCatedra,"Recibí en la cola APPEARED_POKEMON . POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",app_poke.nombre_pokemon,app_poke.posicion_x,app_poke.posicion_y);
				//printf("Recibí en la cola APPEARED_POKEMON . POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d \n",app_poke.nombre_pokemon,app_poke.posicion_x,app_poke.posicion_y);
				ejecutar();
				//free(app_poke.nombre_pokemon);
			}
		break;
		}
		case CAUGHT_POKEMON :{
			cola_CAUGHT_POKEMON caug_poke ;
			//responde por caught_pokemon
			deserealizar_CAUGHT_POKEMON(head, mensaje, bufferTam, & caug_poke);
			entrenadorPokemon* entrenador = verificarMensajeRecibido(caug_poke.id_mensaje);
			if (entrenador != NULL) {
				if (caug_poke.atrapo_pokemon == 0) {
					pokemonAtrapado(entrenador, &caug_poke);
				}
			}
			log_info(loggerCatedra,"Recibí en la cola CAUGHT_POKEMON . MENSAJE ID: %d  , ATRAPO: %d",caug_poke.id_mensaje,caug_poke.atrapo_pokemon);
			//printf("Recibí en la cola CAUGHT_POKEMON . MENSAJE ID: %d  , ATRAPO: %d\n",caug_poke.id_mensaje,caug_poke.atrapo_pokemon);
		break;
		}
		case LOCALIZED_POKEMON :{
			cola_LOCALIZED_POKEMON loc_poke ;
			deserealizar_LOCALIZED_POKEMON(head, mensaje, bufferTam, & loc_poke);
			for (int i = 0 ; i < list_size(loc_poke.lista_posiciones); i++){
			log_info(loggerCatedra,"Recibí en la cola LOCALIZED_POKEMON . POKEMON: %s  , CANTIDAD: %d , POSICIÓN X: %d , POSICIÓN Y: %d",loc_poke.nombre_pokemon,loc_poke.cantidad,list_get(loc_poke.lista_posiciones,i),list_get(loc_poke.lista_posiciones,i + 1));
			//printf("Recibí en la cola LOCALIZED_POKEMON . POKEMON: %s  , CANTIDAD: %d , POSICIÓN X: %d , POSICIÓN Y: %d\n",loc_poke.nombre_pokemon,loc_poke.cantidad,list_get(loc_poke.lista_posiciones,i),list_get(loc_poke.lista_posiciones,i + 1));
			i++;
		}
			desBloquearSemaforoEnt(colaNew,1);
			//free(loc_poke.nombre_pokemon);
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
		//ejecutar();
	}
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
		pthread_mutex_lock(&h_reconectar);
		while (conBroker != 1) {
			sleep(configFile->tiempoReconexion);
			log_info(loggerCatedra, "Inicio de proceso de reconexión con broker");
			conBroker = conectarCon(fdBroker,configFile->ipBroker,configFile->puertoBroker,loggerCatedra);
		}
		handshake_cliente(fdBroker,"Team","Broker",logger);
		pthread_mutex_unlock(&h_reconectar);
	}
