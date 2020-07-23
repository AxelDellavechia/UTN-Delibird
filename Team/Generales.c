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
	if (config_get_int_value(config,"TOKEN") != 0 ){
		configFile->token = config_get_int_value(config,"TOKEN");
	}else
	{
		configFile->token = token();
		grabarToken(configFile->token);
	}
	//config_destroy(config); // Libero la estructura archivoConfig
	/*free(posicionEntrenadoresAux);
	free(pokemonEntrenadoresAux);
	free(objetivosEntrenadoresAux);*/
}

void grabarToken(unsigned int token) {
	t_config *config;
	//	config = malloc (sizeof(t_config));
	config = config_create(RUTA_CONFIG_TEAM);
	pthread_mutex_lock(&mutexLog);
	log_info(logger, "Guardar Token");
	pthread_mutex_unlock(&mutexLog);
	if (config != NULL) {
		char* ctoken = string_itoa(token);
		config_set_value(config, "TOKEN", ctoken);
		config_save(config);
		free(ctoken);
	}
	config_destroy(config);
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
	pokemonesBuscados = list_create();
	pokemonesEjecutando = list_create();
	idMensaje = 0;
	crearLista = 1;
	ciclosEnCPU = 0;
	cantCambiosContexto = 0;
	cantDeadlocks = 0;
	getEnviado = FALSE;
	exec = NULL;
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
		entrenador->proximaAccion = malloc(sizeof(""));
		strcpy(entrenador->proximaAccion,"");
		entrenador->estimacionUltimaRafaga = 0;
		//pthread_mutex_lock(&entrenador->semaforMutex);
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
		pthread_mutex_lock(&mutexLogCatedra);
		log_info(loggerCatedra, "Entrenador %i creado y en cola de New", listaEntrenadores);
		pthread_mutex_unlock(&mutexLogCatedra);
		list_add_all(objetivoTeam, entrenador->pokemonesObjetivo);

		//REVISAR ESTO, NO ES LO PEDIDO POR CATEDRA. TIENEN QUE ESTAR EN NEW HASTA QUE SE LOS LLAME
		pthread_mutex_lock(&mutexColaReady);
		list_add(colaReady, entrenador);
		pthread_mutex_unlock(&mutexColaReady);
		pthread_mutex_lock(&mutexLogCatedra);
		log_info(loggerCatedra, "Entrenador %i movido a cola de Ready", listaEntrenadores);
		pthread_mutex_unlock(&mutexLogCatedra);
		list_remove(colaNew, 0);
	}
	pthread_mutex_lock(&mutexColaReady);
	cantEntrenadores = list_size(colaReady);
	pthread_mutex_unlock(&mutexColaReady);
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

void seleccionarEntrenadorMasCercano(posicionPokemon* pokemonAparecido,posicion* pos, entrenadorPokemon* proximoEntrenadorEnEjecutar) {


	int movimientoDeProximoEntrenadorAReady = 0;
	int posicionProximoEntrenador = 0;
	posicion* posPoke = list_get(pokemonAparecido->posiciones,0);
	int posicionXPokemon = pos->posicion_x;
	int posicionYPokemon = pos->posicion_y;
	int sizeReady;
	pthread_mutex_lock(&mutexColaReady);
	sizeReady = list_size(colaReady);
	pthread_mutex_unlock(&mutexColaReady);
	for(int posicionEntrenador = 0; posicionEntrenador < sizeReady; posicionEntrenador++) {
		pthread_mutex_lock(&mutexColaReady);
		entrenadorPokemon* entrenador = list_get(colaReady, posicionEntrenador);
		pthread_mutex_unlock(&mutexColaReady);
		char* proximaAccionEntrenador = malloc(strlen(entrenador->proximaAccion) + 1);
		strcpy(proximaAccionEntrenador, entrenador->proximaAccion);
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
	//printf("Entrenador %i es el más cercano\n", proximoEntrenadorEnEjecutar->idEntrenador);
}

int moverEntrenador(entrenadorPokemon* entrenador, int posicionXDestino, int posicionYDestino) {

	int tiempo = 0;
	if (string_equals_ignore_case("RR", configFile->algoritmoPlanificacion)){
		tiempo = configFile->quantum;
	}

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
	pthread_mutex_lock(&mutexLogCatedra);
	log_info(loggerCatedra, "Entrenador %i se movió a la posición %i %i", entrenador->idEntrenador, entrenador->posicion_x, entrenador->posicion_y);
	pthread_mutex_unlock(&mutexLogCatedra);
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
	pthread_mutex_lock(&mutexLogCatedra);
	log_info(loggerCatedra, "Se realizó el intercambio del pokemon %s del entrenador %i por el pokemon %s del entrenador %i", pokemonEntrenador1, entrenador1->idEntrenador, pokemonEntrenador2, entrenador2->idEntrenador);
	pthread_mutex_unlock(&mutexLogCatedra);
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
			pthread_mutex_lock(&mutexLogCatedra);
			log_info(loggerCatedra, "Se movió al entrenador de id %i a Exit ya que atrapó a todos los Pokemon que tenía como objetivo", entrenador->idEntrenador);
			pthread_mutex_unlock(&mutexLogCatedra);
			if (cantEntrenadores == list_size(colaExit)) {
				pthread_mutex_lock(&mutexLogCatedra);
				log_info(loggerCatedra, "Cantidad de ciclos de CPU totales: %i", ciclosEnCPU);
				log_info(loggerCatedra, "Cantidad de cambios de contexto realizados: %i", cantCambiosContexto);
				log_info(loggerCatedra, "Cantidad de ciclos de CPU realizados por entrenador:");
				pthread_mutex_unlock(&mutexLogCatedra);
				for (int posicionEntrenador = 0; posicionEntrenador < list_size(colaExit); posicionEntrenador++) {
					entrenadorPokemon* entrenadorEnExit = list_get(colaExit, posicionEntrenador);
					pthread_mutex_lock(&mutexLogCatedra);
					log_info(loggerCatedra, "Entrenador %i - Ciclos de CPU: %i", entrenadorEnExit->idEntrenador, entrenadorEnExit->ciclosEnCPU);
					pthread_mutex_unlock(&mutexLogCatedra);
				}
				pthread_mutex_lock(&mutexLogCatedra);
				log_info(loggerCatedra, "Deadlocks producidos y resueltos: %i", cantDeadlocks);
				pthread_mutex_unlock(&mutexLogCatedra);
			}
			//printf("Se movio al entrenador de id %i a Exit\n", entrenador->idEntrenador);
		}
	} else {
		pthread_mutex_lock(&mutexColaReady);
		list_add(colaReady, entrenador);
		pthread_mutex_unlock(&mutexColaReady);
		//printf("Se movio al entrenador de id %i a la cola de Ready\n", entrenador->idEntrenador);
		pthread_mutex_lock(&mutexLogCatedra);
		log_info(loggerCatedra, "Se movió al entrenador de id %i de la cola de Blocked a la cola de Ready ya que no completó sus objetivos", entrenador->idEntrenador);
		pthread_mutex_unlock(&mutexLogCatedra);
		/*for (int i=0; i < list_size(colaReady);i++) {
			entrenadorPokemon* entrenador = list_get(colaReady, i);
			printf("Entrenador posicion %i es %i\n", i, entrenador->idEntrenador);
		}*/
		quitarDeColaBlocked(entrenador);
		entrenador->proximaAccion = "";
		sem_post(&semEntrenadores);
	}
}

void quitarDeColaBlocked(entrenadorPokemon* entrenador) {
	pthread_mutex_lock(&mutexColaBlocked);
	for(int posicionEnListaBlocked = 0; posicionEnListaBlocked < list_size(colaBlocked); posicionEnListaBlocked++) {
		entrenadorPokemon* entrenadorBlockeado = list_get(colaBlocked, posicionEnListaBlocked);
		if (entrenadorBlockeado->idEntrenador == entrenador->idEntrenador) {
			list_remove(colaBlocked, posicionEnListaBlocked);
			break;
		}
	}
	pthread_mutex_unlock(&mutexColaBlocked);
}

void verificarIntercambios() {
	if (list_size(entrenadoresEnDeadlock) > 1) {
		pthread_mutex_lock(&mutexLogCatedra);
		log_info(loggerCatedra, "Inicia algoritmo de detección de deadlock");
		pthread_mutex_unlock(&mutexLogCatedra);
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
					entrenador1->proximaAccion = malloc(sizeof(""));
					strcpy(entrenador1->proximaAccion,"");
					pthread_mutex_lock(&mutexLogCatedra);
					log_info(loggerCatedra, "Se detectó deadlock entre los entrenadores %i y %i", entrenador1->idEntrenador, idEntrenador2);
					pthread_mutex_unlock(&mutexLogCatedra);
					cantDeadlocks++;
					char* id2 = string_itoa(idEntrenador2);
					char* pos_x2 = string_itoa(posicionXEntrenador2);
					char* pos_y2 = string_itoa(posicionYEntrenador2);
					char* proximaAccionEntrenador = malloc(string_length("HacerIntercambio     ") + string_length(id2) + string_length(pos_x2) + string_length(pos_y2) + string_length(atrapadoInnecesarioEntrenador1) + string_length(atrapadoInnecesarioEntrenador2) + 1);
					strcpy(proximaAccionEntrenador, "HacerIntercambio ");
					strcat(proximaAccionEntrenador, id2);
					strcat(proximaAccionEntrenador, " ");
					strcat(proximaAccionEntrenador, pos_x2);
					strcat(proximaAccionEntrenador, " ");
					strcat(proximaAccionEntrenador, pos_y2);
					strcat(proximaAccionEntrenador, " ");
					strcat(proximaAccionEntrenador, atrapadoInnecesarioEntrenador1);
					strcat(proximaAccionEntrenador, " ");
					strcat(proximaAccionEntrenador, atrapadoInnecesarioEntrenador2);
					entrenador1->proximaAccion = malloc(strlen(proximaAccionEntrenador) + 1);
					strcpy(entrenador1->proximaAccion, proximaAccionEntrenador);
					//log_info(loggerCatedra, "Se realizará un intercambio entre los entrenadores %i y %i", idEntrenador2, entrenador1->idEntrenador);
					/*string_append_with_format(&proximaAccionEntrenador, "HacerIntercambio %i %i %i %s %s", idEntrenador2, posicionXEntrenador2, posicionYEntrenador2, atrapadoInnecesarioEntrenador1, atrapadoInnecesarioEntrenador2);
					entrenador1->proximaAccion = proximaAccionEntrenador;*/
					quitarDeColaBlocked(entrenador1);
					pthread_mutex_lock(&mutexColaReady);
					list_add(colaReady, entrenador1);
					pthread_mutex_unlock(&mutexColaReady);
					//ejecutar();
					//realizarIntercambio(entrenador1, entrenador2, atrapadoInnecesarioEntrenador1, atrapadoInnecesarioEntrenador2);
					/*verificarDeadlock(entrenador1);
					verificarDeadlock(entrenador2);
					verificarIntercambios();*/
					free(id2);
					free(pos_x2);
					free(pos_y2);
					free(proximaAccionEntrenador);
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
	pthread_mutex_lock(&mutexColaBlocked);
	for (int posicionEntrenador = 0; posicionEntrenador < list_size(colaBlocked); posicionEntrenador++) {
		entrenador = list_get(colaBlocked, posicionEntrenador);
		if (idMensajeRecibido == entrenador->idMsjEsperado) {
			break;
		}
	}
	pthread_mutex_unlock(&mutexColaBlocked);
	return entrenador;
}

//Agrega el pokemon atrapado a la lista de pokemones atrapados del entrenador, quita al pokemon de la lista CatchPokemon y verifica si el entrenador esta en Deadlock
void pokemonAtrapado(entrenadorPokemon* entrenador, cola_CAUGHT_POKEMON* pokemonRecibido) {

	for(int posicionPokemon = 0; posicionPokemon < list_size(listaCatchPokemon); posicionPokemon++) {
		cola_CATCH_POKEMON* pokemonCatch = list_get(listaCatchPokemon, posicionPokemon);
		if(pokemonCatch->id_mensaje == pokemonRecibido->id_mensaje) {
			entrenador->proximaAccion = "";
			list_add(entrenador->pokemonesAtrapados, string_duplicate(pokemonCatch->nombre_pokemon));
			for (int posicionObjetivo = 0; posicionObjetivo < list_size(objetivoTeam); posicionObjetivo++) {
				char* pokemonObjetivo = list_get(objetivoTeam, posicionObjetivo);
				if (string_equals_ignore_case(pokemonObjetivo, pokemonCatch->nombre_pokemon)){
					list_remove(objetivoTeam, posicionObjetivo);

					//como ya no son parte del objetivo, tambien debo sacarlos de la lista de buscados
					pthread_mutex_lock(&mutexBuscados);
					_Bool quitar_poke(pokemon * lista){return string_equals_ignore_case(lista->nombre_pokemon, pokemonCatch->nombre_pokemon);}
					list_remove_by_condition(pokemonesBuscados, (void*)quitar_poke);
					pthread_mutex_unlock(&mutexBuscados);

					break;
				}
			}
			list_remove(listaCatchPokemon, posicionPokemon);
			verificarDeadlock(entrenador);
			break;
		}
	}
}

int pokemonNecesario(char* pokemonAparecido) {

	for (int posicionObjetivo = 0; posicionObjetivo < list_size(objetivoTeam); posicionObjetivo++) {
		char* pokemonObjetivo = list_get(objetivoTeam, posicionObjetivo);
		if (string_equals_ignore_case(pokemonObjetivo, pokemonAparecido)){
			_Bool buscar_poke(posicionPokemon * lista){return string_equals_ignore_case(lista->tipoPokemon, pokemonAparecido);}
			_Bool yaLoTengo = list_any_satisfy(pokemonesBuscados, (void*)buscar_poke);
			if(yaLoTengo){
				return FALSE;
			}else{
				return TRUE;
			}

		}
	}
	return FALSE;
}

int mensajeNoRecibido(cola_LOCALIZED_POKEMON* pokemonLocalizado) {
	for(int posicionPokemon = 0; posicionPokemon < list_size(pokemonesBuscados); posicionPokemon++) {
		char* pokemonBuscado = list_get(pokemonesBuscados, posicionPokemon);
		if(string_equals_ignore_case(pokemonBuscado, pokemonLocalizado->nombre_pokemon)){
			return FALSE;
		}
	}
	return TRUE;
}

void catchPokemon(entrenadorPokemon* entrenador, char* nombrePokemon, int posicionX, int posicionY) {
	if (crearLista == TRUE) {
		listaCatchPokemon = list_create();
		crearLista = FALSE;
	}
	int head , bufferTam;
	//pokemon* pokemonEnLista;
	pthread_mutex_lock(&mxListaCatch);
	pthread_mutex_lock(&mxListaEjecutando);
	_Bool buscar_poke(pokemon * lista){return lista->posicion_x == posicionX && lista->posicion_y == posicionY && string_equals_ignore_case(lista->nombre_pokemon, nombrePokemon);}
    list_remove_by_condition(pokemonesEjecutando, (void*)buscar_poke);

	pokemon* pokemonEnCatch = malloc(sizeof(pokemon));
	pokemonEnCatch->id_mensaje = 0;
	pokemonEnCatch->nombre_pokemon = malloc(string_length(nombrePokemon) + 1);
	pokemonEnCatch->nombre_pokemon = string_duplicate(nombrePokemon);
	pokemonEnCatch->tamanio_nombre = string_length(nombrePokemon);
	pokemonEnCatch->posicion_x = posicionX;
	pokemonEnCatch->posicion_y = posicionY;
	//pthread_mutex_lock(&entrenador->semaforMutex);

	exec = NULL;
	pthread_mutex_lock(&mutexColaBlocked);
	list_add(colaBlocked, entrenador);
	pthread_mutex_unlock(&mutexColaBlocked);
	pthread_mutex_lock(&mutexLogCatedra);
	log_info(loggerCatedra, "Se movió al entrenador id %i a la cola de Blocked a la espera del resultado del catch", entrenador->idEntrenador);
	pthread_mutex_unlock(&mutexLogCatedra);
	pthread_mutex_lock(&mutexLogCatedra);


	pthread_mutex_lock(&mutexLog);
	int socket = nuevoSocket();
	int result = conectarCon( socket ,configFile->ipBroker,configFile->puertoBroker,logger);
	if (result != ERROR){
			handshake_cliente(socket,"Team","Broker",logger);
			result = aplicar_protocolo_enviar(socket,CATCH_POKEMON,pokemonEnCatch);
			if(result != ERROR){

				int recibido = recibirProtocolo(&head,&bufferTam,socket);
				void * mensaje = malloc(bufferTam);
				recibirMensaje(socket, bufferTam, mensaje);
				if(head == ACK){
					respuesta_ACK * ack = malloc(sizeof(respuesta_ACK));
					deserealizar_ACK( head, mensaje, bufferTam, & ack);
					pokemonEnCatch->id_mensaje = ack->id_msj;

					list_add(listaCatchPokemon, pokemonEnCatch);

				}else{
					result=ERROR;
				}
						//pokemonEnCatch->id_mensaje = idRecibido;
			}			//entrenador->idMsjEsperado = idRecibido;
	}
	if (result == ERROR || result == FALSE){
			log_info(loggerCatedra, "No se pudo conectar por el Broker, se considera al Pokemon %s atrapado por el entrenador id %i",pokemonEnCatch->nombre_pokemon, entrenador->idEntrenador);

			for (int posicionObjetivo = 0; posicionObjetivo < list_size(objetivoTeam); posicionObjetivo++) {
					char* pokemonObjetivo = list_get(objetivoTeam, posicionObjetivo);
					if (string_equals_ignore_case(pokemonObjetivo, pokemonEnCatch->nombre_pokemon)){
							list_remove(objetivoTeam, posicionObjetivo);

							//como ya no son parte del objetivo, tambien debo sacarlos de la lista de buscados
							pthread_mutex_lock(&mutexBuscados);
							_Bool quitar_poke(pokemon * lista){return string_equals_ignore_case(lista->nombre_pokemon, pokemonEnCatch->nombre_pokemon);}
							list_remove_by_condition(pokemonesBuscados, (void*)quitar_poke);
							pthread_mutex_unlock(&mutexBuscados);
							break;
					}
			}
			//pokemonAtrapado(entrenador, pokemonEnCatch);
	}
		pthread_mutex_unlock(&mutexLogCatedra);
		pthread_mutex_unlock(&mutexLog);
		pthread_mutex_unlock(&mxListaCatch);
		pthread_mutex_unlock(&mxListaEjecutando);
		//aplicar_protocolo_enviar(fdBroker, CATCH_POKEMON, pokemonEnCatch);
		//int recibido = recibirProtocolo(&head,&bufferTam,fdBroker);
		//void * mensaje = malloc(bufferTam);
		//recibirMensaje(fdBroker, bufferTam, mensaje);
		//pokemonEnCatch->id_mensaje = idRecibido;
		//entrenador->idMsjEsperado = idRecibido;

	/*} else {
		list_add(listaCatchPokemon, pokemonEnCatch);
		cola_CAUGHT_POKEMON* pokemonCaught = reservarMemoria(sizeof(cola_CAUGHT_POKEMON));
		pokemonCaught->atrapo_pokemon = OK;
		pokemonCaught->id_mensaje = idMensaje;
		entrenador->idMsjEsperado = idMensaje;
		pokemonAtrapado(entrenador, pokemonCaught);
	}*/
	//int enviado = conectar_y_enviar("BROKER", configFile->ipBroker, configFile->puertoBroker, "TEAM", "BROKER", CATCH_POKEMON, pokemon, logger, loggerCatedra);
	//if (enviado != ERROR) log_info(loggerCatedra,"Le envio a la cola APPEARED_POKEMON -> POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",app_poke->nombre_pokemon,app_poke->posicion_x,app_poke->posicion_y);
	//Aca deberia recibir el id del mensaje del broker y ponerselo al entrenador en edMsjEsperado y al pokemon en id_mensaje
	//printf("Se agrega el pokemon %s a la lista de Catch Pokemon\n", pokemon->nombre_pokemon);
	ciclosEnCPU++;
}

void getPokemon() {
	int getYaRealizado;
	char* primerObjetivo = list_get(objetivoTeam, 0);
	cola_GET_POKEMON* pokemonAGet = malloc(sizeof(cola_GET_POKEMON));
	pokemonAGet->id_mensaje = 0;
	pokemonAGet->nombre_pokemon = malloc(sizeof(primerObjetivo + 1));
	strcpy(pokemonAGet->nombre_pokemon, primerObjetivo);
	pokemonAGet->tamanio_nombre = strlen(primerObjetivo);
	pthread_mutex_lock(&mutexLogCatedra);
	pthread_mutex_lock(&mutexLog);
	conectar_y_enviar("BROKER" , configFile->ipBroker,configFile->puertoBroker, "Team" , "Broker",GET_POKEMON, pokemonAGet,logger ,loggerCatedra );
	pthread_mutex_unlock(&mutexLogCatedra);
	pthread_mutex_unlock(&mutexLog);
	//aplicar_protocolo_enviar(fdBroker, GET_POKEMON, primerObjetivo);
	for (int posicionObjetivo1 = 1; posicionObjetivo1 < list_size(objetivoTeam); posicionObjetivo1++) {
		getYaRealizado = FALSE;
		char* pokemonObjetivo1 = list_get(objetivoTeam, posicionObjetivo1);
		for (int posicionObjetivo2 = 0; posicionObjetivo2 < posicionObjetivo1; posicionObjetivo2++) {
			char* pokemonObjetivo2 = list_get(objetivoTeam, posicionObjetivo2);
			if (string_equals_ignore_case(pokemonObjetivo1, pokemonObjetivo2)) {
				getYaRealizado = TRUE;
				break;
			}
		}
		if (getYaRealizado == FALSE) {
			cola_GET_POKEMON* pokemonAGet1 = malloc(sizeof(cola_GET_POKEMON));
			pokemonAGet1->id_mensaje = 0;
			pokemonAGet1->nombre_pokemon = malloc(sizeof(pokemonObjetivo1 + 1));
			strcpy(pokemonAGet1->nombre_pokemon, pokemonObjetivo1);
			pokemonAGet1->tamanio_nombre = strlen(pokemonObjetivo1);
			pthread_mutex_lock(&mutexLogCatedra);
			pthread_mutex_lock(&mutexLog);
			conectar_y_enviar("BROKER" , configFile->ipBroker,configFile->puertoBroker, "Team" , "Broker",GET_POKEMON, pokemonAGet1 ,logger ,loggerCatedra );
			pthread_mutex_unlock(&mutexLogCatedra);
			pthread_mutex_unlock(&mutexLog);
			//aplicar_protocolo_enviar(fdBroker, GET_POKEMON, pokemonObjetivo1);
		}
	}
}

void planificador() {

	char* accion;
	int sizeReady;
	int posicionProximoAEjecutar = 0;
	double rafagaCPUAccion;
	entrenadorPokemon* entrenador;
	while(TRUE) {
		sem_wait(&semPokemonesBuscados);
		sem_wait(&semEntrenadores);
		pthread_mutex_lock(&mutexColaReady);
		sizeReady = list_size(colaReady);
		pthread_mutex_unlock(&mutexColaReady);


		if (sizeReady > 0) {
			pthread_mutex_lock(&mutexBuscados);
			//cola_APPEARED_POKEMON* app_poke =
			_Bool pokeSeleccionado = false;
			posicionPokemon* buscarPoke = malloc(sizeof(posicionPokemon));
			posicion *pos = malloc(sizeof(posicion));
			pthread_mutex_lock(&mxListaCatch);
			pthread_mutex_lock(&mxListaEjecutando);
			for(int i = 0;i < list_size(pokemonesBuscados) && !pokeSeleccionado; i++){
				buscarPoke = list_get(pokemonesBuscados,i);
				for(int j = 0;j < list_size(buscarPoke->posiciones) && !pokeSeleccionado; j++){
					pos = list_get(buscarPoke->posiciones,j);

					//AHORA VERIFICO SI EL POKEMON BUSCADO NO ESTÁ EN CATCH O EJECUTANDOSE (NO DEBERÍAN)
					_Bool buscar_poke(pokemon * lista){return lista->posicion_x == pos->posicion_x && lista->posicion_y == pos->posicion_y && string_equals_ignore_case(lista->nombre_pokemon, buscarPoke->tipoPokemon);}
					pokeSeleccionado = list_any_satisfy(listaCatchPokemon, (void*)buscar_poke);
					if(!pokeSeleccionado){
						pokeSeleccionado = list_any_satisfy(pokemonesEjecutando, (void*)buscar_poke);
					}
				}
			}

			pthread_mutex_unlock(&mutexBuscados);
			posicion* posPoke = list_get(buscarPoke->posiciones,0);

			entrenadorPokemon* proximoEntrenadorEnEjecutar = malloc(sizeof(entrenadorPokemon));
			seleccionarEntrenadorMasCercano(buscarPoke, pos, proximoEntrenadorEnEjecutar);

			pokemon* pokeEjecutando = malloc( sizeof(pokemon));
			pokeEjecutando->nombre_pokemon = malloc(string_length(buscarPoke->tipoPokemon));
			strcpy(pokeEjecutando->nombre_pokemon,buscarPoke->tipoPokemon);
			pokeEjecutando->posicion_x = pos->posicion_x;
			pokeEjecutando->posicion_y = pos->posicion_y;
			list_add(pokemonesEjecutando,pokeEjecutando);

			pthread_mutex_lock(&mxListaCatch);
			pthread_mutex_lock(&mxListaEjecutando);

			//COMO YA LO AGREGUE A LA COLA DE EJECUCION LO SACO DE LOS POKEMONES QUE ESTOY BUSCANDO
			pthread_mutex_lock(&mutexBuscados);
			if(list_size(buscarPoke->posiciones) <= 1){
				_Bool buscar_poke(pokemon * lista){return string_equals_ignore_case(lista->nombre_pokemon, buscarPoke->tipoPokemon);}
			    list_remove_by_condition(pokemonesBuscados, (void*)buscar_poke);
			}else{
				_Bool buscar_pos(pokemon * lista){return lista->posicion_x == pos->posicion_x && lista->posicion_y == pos->posicion_y;}
			    list_remove_by_condition(buscarPoke->posiciones, (void*)buscar_pos);
			}
			pthread_mutex_unlock(&mutexBuscados);

			char* pos_x = string_itoa(posPoke->posicion_x);
			char* pos_y = string_itoa(posPoke->posicion_y);
			char* proximaAccionEntrenador = malloc(string_length("AtraparPokemon   ") + string_length(buscarPoke->tipoPokemon) + string_length(pos_x) + string_length(pos_y) + 1);
			strcpy(proximaAccionEntrenador, "AtraparPokemon ");
			strcat(proximaAccionEntrenador, buscarPoke->tipoPokemon);
			strcat(proximaAccionEntrenador, " ");
			strcat(proximaAccionEntrenador, pos_x);
			strcat(proximaAccionEntrenador, " ");
			strcat(proximaAccionEntrenador, pos_y);
			//string_append_with_format(&proximaAccionEntrenador, "AtraparPokemon %s %i %i", app_poke.nombre_pokemon, app_poke.posicion_x, app_poke.posicion_y);
			proximoEntrenadorEnEjecutar->proximaAccion = malloc(strlen(proximaAccionEntrenador) + 1);
			strcpy(proximoEntrenadorEnEjecutar->proximaAccion, proximaAccionEntrenador);
			pthread_mutex_lock(&mutexColaReady);
			for(int posicionEnReady = 0; posicionEnReady < list_size(colaReady); posicionEnReady++){
				entrenadorPokemon* entrenador = list_get(colaReady, posicionEnReady);
				if (entrenador->idEntrenador == proximoEntrenadorEnEjecutar->idEntrenador) {
					list_remove(colaReady, posicionEnReady);
					break;
					}
			}

			//Lo saca y lo pone arriba?
			list_add_in_index(colaReady, 0, proximoEntrenadorEnEjecutar);
			pthread_mutex_unlock(&mutexColaReady);
			pthread_mutex_lock(&mutexLogCatedra);
			log_info(loggerCatedra, "Entrenador %i a cola de Ready por ser el más cercano", proximoEntrenadorEnEjecutar->idEntrenador);
			pthread_mutex_unlock(&mutexLogCatedra);
			//responder por localized_pokemon
			//printf("Recibí en la cola APPEARED_POKEMON . POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d \n",app_poke.nombre_pokemon,app_poke.posicion_x,app_poke.posicion_y);
			//free(app_poke.nombre_pokemon);
			free(pos_x);
			free(pos_y);
			free(proximaAccionEntrenador);

		/*
			pthread_mutex_lock(&mutexColaReady);
			entrenador = list_get(colaReady, 0);    //ESTO ESTA MAL, AGARRA AL PRIMER ENTRENADOR DE LA LISTA Y NO AL QUE ESTÉ MAS CERCA
			pthread_mutex_unlock(&mutexColaReady);

	*/
			accion = malloc(entrenador->proximaAccion);
			strcpy(accion, entrenador->proximaAccion);

			/*_Bool entrenadorSinAccion(entrenadorPokemon* entrenador) {
				return string_equals_ignore_case(entrenador->proximaAccion,"");
			}
			if (list_all_satisfy(colaReady, (void *) entrenadorSinAccion)) {
				break;
			}*/
			//printf("Entrenador en ready numero %i\n", entrenador->idEntrenador);
			if (string_equals_ignore_case("FIFO", configFile->algoritmoPlanificacion)) {
				exec = entrenador;
				cantCambiosContexto++;
				pthread_mutex_lock(&mutexColaReady);
				list_remove(colaReady, 0);
				pthread_mutex_unlock(&mutexColaReady);
				pthread_mutex_lock(&mxEjecutando);
				pthread_mutex_unlock(&exec->semaforMutex);
					/*for (int i=0; i < list_size(colaReady);i++) {
						entrenadorPokemon* entrenador = list_get(colaReady, i);
						printf("Entrenador posicion %i es %i\n", i, entrenador->idEntrenador);
					}*/
				//realizarAccion(entrenador, 0);
			} else if (string_equals_ignore_case("RR", configFile->algoritmoPlanificacion)) {
				//quantum = configFile->quantum;
				exec = entrenador;
				cantCambiosContexto++;
				pthread_mutex_lock(&mutexColaReady);
				list_remove(colaReady, 0);
				pthread_mutex_unlock(&mutexColaReady);

				pthread_mutex_unlock(&exec->semaforMutex);
				//realizarAccion(entrenador, quantum);
			} else if (string_equals_ignore_case("SJF sin desalojo", configFile->algoritmoPlanificacion)) {
				rafagaCPUAccion = calcularRafagaCPU(entrenador);
				for (int posicionEntrenador = 1; posicionEntrenador < sizeReady; posicionEntrenador++) {
					pthread_mutex_lock(&mutexColaReady);
					entrenadorPokemon* entrenador = list_get(colaReady, posicionEntrenador);
					pthread_mutex_unlock(&mutexColaReady);
					if (string_equals_ignore_case(entrenador->proximaAccion, "")) {
						double rafagaCPUComparacion = calcularRafagaCPU(entrenador);
						if(rafagaCPUComparacion > rafagaCPUAccion) {
							rafagaCPUAccion = rafagaCPUComparacion;
							accion = malloc(sizeof(entrenador->proximaAccion));
							strcpy(accion, entrenador->proximaAccion);
							posicionProximoAEjecutar = posicionEntrenador;
						}
					}
				}
				exec = entrenador;
				cantCambiosContexto++;
				entrenador->estimacionUltimaRafaga = rafagaCPUAccion;
				pthread_mutex_unlock(&exec->semaforMutex);
				pthread_mutex_lock(&mutexColaReady);
				list_remove(colaReady, posicionProximoAEjecutar);
				pthread_mutex_unlock(&mutexColaReady);
				realizarAccion(entrenador, 0);
				} else if (string_equals_ignore_case("SJF con desalojo", configFile->algoritmoPlanificacion)){
					rafagaCPUAccion = calcularRafagaCPU(entrenador);
					for (int posicionEntrenador = 1; posicionEntrenador < sizeReady; posicionEntrenador++) {
						pthread_mutex_lock(&mutexColaReady);
						entrenadorPokemon* entrenador = list_get(colaReady, posicionEntrenador);
						pthread_mutex_unlock(&mutexColaReady);
						double rafagaCPUComparacion = calcularRafagaCPU(entrenador);
						if(rafagaCPUComparacion > rafagaCPUAccion) {
							rafagaCPUAccion = rafagaCPUComparacion;
							accion = malloc(sizeof(entrenador->proximaAccion));
							strcpy(accion, entrenador->proximaAccion);
							posicionProximoAEjecutar = posicionEntrenador;
						}
					}
					if (exec != NULL) {
						pthread_mutex_lock(&mutexColaReady);
						list_add(colaReady, exec);
						pthread_mutex_unlock(&mutexColaReady);
					}
					if ((exec == NULL) || (exec->idEntrenador != entrenador->idEntrenador)) {
						exec = entrenador;
						cantCambiosContexto++;
						entrenador->estimacionUltimaRafaga = rafagaCPUAccion;
					}
					pthread_mutex_unlock(&exec->semaforMutex);
					pthread_mutex_lock(&mutexColaReady);
					list_remove(colaReady, posicionProximoAEjecutar);
					pthread_mutex_unlock(&mutexColaReady);
					realizarAccion(entrenador, 0);
				}
		} /*else {
			break;
		}*/
		//pthread_mutex_unlock(&mutex_ejecutar);
	}
}

double calcularRafagaCPU(entrenadorPokemon* entrenador) {
	double rafagaActual = entrenador->estimacionUltimaRafaga;
	int tiempoCPUEntrenador = entrenador->ciclosEnCPU;
	double alpha = configFile->alpha;
	return (alpha * tiempoCPUEntrenador) + ((1 - alpha) * rafagaActual);
}

void realizarAccion(entrenadorPokemon* entrenador, int tiempo) {

	}

void inicializar_semaforos(){
	//inicializo semaforos de nodos
	pthread_mutex_init(&mxSocketsFD, NULL);
	pthread_mutex_init(&mxHilos, NULL);
	pthread_mutex_init(&mutex_idMensaje, NULL);
	pthread_mutex_init(&mutexColaReady, NULL);
	pthread_mutex_init(&mutexColaBlocked, NULL);
	pthread_mutex_init(&mutexBuscados, NULL);
	pthread_mutex_init(&mxEjecutando, NULL);
	pthread_mutex_init(&mxListaEjecutando, NULL);
	pthread_mutex_init(&mxListaCatch, NULL);


	//semEntrenadores = malloc(sizeof(sem_t));
	sem_init(&semEntrenadores, 0, cantEntrenadores);
	sem_init(&semPokemonesBuscados, 0, 0);
}

void crearHilos() {
	hilo_servidor = 0;
	hilo_gameboy = 0;
	hilo_consola = 0;
	hilo_conexion = 0;
	hilo_ejecucion = 0;

	hilo_APPEARED = 0;
	hilo_LOCALIZED = 0;
	hilo_CAUGHT = 0;


	//pthread_create(&hilo_servidor, NULL, (void*) planificador_Broker, NULL);
	pthread_create(&hilo_APPEARED, NULL, (void*) suscripcion_APPEARED_POKEMON, NULL);
	pthread_create(&hilo_LOCALIZED, NULL, (void*) suscripcion_LOCALIZED_POKEMON, NULL);
	pthread_create(&hilo_CAUGHT, NULL, (void*) suscripcion_CAUGHT_POKEMON, NULL);
	pthread_create(&hilo_gameboy, NULL, (void*) planificador_GameBoy, NULL);
	pthread_create(&hilo_consola, NULL, (void*) consola, NULL);
	pthread_create(&hilo_ejecucion, NULL, (void*) planificador, NULL);

	/*pthread_mutex_init(&h_reconectar,NULL);
	pthread_mutex_lock(&h_reconectar);*/
	//pthread_join(hilo_servidor, NULL);
	pthread_join(hilo_gameboy, NULL);
	pthread_join(hilo_consola, NULL);
	pthread_join(hilo_ejecucion, NULL);

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
			pthread_mutex_lock(&mutexLog);
			log_error(logger,"Error en getline");
			pthread_mutex_unlock(&mutexLog);
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


 	pthread_cancel(hilo_APPEARED);
 	pthread_detach(hilo_APPEARED);
 	pthread_cancel(hilo_LOCALIZED);
 	pthread_detach(hilo_LOCALIZED);
 	pthread_cancel(hilo_CAUGHT);
 	pthread_detach(hilo_CAUGHT);

	pthread_cancel(hilo_servidor);
	pthread_detach( pthread_self() );
}

void servidor() {
	fdTeam = nuevoSocket();
	asociarSocket(fdTeam, configFile->puertoTeam);
	escucharSocket(fdTeam, CONEXIONES_PERMITIDAS);
	pthread_mutex_lock(&mutexLog);
	log_info(logger,"Escuchando conexiones");
	pthread_mutex_unlock(&mutexLog);
	while(TRUE) {
		int conexionNueva = 0;
		int comandoNuevo;//= reservarMemoria(INT);
		while(conexionNueva == 0) {
			comandoNuevo = aceptarConexionSocket(fdTeam);
			pthread_mutex_lock(&mutexLog);
			conexionNueva = handshake_servidor ( comandoNuevo,"Team" , "Broker",logger);
			if(!validar_conexion(conexionNueva, 0,logger) ) {
				pthread_mutex_lock(&mxSocketsFD); //desbloquea el semaforo
				cerrarSocket(fdTeam);
				pthread_mutex_unlock(&mxSocketsFD);
			}
			pthread_mutex_unlock(&mutexLog);
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

void sendACK( int idMsj){
	respuesta_ACK ack;
	ack.ack = OK;
	ack.id_msj=idMsj;
	ack.token = 0;//config_File->TOKEN;
	pthread_mutex_lock(&mutexLogCatedra);
	pthread_mutex_lock(&mutexLog);
	conectar_y_enviar("BROKER" , configFile->ipBroker,configFile->puertoBroker, "Team" , "Broker",ACK, &ack ,logger ,loggerCatedra );
	pthread_mutex_unlock(&mutexLogCatedra);
	pthread_mutex_unlock(&mutexLog);
}

void suscripcion_APPEARED_POKEMON() {
	pthread_mutex_lock(&mutexLog);
	log_info(logger,"Obteniendo los Entrenadores");
	pthread_mutex_unlock(&mutexLog);
	//printf("Obteniendo los entrenadores\n");
	int fdAppeared = nuevoSocket();
	//asociarSocket(fdBroker, configFile->puertoBroker);
	//escucharSocket(fdBroker, CONEXIONES_PERMITIDAS);
	pthread_mutex_lock(&mutexLog);
	log_info(logger,"Escuchando conexiones");
	pthread_mutex_unlock(&mutexLog);
	//printf("Escuchando conexiones\n");

	int head , bufferTam;
	while(TRUE) {
		pthread_mutex_lock(&mutexLogCatedra);
		conBroker = conectarCon(fdAppeared,configFile->ipBroker,configFile->puertoBroker,loggerCatedra);
		pthread_mutex_unlock(&mutexLogCatedra);

		if(conBroker == TRUE){
			pthread_mutex_lock(&mutexLog);
			conBroker = handshake_cliente(fdAppeared, "Team" , "Broker", logger);
			pthread_mutex_unlock(&mutexLog);
		}
		if(conBroker == TRUE){
			suscriptor laSuscripcion;// = malloc(sizeof(suscriptor));
			laSuscripcion.modulo = TEAM; // @suppress("Symbol is not resolved")
			laSuscripcion.token = configFile->token;//config_File->TOKEN;
			laSuscripcion.cola_a_suscribir = list_create();
			list_add(laSuscripcion.cola_a_suscribir, APPEARED_POKEMON); // @suppress("Symbol is not resolved")
			aplicar_protocolo_enviar(fdAppeared, SUSCRIPCION, &laSuscripcion);

			list_destroy(laSuscripcion.cola_a_suscribir);
		}
		while (conBroker == TRUE) {

			int recibido = recibirProtocolo(&head,&bufferTam,fdAppeared); // recibo head y tamaño de msj
			void * mensaje = malloc(bufferTam);
			if (head < 1 || recibido <= 0){ // DESCONEXIÓN, DEBE RECONECTARSE, SALE DEL WHILE Y SE RECONECTA
				conBroker = FALSE;
			}else{
				recibirMensaje(fdAppeared , bufferTam , mensaje ); // recibo msj serializado para el tratamiento deseado
				pthread_mutex_lock(&mutexLog);
				log_info(logger,"aplicar_protocolo_recibir -> recibió el HEAD #%d",head);
				log_info(logger,"aplicar_protocolo_recibir -> recibió un tamaño de -> %d",bufferTam);
				log_info(logger,"aplicar_protocolo_recibir -> comienza a deserealizar");
				pthread_mutex_unlock(&mutexLog);
				switch( head ){
					case APPEARED_POKEMON :{
					//	sem_wait(&semEntrenadores);
						cola_APPEARED_POKEMON* app_poke = malloc(sizeof(cola_APPEARED_POKEMON));
						deserealizar_APPEARED_POKEMON(head, mensaje, bufferTam, app_poke);
						pthread_t hilo_APPEARED;
						pthread_create(&hilo_APPEARED, NULL, (void*) threadAppeared, app_poke);
					break;
					}
					default:
						//log_info(logger, "Instrucción no reconocida");
						printf("Instrucción no reconocida");
					break;
				}
			}
		}
	sleep(configFile->tiempoReconexion);
	fdAppeared=nuevoSocket();
	pthread_mutex_lock(&mutexLogCatedra);
	log_info(loggerCatedra, "Reinicio de proceso de reconexión con broker");
	pthread_mutex_unlock(&mutexLogCatedra);
	}
}

void threadAppeared(cola_APPEARED_POKEMON* app_poke) {
	sendACK(app_poke->id_mensaje);
	pthread_mutex_lock(&mutexLogCatedra);
	log_info(loggerCatedra,"Recibí en la cola APPEARED_POKEMON . POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",app_poke->nombre_pokemon,app_poke->posicion_x,app_poke->posicion_y);
	pthread_mutex_unlock(&mutexLogCatedra);
	if (pokemonNecesario(app_poke->nombre_pokemon) == TRUE) {

		posicionPokemon* newPoke = malloc(sizeof(posicionPokemon));
		newPoke->posiciones = list_create();
		posicion* pos = malloc(sizeof(posicion));
		pos->posicion_x = app_poke->posicion_x;
		pos->posicion_y = app_poke->posicion_y;
		list_add(newPoke->posiciones,pos);
		newPoke->tipoPokemon = malloc(1+string_length(app_poke->nombre_pokemon));
		strcpy(newPoke->tipoPokemon,app_poke->nombre_pokemon);
		pthread_mutex_lock(&mutexBuscados);
		list_add(pokemonesBuscados, newPoke);
		//sem_post(&semEntrenadores);
		sem_post(&semPokemonesBuscados);
		pthread_mutex_unlock(&mutexBuscados);


		//ESTA SECCIÓN DEL APPEARED SE TIENE QUE HACER EN EL PLANIFICADOR
		//SE LE DEBE ASIGNAR EL ENTRENADOR MAS CERCANO QUE ESTÉ EN COLA DE READY AL MOMENTO DE PLANIFICAR
		//LA FUNCION APPEARED SOLO DEBE COLOCAR EL POKEMON EN LA LISTA DE POKEMONES A ATRAPAR
		/*entrenadorPokemon* proximoEntrenadorEnEjecutar = malloc(sizeof(entrenadorPokemon));
		seleccionarEntrenadorMasCercano(app_poke, proximoEntrenadorEnEjecutar);
		char* pos_x = string_itoa(app_poke->posicion_x);
		char* pos_y = string_itoa(app_poke->posicion_y);
		char* proximaAccionEntrenador = malloc(string_length("AtraparPokemon   ") + string_length(app_poke->nombre_pokemon) + string_length(pos_x) + string_length(pos_y) + 1);
		strcpy(proximaAccionEntrenador, "AtraparPokemon ");
		strcat(proximaAccionEntrenador, app_poke->nombre_pokemon);
		strcat(proximaAccionEntrenador, " ");
		strcat(proximaAccionEntrenador, pos_x);
		strcat(proximaAccionEntrenador, " ");
		strcat(proximaAccionEntrenador, pos_y);
		//string_append_with_format(&proximaAccionEntrenador, "AtraparPokemon %s %i %i", app_poke.nombre_pokemon, app_poke.posicion_x, app_poke.posicion_y);
		proximoEntrenadorEnEjecutar->proximaAccion = malloc(strlen(proximaAccionEntrenador) + 1);
		strcpy(proximoEntrenadorEnEjecutar->proximaAccion, proximaAccionEntrenador);
*/
		/*char* proximaAccionEntrenador = string_new();
		string_append_with_format(&proximaAccionEntrenador, "AtraparPokemon %s %i %i", app_poke->nombre_pokemon, app_poke->posicion_x, app_poke->posicion_y);
		proximoEntrenadorEnEjecutar->proximaAccion = proximaAccionEntrenador;*/
		//responder por localized_pokemon

		//printf("Recibí en la cola APPEARED_POKEMON . POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d \n",app_poke.nombre_pokemon,app_poke.posicion_x,app_poke.posicion_y);
		//ejecutar();
		//free(app_poke.nombre_pokemon);

		//ESTOS FREE TAMBIEN IRIAN EN PLANIFICADOR
		/*free(pos_x);
		free(pos_y);
		free(proximaAccionEntrenador);*/
	}
}


void suscripcion_CAUGHT_POKEMON() {
	pthread_mutex_lock(&mutexLog);
	log_info(logger,"Obteniendo los Entrenadores");
	pthread_mutex_unlock(&mutexLog);
	//printf("Obteniendo los entrenadores\n");
	int fdCaught = nuevoSocket();
	//asociarSocket(fdBroker, configFile->puertoBroker);
	//escucharSocket(fdBroker, CONEXIONES_PERMITIDAS);
	pthread_mutex_lock(&mutexLog);
	log_info(logger,"Escuchando conexiones");
	pthread_mutex_unlock(&mutexLog);
	//printf("Escuchando conexiones\n");

	int head , bufferTam;
	while(TRUE) {
		pthread_mutex_lock(&mutexLogCatedra);
		conBroker = conectarCon(fdCaught,configFile->ipBroker,configFile->puertoBroker,loggerCatedra);
		pthread_mutex_unlock(&mutexLogCatedra);
		if(conBroker == TRUE){
			pthread_mutex_lock(&mutexLog);
			conBroker = handshake_cliente(fdCaught, "Team" , "Broker", logger);
			pthread_mutex_unlock(&mutexLog);
		}
		if(conBroker == TRUE){
			suscriptor laSuscripcion;// = malloc(sizeof(suscriptor));
			laSuscripcion.modulo = TEAM; // @suppress("Symbol is not resolved")
			laSuscripcion.token = configFile->token;//config_File->TOKEN;
			laSuscripcion.cola_a_suscribir = list_create();
			list_add(laSuscripcion.cola_a_suscribir, CAUGHT_POKEMON);
			aplicar_protocolo_enviar(fdCaught, SUSCRIPCION, &laSuscripcion);

			list_destroy(laSuscripcion.cola_a_suscribir);
		}
		while (conBroker == TRUE) {

			int recibido = recibirProtocolo(&head,&bufferTam,fdCaught); // recibo head y tamaño de msj
			void * mensaje = malloc(bufferTam);
			if (head < 1 || recibido <= 0){ // DESCONEXIÓN, DEBE RECONECTARSE, SALE DEL WHILE Y SE RECONECTA
				conBroker = FALSE;
			}else{
				recibirMensaje(fdCaught , bufferTam , mensaje ); // recibo msj serializado para el tratamiento deseado
				pthread_mutex_lock(&mutexLog);
				log_info(logger,"aplicar_protocolo_recibir -> recibió el HEAD #%d",head);
				log_info(logger,"aplicar_protocolo_recibir -> recibió un tamaño de -> %d",bufferTam);
				log_info(logger,"aplicar_protocolo_recibir -> comienza a deserealizar");
				pthread_mutex_unlock(&mutexLog);
				switch( head ){

					case CAUGHT_POKEMON :{
						cola_CAUGHT_POKEMON* caug_poke = malloc(sizeof(cola_CAUGHT_POKEMON));
						//responde por caught_pokemon
						deserealizar_CAUGHT_POKEMON(head, mensaje, bufferTam, caug_poke);
						pthread_t hilo_CAUGHT;
						pthread_create(&hilo_CAUGHT, NULL, (void*) threadCaught, caug_poke);
					break;
					}
					default:
						//log_info(logger, "Instrucción no reconocida");
						printf("Instrucción no reconocida");
					break;
				}
			}
		}
	sleep(configFile->tiempoReconexion);
	fdCaught=nuevoSocket();
	pthread_mutex_lock(&mutexLogCatedra);
	log_info(loggerCatedra, "Reinicio de proceso de reconexión con broker");
	pthread_mutex_unlock(&mutexLogCatedra);
	}
}

void threadCaught(cola_CAUGHT_POKEMON* caug_poke) {
	sendACK(caug_poke->id_mensaje);
	entrenadorPokemon* entrenador = verificarMensajeRecibido(caug_poke->id_mensaje);
	if (entrenador != NULL) {
		if (caug_poke->atrapo_pokemon == 0) {
			pthread_mutex_lock(&mxListaCatch);
			pokemonAtrapado(entrenador, caug_poke);
			pthread_mutex_unlock(&mxListaCatch);
		}
	}
	pthread_mutex_lock(&mutexLogCatedra);
	log_info(loggerCatedra,"Recibí en la cola CAUGHT_POKEMON . MENSAJE ID: %d  , ATRAPO: %d",caug_poke->id_mensaje,caug_poke->atrapo_pokemon);
	pthread_mutex_unlock(&mutexLogCatedra);
	//printf("Recibí en la cola CAUGHT_POKEMON . MENSAJE ID: %d  , ATRAPO: %d\n",caug_poke.id_mensaje,caug_poke.atrapo_pokemon);
}

void suscripcion_LOCALIZED_POKEMON() {
	pthread_mutex_lock(&mutexLog);
	log_info(logger,"Obteniendo los Entrenadores");
	pthread_mutex_unlock(&mutexLog);
	//printf("Obteniendo los entrenadores\n");
	int fdLocalized = nuevoSocket();
	//asociarSocket(fdBroker, configFile->puertoBroker);
	//escucharSocket(fdBroker, CONEXIONES_PERMITIDAS);
	pthread_mutex_lock(&mutexLog);
	log_info(logger,"Escuchando conexiones");
	pthread_mutex_unlock(&mutexLog);
	//printf("Escuchando conexiones\n");

	int head , bufferTam;
	while(TRUE) {
		pthread_mutex_lock(&mutexLogCatedra);
		conBroker = conectarCon(fdLocalized,configFile->ipBroker,configFile->puertoBroker,loggerCatedra);
		pthread_mutex_unlock(&mutexLogCatedra);
		if(conBroker == TRUE){
			pthread_mutex_lock(&mutexLog);
			conBroker = handshake_cliente(fdLocalized, "Team" , "Broker", logger);
			pthread_mutex_unlock(&mutexLog);
		}
		if(conBroker == TRUE){
			suscriptor laSuscripcion;// = malloc(sizeof(suscriptor));
			laSuscripcion.modulo = TEAM; // @suppress("Symbol is not resolved")
			laSuscripcion.token = configFile->token;//config_File->TOKEN;
			laSuscripcion.cola_a_suscribir = list_create();
			list_add(laSuscripcion.cola_a_suscribir, LOCALIZED_POKEMON);
			int reg = aplicar_protocolo_enviar(fdLocalized, SUSCRIPCION, &laSuscripcion);

			list_destroy(laSuscripcion.cola_a_suscribir);
			if ((reg != ERROR) && (getEnviado == FALSE)) {
				getPokemon();
				getEnviado = TRUE;
			}
		}
		while (conBroker == TRUE) {

			int recibido = recibirProtocolo(&head,&bufferTam,fdLocalized); // recibo head y tamaño de msj
			void * mensaje = malloc(bufferTam);
			if (head < 1 || recibido <= 0){ // DESCONEXIÓN, DEBE RECONECTARSE, SALE DEL WHILE Y SE RECONECTA
				conBroker = FALSE;
			}else{
				recibirMensaje(fdLocalized , bufferTam , mensaje ); // recibo msj serializado para el tratamiento deseado
				pthread_mutex_lock(&mutexLog);
				log_info(logger,"aplicar_protocolo_recibir -> recibió el HEAD #%d",head);
				log_info(logger,"aplicar_protocolo_recibir -> recibió un tamaño de -> %d",bufferTam);
				log_info(logger,"aplicar_protocolo_recibir -> comienza a deserealizar");
				pthread_mutex_unlock(&mutexLog);
				switch( head ){
					case LOCALIZED_POKEMON :{
						cola_LOCALIZED_POKEMON* loc_poke = malloc(sizeof(cola_LOCALIZED_POKEMON));
						deserealizar_LOCALIZED_POKEMON(head, mensaje, bufferTam, loc_poke);
						pthread_t hilo_LOCALIZED;
						pthread_create(&hilo_LOCALIZED, NULL, (void*) threadLocalized, loc_poke);
					break;
					}
					default:
						//log_info(logger, "Instrucción no reconocida");
						printf("Instrucción no reconocida");
					break;
				}
			}
		}
	sleep(configFile->tiempoReconexion);
	fdLocalized=nuevoSocket();
	pthread_mutex_lock(&mutexLogCatedra);
	log_info(loggerCatedra, "Reinicio de proceso de reconexión con broker");
	pthread_mutex_unlock(&mutexLogCatedra);
	}
}

void threadLocalized(cola_LOCALIZED_POKEMON* loc_poke) {
	sendACK(loc_poke->id_mensaje);
	if (mensajeNoRecibido(loc_poke) == TRUE) {

		for (int i = 0 ; i < list_size(loc_poke->lista_posiciones); i++){
			pthread_mutex_lock(&mutexLogCatedra);
			log_info(loggerCatedra,"Recibí en la cola LOCALIZED_POKEMON . POKEMON: %s  , CANTIDAD: %d , POSICIÓN X: %d , POSICIÓN Y: %d",loc_poke->nombre_pokemon,loc_poke->cantidad,list_get(loc_poke->lista_posiciones,i),list_get(loc_poke->lista_posiciones,i + 1));
			pthread_mutex_unlock(&mutexLogCatedra);
						//printf("Recibí en la cola LOCALIZED_POKEMON . POKEMON: %s  , CANTIDAD: %d , POSICIÓN X: %d , POSICIÓN Y: %d\n",loc_poke.nombre_pokemon,loc_poke.cantidad,list_get(loc_poke.lista_posiciones,i),list_get(loc_poke.lista_posiciones,i + 1));
			i++;
		}
					//desBloquearSemaforoEnt(colaNew,1);
		if (pokemonNecesario(loc_poke->nombre_pokemon) == TRUE) {
			posicionPokemon* newPoke = malloc(sizeof(posicionPokemon));
			newPoke->tipoPokemon = malloc(1+string_length(loc_poke->nombre_pokemon));
			strcpy(newPoke->tipoPokemon,loc_poke->nombre_pokemon);

			for (int i = 0 ; i < list_size(loc_poke->lista_posiciones); i++){

				posicion* pos = list_get(loc_poke->lista_posiciones,i);
				newPoke->posiciones = list_create();
				list_add(newPoke->posiciones,pos);
					//printf("Recibí en la cola LOCALIZED_POKEMON . POKEMON: %s  , CANTIDAD: %d , POSICIÓN X: %d , POSICIÓN Y: %d\n",loc_poke.nombre_pokemon,loc_poke.cantidad,list_get(loc_poke.lista_posiciones,i),list_get(loc_poke.lista_posiciones,i + 1));

			}
			pthread_mutex_lock(&mutexBuscados);
			list_add(pokemonesBuscados, newPoke);
			//sem_post(&semEntrenadores);
			sem_post(&semPokemonesBuscados);
			pthread_mutex_unlock(&mutexBuscados);
		}
	}

}


/*void planificador_Broker() {
	pthread_mutex_lock(&mutexLog);
	log_info(logger,"Obteniendo los Entrenadores");
	pthread_mutex_unlock(&mutexLog);
	//printf("Obteniendo los entrenadores\n");
	fdBroker = nuevoSocket();
	//asociarSocket(fdBroker, configFile->puertoBroker);
	//escucharSocket(fdBroker, CONEXIONES_PERMITIDAS);
	pthread_mutex_lock(&mutexLog);
	log_info(logger,"Escuchando conexiones");
	pthread_mutex_unlock(&mutexLog);
	//printf("Escuchando conexiones\n");

	int head , bufferTam;
	while(TRUE) {
		conBroker = conectarCon(fdBroker,configFile->ipBroker,configFile->puertoBroker,loggerCatedra);

		if(conBroker == TRUE){
			conBroker = handshake_cliente(fdBroker, "Team" , "Broker", logger);
			if (getEnviado == FALSE) {
				getPokemon();
				getEnviado = TRUE;
			}
		}
		if(conBroker == TRUE){
			suscriptor laSuscripcion;// = malloc(sizeof(suscriptor));
			laSuscripcion.modulo = TEAM; // @suppress("Symbol is not resolved")
			laSuscripcion.token = configFile->token;//config_File->TOKEN;
			laSuscripcion.cola_a_suscribir = list_create();
			list_add(laSuscripcion.cola_a_suscribir, APPEARED_POKEMON); // @suppress("Symbol is not resolved")
			list_add(laSuscripcion.cola_a_suscribir, CAUGHT_POKEMON);
			list_add(laSuscripcion.cola_a_suscribir, LOCALIZED_POKEMON);
			aplicar_protocolo_enviar(fdBroker, SUSCRIPCION, &laSuscripcion);

			list_destroy(laSuscripcion.cola_a_suscribir);
		}
		while (conBroker == TRUE) {

			int recibido = recibirProtocolo(&head,&bufferTam,fdBroker); // recibo head y tamaño de msj
			void * mensaje = malloc(bufferTam);
			if (head < 1 || recibido <= 0){ // DESCONEXIÓN, DEBE RECONECTARSE, SALE DEL WHILE Y SE RECONECTA
				conBroker = FALSE;
			}else{
				recibirMensaje(fdBroker , bufferTam , mensaje ); // recibo msj serializado para el tratamiento deseado
				log_info(logger,"aplicar_protocolo_recibir -> recibió el HEAD #%d",head);
				log_info(logger,"aplicar_protocolo_recibir -> recibió un tamaño de -> %d",bufferTam);
				log_info(logger,"aplicar_protocolo_recibir -> comienza a deserealizar");

				switch( head ){
					case APPEARED_POKEMON :{
						sem_wait(&semEntrenadores);
						cola_APPEARED_POKEMON* app_poke = malloc(sizeof(cola_APPEARED_POKEMON));
						deserealizar_APPEARED_POKEMON(head, mensaje, bufferTam, app_poke);
						pthread_t hilo_APPEARED;
						pthread_create(&hilo_APPEARED, NULL, (void*) threadAppeared, app_poke);
					break;
					}
					case CAUGHT_POKEMON :{
						cola_CAUGHT_POKEMON* caug_poke = malloc(sizeof(cola_CAUGHT_POKEMON));
						//responde por caught_pokemon
						deserealizar_CAUGHT_POKEMON(head, mensaje, bufferTam, caug_poke);
						pthread_t hilo_CAUGHT;
						pthread_create(&hilo_CAUGHT, NULL, (void*) threadCaught, caug_poke);
					break;
					}
					case LOCALIZED_POKEMON :{
						cola_LOCALIZED_POKEMON* loc_poke = malloc(sizeof(cola_LOCALIZED_POKEMON));
						deserealizar_LOCALIZED_POKEMON(head, mensaje, bufferTam, loc_poke);
						pthread_t hilo_LOCALIZED;
						pthread_create(&hilo_LOCALIZED, NULL, (void*) threadLocalized, loc_poke);
					break;
					}
					default:
						//log_info(logger, "Instrucción no reconocida");
						printf("Instrucción no reconocida");
					break;
				}
			}
		}
	sleep(configFile->tiempoReconexion);
	fdBroker=nuevoSocket();
	log_info(loggerCatedra, "Reinicio de proceso de reconexión con broker");
	}
}*/

void planificador_GameBoy() {
/*	pthread_mutex_lock(&mutexLog);
	log_info(logger,"Obteniendo los Entrenadores");
	pthread_mutex_unlock(&mutexLog);*/
	//printf("Obteniendo los entrenadores\n");

	fdTeam = nuevoSocket();
	asociarSocket(fdTeam, configFile->puertoTeam);
	escucharSocket(fdTeam, CONEXIONES_PERMITIDAS);

	pthread_mutex_lock(&mutexLog);
	log_info(logger,"Escuchando conexiones");
	pthread_mutex_unlock(&mutexLog);
	//printf("Escuchando conexiones\n");

	while(TRUE) {
		int conexionNueva = 0;
		int comandoNuevo;//= reservarMemoria(INT);
		while(conexionNueva == 0) {
			comandoNuevo = aceptarConexionSocket(fdTeam);
			pthread_mutex_lock(&mutexLog);
			conexionNueva = handshake_servidor(comandoNuevo,"Team", "Broker", logger);
			if(!validar_conexion(conexionNueva, 0,logger) ) {
				cerrarSocket(fdTeam);
			}
			pthread_mutex_unlock(&mutexLog);
			pthread_t hilo_GB;
			pthread_create(&hilo_GB, NULL, (void*)  thread_NewGameboy, comandoNuevo);
		}
	}
}

void thread_NewGameboy(int comandoNuevo){
	int head , bufferTam;
	int recibido = recibirProtocolo(&head,&bufferTam,comandoNuevo); // recibo head y tamaño de msj
		void * mensaje = malloc(bufferTam);
		if (head < 1 || recibido <= 0){ // DESCONEXIÓN
			printf("Error al recibir mensaje.\n");
		//	reconectar();
		}else{
			recibirMensaje(comandoNuevo , bufferTam , mensaje ); // recibo msj serializado para el tratamiento deseado
			pthread_mutex_lock(&mutexLog);
			log_info(logger,"aplicar_protocolo_recibir -> recibió el HEAD #%d",head);
			log_info(logger,"aplicar_protocolo_recibir -> recibió un tamaño de -> %d",bufferTam);
			log_info(logger,"aplicar_protocolo_recibir -> comienza a deserealizar");
			pthread_mutex_unlock(&mutexLog);
		/*printf("aplicar_protocolo_recibir -> recibió el HEAD #%d\n",head);
		printf("aplicar_protocolo_recibir -> recibió un tamaño de -> %d\n",bufferTam);
		printf("aplicar_protocolo_recibir -> comienza a deserealizar\n");*/
		switch( head ){
		case APPEARED_POKEMON :{
			//sem_wait(&semEntrenadores);
			cola_APPEARED_POKEMON* app_poke = malloc(sizeof(cola_APPEARED_POKEMON));
			deserealizar_APPEARED_POKEMON(head, mensaje, bufferTam, app_poke);
			pthread_mutex_lock(&mutexLogCatedra);
			log_info(loggerCatedra,"Recibí en la cola APPEARED_POKEMON . POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",app_poke->nombre_pokemon,app_poke->posicion_x,app_poke->posicion_y);
			pthread_mutex_unlock(&mutexLogCatedra);
			if (pokemonNecesario(app_poke->nombre_pokemon) == TRUE) {

				posicionPokemon* newPoke = malloc(sizeof(posicionPokemon));
				newPoke->posiciones = list_create();
				posicion* pos = malloc(sizeof(posicion));
				pos->posicion_x = app_poke->posicion_x;
				pos->posicion_y = app_poke->posicion_y;
				list_add(newPoke->posiciones,pos);
				newPoke->tipoPokemon = malloc(1+string_length(app_poke->nombre_pokemon));
				strcpy(newPoke->tipoPokemon,app_poke->nombre_pokemon);
				pthread_mutex_lock(&mutexBuscados);
				list_add(pokemonesBuscados, newPoke);
				//sem_post(&semEntrenadores);
				sem_post(&semPokemonesBuscados);
				pthread_mutex_unlock(&mutexBuscados);

			/*	entrenadorPokemon* proximoEntrenadorEnEjecutar = malloc(sizeof(entrenadorPokemon));
				seleccionarEntrenadorMasCercano(&app_poke, proximoEntrenadorEnEjecutar);
				char* pos_x = string_itoa(app_poke.posicion_x);
				char* pos_y = string_itoa(app_poke.posicion_y);
				char* proximaAccionEntrenador = malloc(string_length("AtraparPokemon   ") + string_length(app_poke.nombre_pokemon) + string_length(pos_x) + string_length(pos_y) + 1);
				strcpy(proximaAccionEntrenador, "AtraparPokemon ");
				strcat(proximaAccionEntrenador, app_poke.nombre_pokemon);
				strcat(proximaAccionEntrenador, " ");
				strcat(proximaAccionEntrenador, pos_x);
				strcat(proximaAccionEntrenador, " ");
				strcat(proximaAccionEntrenador, pos_y);
				//string_append_with_format(&proximaAccionEntrenador, "AtraparPokemon %s %i %i", app_poke.nombre_pokemon, app_poke.posicion_x, app_poke.posicion_y);
				proximoEntrenadorEnEjecutar->proximaAccion = malloc(strlen(proximaAccionEntrenador) + 1);
				strcpy(proximoEntrenadorEnEjecutar->proximaAccion, proximaAccionEntrenador);
				pthread_mutex_lock(&mutexColaReady);
				for(int posicionEnReady = 0; posicionEnReady < list_size(colaReady); posicionEnReady++){
					entrenadorPokemon* entrenador = list_get(colaReady, posicionEnReady);
					if (entrenador->idEntrenador == proximoEntrenadorEnEjecutar->idEntrenador) {
						list_remove(colaReady, posicionEnReady);
						break;
					}
				}
				list_add_in_index(colaReady, 0, proximoEntrenadorEnEjecutar);
				pthread_mutex_unlock(&mutexColaReady);
				pthread_mutex_lock(&mutexLogCatedra);
				log_info(loggerCatedra, "Entrenador %i a cola de Ready por ser el más cercano", proximoEntrenadorEnEjecutar->idEntrenador);
				pthread_mutex_unlock(&mutexLogCatedra);
				//responder por localized_pokemon
				//printf("Recibí en la cola APPEARED_POKEMON . POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d \n",app_poke.nombre_pokemon,app_poke.posicion_x,app_poke.posicion_y);
				//free(app_poke.nombre_pokemon);
				free(pos_x);
				free(pos_y);
				free(proximaAccionEntrenador);*/
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
			pthread_mutex_lock(&mutexLogCatedra);
			log_info(loggerCatedra,"Recibí en la cola CAUGHT_POKEMON . MENSAJE ID: %d  , ATRAPO: %d",caug_poke.id_mensaje,caug_poke.atrapo_pokemon);
			pthread_mutex_unlock(&mutexLogCatedra);
			//printf("Recibí en la cola CAUGHT_POKEMON . MENSAJE ID: %d  , ATRAPO: %d\n",caug_poke.id_mensaje,caug_poke.atrapo_pokemon);
		break;
		}
		case LOCALIZED_POKEMON :{
			cola_LOCALIZED_POKEMON* loc_poke = malloc(sizeof(cola_LOCALIZED_POKEMON)) ;
			deserealizar_LOCALIZED_POKEMON(head, mensaje, bufferTam, loc_poke);
			for (int i = 0 ; i < list_size(loc_poke->lista_posiciones); i++){
				pthread_mutex_lock(&mutexLogCatedra);
				log_info(loggerCatedra,"Recibí en la cola LOCALIZED_POKEMON . POKEMON: %s  , CANTIDAD: %d , POSICIÓN X: %d , POSICIÓN Y: %d",loc_poke->nombre_pokemon,loc_poke->cantidad,list_get(loc_poke->lista_posiciones,i),list_get(loc_poke->lista_posiciones,i + 1));
				pthread_mutex_unlock(&mutexLogCatedra);
				//printf("Recibí en la cola LOCALIZED_POKEMON . POKEMON: %s  , CANTIDAD: %d , POSICIÓN X: %d , POSICIÓN Y: %d\n",loc_poke.nombre_pokemon,loc_poke.cantidad,list_get(loc_poke.lista_posiciones,i),list_get(loc_poke.lista_posiciones,i + 1));

			}
			//desBloquearSemaforoEnt(colaNew,1);

			if (pokemonNecesario(loc_poke->nombre_pokemon) == TRUE) {
				posicionPokemon* newPoke = malloc(sizeof(posicionPokemon));
				newPoke->tipoPokemon = malloc(1+string_length(loc_poke->nombre_pokemon));
				strcpy(newPoke->tipoPokemon,loc_poke->nombre_pokemon);

				for (int i = 0 ; i < list_size(loc_poke->lista_posiciones); i++){

					posicion* pos = list_get(loc_poke->lista_posiciones,i);
					newPoke->posiciones = list_create();
					list_add(newPoke->posiciones,pos);
						//printf("Recibí en la cola LOCALIZED_POKEMON . POKEMON: %s  , CANTIDAD: %d , POSICIÓN X: %d , POSICIÓN Y: %d\n",loc_poke.nombre_pokemon,loc_poke.cantidad,list_get(loc_poke.lista_posiciones,i),list_get(loc_poke.lista_posiciones,i + 1));

				}
				pthread_mutex_lock(&mutexBuscados);
				list_add(pokemonesBuscados, newPoke);
				//sem_post(&semEntrenadores);
				sem_post(&semPokemonesBuscados);
				pthread_mutex_unlock(&mutexBuscados);

				}


		break;
		}
		default:
			//log_info(logger, "Instrucción no reconocida");
			printf("Instrucción no reconocida");
		break;
		}
		//ejecutar();
	}
	free(mensaje);

	//pthread_mutex_lock(&mxHilos);
	pthread_mutex_lock(&mutexLog);
	log_info(logger, "Fin de hilo GameBoy");
	pthread_mutex_unlock(&mutexLog);
	//pthread_cancel( pthread_self() );
	pthread_detach( pthread_self() );
}



/*void planificador_BACKUP() {
	pthread_mutex_lock(&mutexLog);
	log_info(logger,"Obteniendo los Entrenadores");
	pthread_mutex_unlock(&mutexLog);
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
		printf("aplicar_protocolo_recibir -> recibió el HEAD #%d\n",head);
		printf("aplicar_protocolo_recibir -> recibió un tamaño de -> %d\n",bufferTam);
		printf("aplicar_protocolo_recibir -> comienza a deserealizar\n");*/
/*
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
	}
	}
}

 *  */

void thread_Entrenador(entrenadorPokemon * entrenador) {
	while(TRUE){ //VALIDAR SI VA CON UN WHILE TRUE
	pthread_mutex_lock(&entrenador->semaforMutex);
	char* arrayAccion = string_new();
	int llegoADestino = 0;
	char* accion = malloc(1 + string_length(entrenador->proximaAccion));
	strcpy(accion,entrenador->proximaAccion);
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
			llegoADestino = moverEntrenador(entrenador, posicionXInt, posicionYInt );
			if (llegoADestino == 1) {
				catchPokemon(entrenador, pokemonAAtrapar, posicionXInt, posicionYInt);
				/*for (int i=0; i < list_size(colaBlocked);i++) {
					entrenadorPokemon* entrenador = list_get(colaBlocked, i);
					printf("Entrenador %i en lista blocked\n", entrenador->idEntrenador);
				}*/
			} else {
				pthread_mutex_lock(&mutexLogCatedra);
				log_info(loggerCatedra, "Se movió al entrenador id %i a la cola de Ready para continuar su ejecución proximamente", entrenador->idEntrenador);
				pthread_mutex_unlock(&mutexLogCatedra);
				pthread_mutex_lock(&mutexColaReady);
				list_add(colaReady, entrenador);
				pthread_mutex_unlock(&mutexColaReady);
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
			moverEntrenador(entrenador, posicionXEntrenador2Int, posicionYEntrenador2Int);
			realizarIntercambio(entrenador, entrenador2, atrapadoInnecesarioEntrenador1, atrapadoInnecesarioEntrenador2);
			entrenador->proximaAccion = "";
			entrenador2->proximaAccion = "";
			verificarDeadlock(entrenador);
			verificarDeadlock(entrenador2);
			//verificarIntercambios();
		}
	} else {
			/*pthread_mutex_lock(&mutexColaReady);
			list_add(colaReady, entrenador);
			pthread_mutex_unlock(&mutexColaReady);*/
		}

	pthread_mutex_lock(&mxEjecutando);
	}
}

/*void reconectar(){
		pthread_mutex_lock(&h_reconectar);
		while (conBroker != 1) {
			sleep(configFile->tiempoReconexion);
			log_info(loggerCatedra, "Inicio de proceso de reconexión con broker");
			conBroker = conectarCon(fdBroker,configFile->ipBroker,configFile->puertoBroker,loggerCatedra);
		}
		handshake_cliente(fdBroker,"Team","Broker",logger);
		pthread_mutex_unlock(&h_reconectar);
	}
*/
