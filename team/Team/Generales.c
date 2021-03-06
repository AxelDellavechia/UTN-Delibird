#include "Generales.h"



	void destroySubItem(void* a){
			free(a);

 	}

		 	void destroyItem(void* a){
		 			entrenadorPokemon* elem = (entrenadorPokemon*) a;

		 			if(string_length(elem->proximaAccion) > 0){free(elem->proximaAccion);}
		 	/*		for(int i = 0;i<list_size(elem->pokemonesAtrapados);i++){
		 							char* obj = list_get(elem->pokemonesAtrapados,i);
		 							free(obj);
		 			}
		 		    list_destroy(elem->pokemonesAtrapados);

		 			for(int i = 0;i<list_size(elem->pokemonesObjetivo);i++){
		 							char* obj = list_get(elem->pokemonesObjetivo,i);
		 							free(obj);
		 			}
		 		    list_destroy(elem->pokemonesObjetivo);*/
		 			free (elem);
		 //			list_clean_and_destroy_elements(elem->pokemonesAtrapados, destroySubItem);
		// 			list_clean_and_destroy_elements(elem->pokemonesObjetivo, destroySubItem);
		 	}

		 	void destroyItemExit(void* a){
		 			entrenadorPokemon* elem = (entrenadorPokemon*) a;

		 			free(elem->proximaAccion);

		 		    int tamLista = list_size(elem->pokemonesAtrapados);

		 		    for(int i = 0; i < tamLista;i++){
		 		    	char * poke = list_get(elem->pokemonesAtrapados,0);
		 		    	list_remove(elem->pokemonesAtrapados,0);
		 		   	//	free(poke);
		 		   	}
		 		    list_destroy(elem->pokemonesAtrapados);

		 		    tamLista = list_size(elem->pokemonesObjetivo);

		 			for(int i = 0;i< tamLista;i++){
		 				//char* obj = list_get(elem->pokemonesObjetivo,0);
		 				list_remove(elem->pokemonesObjetivo,0);
		 				//free(obj);
		 			}
		 		    list_destroy(elem->pokemonesObjetivo);

		 			//free(elem);

		 	}

		 	void destroyItemCatch(void* a){
		 		cola_CATCH_POKEMON* elem = (cola_CATCH_POKEMON*) a;
		 			if(string_length(elem->nombre_pokemon) > 0){free(elem->nombre_pokemon);}

		 	}


		 	void destroyItemPosBus(void* a){
		 		posicionPokemon* elem = (posicionPokemon*) a;
		 			if(string_length(elem->tipoPokemon) > 0){free(elem->tipoPokemon);}

		 	}

void destroyListaChar(void* lista) {
	int tamLista = list_size(lista);
		for(int i = 0; i < tamLista;i++){
			char * poke = list_get(lista,0);
			list_remove(lista,0);
			free(poke);
			}
		list_destroy(lista);
}


void iniciar_logCatedra(){
	char ** aux = string_split(configFile->logFile,"/") ;
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
		}
		free(aux[i]);
		i++;
	}
	free(aux[i]);
	free(aux);
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
	configFile = malloc(1 + sizeof(archivoConfigFile));

	config = config_create(ruta);
	configFile->tiempoReconexion = config_get_int_value(config, "TIEMPO_RECONEXION");
	configFile->retardoCicloCPU = config_get_int_value(config, "RETARDO_CICLO_CPU");
	configFile->algoritmoPlanificacion = strdup(config_get_string_value(config, "ALGORITMO_PLANIFICACION"));
	configFile->quantum = config_get_int_value(config, "QUANTUM");
	configFile->ipBroker = strdup(config_get_string_value(config, "IP_BROKER"));
	configFile->estimacionInicial = config_get_int_value(config, "ESTIMACION_INICIAL");
	configFile->puertoBroker = config_get_int_value(config, "PUERTO_BROKER");
	configFile->puertoTeam = config_get_int_value(config, "PUERTO_TEAM");
	configFile->logFile = strdup(config_get_string_value(config, "LOG_FILE"));
	configFile->alpha = config_get_double_value(config, "ALPHA");
	char** posicionEntrenadoresAux = config_get_array_value(config, "POSICIONES_ENTRENADORES");
	char** pokemonEntrenadoresAux = config_get_array_value(config, "POKEMON_ENTRENADORES");
	char** objetivosEntrenadoresAux = config_get_array_value(config, "OBJETIVOS_ENTRENADORES");
	configFile->posicionEntrenadores = list_create();
	configFile->pokemonEntrenadores = list_create();
	configFile->objetivosEntrenadores = list_create();
	while(posicionEntrenadoresAux[posicionLista] != NULL) {
		//char * objeto = posicionEntrenadoresAux[posicionLista] ;
		list_add(configFile->posicionEntrenadores, string_duplicate(posicionEntrenadoresAux[posicionLista]));
		//free(objeto);
		posicionLista++;
	}
	//free(posicionEntrenadoresAux);
	posicionLista = 0;
	while(pokemonEntrenadoresAux[posicionLista] != NULL) {
		//char * objeto = pokemonEntrenadoresAux[posicionLista] ;
		list_add(configFile->pokemonEntrenadores, string_duplicate(pokemonEntrenadoresAux[posicionLista]));
		//free(objeto);
		posicionLista++;
	}
	//free(pokemonEntrenadoresAux);
	posicionLista = 0;
	while(objetivosEntrenadoresAux[posicionLista] != NULL) {
		//char * objeto = objetivosEntrenadoresAux[posicionLista] ;
		list_add(configFile->objetivosEntrenadores, string_duplicate(objetivosEntrenadoresAux[posicionLista]));
		//free(objeto);
		posicionLista++;
	}



	//free(objetivosEntrenadoresAux);
	if (config_get_int_value(config,"TOKEN") != 0 ){
		configFile->token = config_get_int_value(config,"TOKEN");
	}else
	{
		configFile->token = token();
		grabarToken(configFile->token);
	}


	//si entra a romper, sacar estos free------

/*	for(int a = 0;a < list_size(configFile->posicionEntrenadores);a++){
		free(posicionEntrenadoresAux[a]);
	}
	free(posicionEntrenadoresAux);

	for(int a = 0;a < list_size(configFile->pokemonEntrenadores);a++){
		free(pokemonEntrenadoresAux[a]);
	}
	free(pokemonEntrenadoresAux);

	for(int a = 0;a < list_size(configFile->objetivosEntrenadores);a++){
		free(objetivosEntrenadoresAux[a]);
	}
	free(objetivosEntrenadoresAux);*/

	//------------------


	 // Libero la estructura archivoConfig
	/*for(int i = 0;i<sizeof(objetivosEntrenadoresAux);i++){
		char* obj = objetivosEntrenadoresAux[i];
		free(obj);
	}
	for(int i = 0;i<sizeof(posicionEntrenadoresAux);i++){
		char* obj = posicionEntrenadoresAux[i];
		free(obj);
	}
	for(int i = 0;i<sizeof(pokemonEntrenadoresAux);i++){
			char* obj = pokemonEntrenadoresAux[i];
			free(obj);
		}
	free(posicionEntrenadoresAux);
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
	listaCatchPokemon = list_create();
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
	idEntrenadorAnterior = -1;
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
		entrenadorPokemon* entrenador = malloc(sizeof(entrenadorPokemon));
		entrenador->pokemonesAtrapados = list_create();
		entrenador->pokemonesObjetivo = list_create();
		entrenador->idEntrenador = listaEntrenadores;
		entrenador->posicion_x = posicionX;
		entrenador->posicion_y = posicionY;
		pthread_mutex_init(&entrenador->semaforMutex, NULL);
		entrenador->ciclosEnCPU = 0;
		entrenador->proximaAccion = strdup("");
		entrenador->estimacionUltimaRafaga = 0;
		//pthread_mutex_lock(&entrenador->semaforMutex);
		char* pokemonAtrapado = strtok(pokemonesEntrenador, "|");
		while (pokemonAtrapado != NULL) {
			list_add(entrenador->pokemonesAtrapados, pokemonAtrapado);
			pokemonAtrapado = strtok(NULL, "|");
		}
		free(pokemonAtrapado);
 		char* pokemonObjetivo = strtok(objetivosEntrenador, "|");
		while (pokemonObjetivo != NULL) {
			list_add(entrenador->pokemonesObjetivo, string_duplicate(pokemonObjetivo));
			pokemonObjetivo = strtok(NULL, "|");
		}

		pthread_mutex_lock(&mutexColaNew);
		list_add(colaNew, entrenador);
		pthread_mutex_unlock(&mutexColaNew);
		pthread_mutex_lock(&mutexLogCatedra);
		log_info(loggerCatedra, "Entrenador %i creado y en cola de New", listaEntrenadores);
		pthread_mutex_unlock(&mutexLogCatedra);
		list_add_all(objetivoTeam, entrenador->pokemonesObjetivo);
		if (list_size(entrenador->pokemonesAtrapados) > 0) {
			for (int posicionObjetivo = 0; posicionObjetivo < list_size(entrenador->pokemonesAtrapados); posicionObjetivo++) {
				char* objetivo = list_get(entrenador->pokemonesAtrapados, posicionObjetivo);
				_Bool mismoObjetivo(char * lista){return string_equals_ignore_case(lista, objetivo);}
				list_remove_by_condition(objetivoTeam, (void*)mismoObjetivo);
			}
		}
	}

	pthread_mutex_lock(&mutexColaNew);
	cantEntrenadores = list_size(colaNew);
	pthread_mutex_unlock(&mutexColaNew);

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
	pthread_mutex_lock(&mutexColaNew);
	pthread_mutex_lock(&mutexColaBlocked);
	int movimientoDeProximoEntrenadorAReady = 0;
	int posicionProximoEntrenador = 0;
	int colaEntrenador = 0;
	posicion* posPoke = list_get(pokemonAparecido->posiciones,0);
	int posicionXPokemon = pos->posicion_x;
	int posicionYPokemon = pos->posicion_y;
	if(list_size(colaNew) > 0) {
		for(int posicionEntrenador = 0; posicionEntrenador < list_size(colaNew); posicionEntrenador++) {
			entrenadorPokemon* entrenador = list_get(colaNew, posicionEntrenador);

		char* proximaAccionEntrenador = malloc(strlen(entrenador->proximaAccion) + 1);
		strcpy(proximaAccionEntrenador, entrenador->proximaAccion);
		if (string_equals_ignore_case(proximaAccionEntrenador, "")) {
			int posicionXEntrenador = entrenador->posicion_x;
			int posicionYEntrenador = entrenador->posicion_y;
			int movimientoEnX = fabs(posicionXEntrenador - posicionXPokemon);
			int movimientoEnY = fabs(posicionYEntrenador - posicionYPokemon);
			int movimientoEntrenador = movimientoEnX + movimientoEnY;
			if ((movimientoEntrenador < movimientoDeProximoEntrenadorAReady) || (movimientoDeProximoEntrenadorAReady == 0)){
				movimientoDeProximoEntrenadorAReady = movimientoEntrenador;
				colaEntrenador = 0;
				posicionProximoEntrenador = posicionEntrenador;
			}
		}
		free(proximaAccionEntrenador);
	}
	}
	if(list_size(colaBlocked) > 0) {
		for(int posicionEntrenador = 0; posicionEntrenador < list_size(colaBlocked); posicionEntrenador++) {
			entrenadorPokemon* entrenador = list_get(colaBlocked, posicionEntrenador);

			char* proximaAccionEntrenador = malloc(strlen(entrenador->proximaAccion) + 1);
			strcpy(proximaAccionEntrenador, entrenador->proximaAccion);
			if (string_equals_ignore_case(proximaAccionEntrenador, "")) {
				int posicionXEntrenador = entrenador->posicion_x;
				int posicionYEntrenador = entrenador->posicion_y;
				int movimientoEnX = fabs(posicionXEntrenador - posicionXPokemon);
				int movimientoEnY = fabs(posicionYEntrenador - posicionYPokemon);
				int movimientoEntrenador = movimientoEnX + movimientoEnY;
				if ((movimientoEntrenador < movimientoDeProximoEntrenadorAReady) || (movimientoDeProximoEntrenadorAReady == 0)){
					movimientoDeProximoEntrenadorAReady = movimientoEntrenador;
					colaEntrenador = 1;
					posicionProximoEntrenador = posicionEntrenador;
				}
			}
			free(proximaAccionEntrenador);
	//printf("Entrenador %i es el más cercano\n", proximoEntrenadorEnEjecutar->idEntrenador);
		}
	}
	entrenadorPokemon* seleccionado;
	if(colaEntrenador == 0){
		seleccionado = list_get(colaNew,posicionProximoEntrenador);

		//quitarDeColaNew(seleccionado);
	}else if(colaEntrenador == 1){
		seleccionado = list_get(colaBlocked,posicionProximoEntrenador);

		//quitarDeColaBlocked(seleccionado);
	}

	proximoEntrenadorEnEjecutar->ciclosEnCPU = seleccionado->ciclosEnCPU;
	proximoEntrenadorEnEjecutar->estimacionUltimaRafaga = seleccionado->estimacionUltimaRafaga;
	proximoEntrenadorEnEjecutar->idEntrenador = seleccionado->idEntrenador;
	proximoEntrenadorEnEjecutar->idMsjEsperado = seleccionado->idMsjEsperado;
	proximoEntrenadorEnEjecutar->pokemonesAtrapados = list_create();
	proximoEntrenadorEnEjecutar->pokemonesObjetivo = list_create();
	//list_add_all(proximoEntrenadorEnEjecutar->pokemonesAtrapados, seleccionado->pokemonesAtrapados);
	proximoEntrenadorEnEjecutar->pokemonesAtrapados=list_duplicate(seleccionado->pokemonesAtrapados);
	//list_add_all(proximoEntrenadorEnEjecutar->pokemonesObjetivo, seleccionado->pokemonesObjetivo);
	proximoEntrenadorEnEjecutar->pokemonesObjetivo=list_duplicate(seleccionado->pokemonesObjetivo);

//	list_add_all(proximoEntrenadorEnEjecutar->pokemonesAtrapados, seleccionado->pokemonesAtrapados);
//	list_add_all(proximoEntrenadorEnEjecutar->pokemonesObjetivo, seleccionado->pokemonesObjetivo);
	proximoEntrenadorEnEjecutar->posicion_x = seleccionado->posicion_x;
	proximoEntrenadorEnEjecutar->posicion_y = seleccionado->posicion_y;
	proximoEntrenadorEnEjecutar->semaforMutex = seleccionado->semaforMutex;

	if(colaEntrenador == 0){

		list_remove_and_destroy_element(colaNew, posicionProximoEntrenador, (void*) destroyItem);
		//quitarDeColaNew(seleccionado);
	}else if(colaEntrenador == 1){

		list_remove_and_destroy_element(colaBlocked, posicionProximoEntrenador, (void*) destroyItem);
		//quitarDeColaBlocked(seleccionado);
	}



	pthread_mutex_unlock(&mutexColaNew);
	pthread_mutex_unlock(&mutexColaBlocked);
	pthread_mutex_lock(&mutexLogCatedra);
	log_info(loggerCatedra, "Entrenador %i a cola de Ready por ser el más cercano", proximoEntrenadorEnEjecutar->idEntrenador);
	pthread_mutex_unlock(&mutexLogCatedra);
}

int moverEntrenador(entrenadorPokemon* entrenador, int posicionXDestino, int posicionYDestino) {

	int tiempo = 0;
	if (string_equals_ignore_case("RR", configFile->algoritmoPlanificacion)){
		tiempo = configFile->quantum;
	}else if (string_equals_ignore_case("SJF-CD", configFile->algoritmoPlanificacion)){
		tiempo = 1;
	}

	int ciclos = 0;
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
				ciclos++;
				log_info(logger, "El entrenador %i se movió a la posición %i %i", entrenador->idEntrenador, posicionXEntrenador, entrenador->posicion_y);
			} else if (posicionXEntrenador > posicionXDestino) {
				posicionXEntrenador--;
				ciclosEnCPUEntrenador++;
				ciclos++;
				log_info(logger, "El entrenador %i se movió a la posición %i %i", entrenador->idEntrenador, posicionXEntrenador, entrenador->posicion_y);
			} else {
				moverEntrenadorEnX = FALSE;
			}
		} else {
			moverEntrenadorEnX = FALSE;
		}
		usleep((configFile->retardoCicloCPU)*1000000);
	}
	while (moverEntrenadorEnY == TRUE) {
		if(ciclosEnCPUEntrenador != ciclosEnCPUEntrenadorMasTiempo) {
			if (posicionYEntrenador < posicionYDestino) {
				posicionYEntrenador++;
				ciclosEnCPUEntrenador++;
				ciclos++;
				log_info(logger, "El entrenador %i se movió a la posición %i %i", entrenador->idEntrenador, entrenador->posicion_x, posicionYEntrenador);
			} else if (posicionYEntrenador > posicionYDestino) {
				posicionYEntrenador--;
				ciclosEnCPUEntrenador++;
				ciclos++;
				log_info(logger, "El entrenador %i se movió a la posición %i %i", entrenador->idEntrenador, entrenador->posicion_x, posicionYEntrenador);
			} else {
				moverEntrenadorEnY = FALSE;
			}
		} else {
			moverEntrenadorEnY = FALSE;
		}
		usleep((configFile->retardoCicloCPU)*1000000);
	}
	entrenador->ciclosEnCPU = ciclosEnCPUEntrenador;
	entrenador->posicion_x = posicionXEntrenador;
	entrenador->posicion_y = posicionYEntrenador;
	pthread_mutex_lock(&mxCiclosCPU);
	ciclosEnCPU = ciclosEnCPU + ciclos;
	pthread_mutex_unlock(&mxCiclosCPU);
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
	/*for(int i = 0; i<  list_size(entrenador1->pokemonesAtrapados);i++){
			char* p1 = list_get(entrenador1->pokemonesAtrapados,i);
			printf("%s\n", p1);
	}
	for(int i = 0; i<  list_size(entrenador2->pokemonesAtrapados);i++){
			char* p2 = list_get(entrenador2->pokemonesAtrapados,i);
			printf("%s\n", p2);
	}*/

	quitarPokemonDeAtrapados(entrenador1, pokemonEntrenador1);
	quitarPokemonDeAtrapados(entrenador2, pokemonEntrenador2);
	list_add(entrenador1->pokemonesAtrapados, pokemonEntrenador2);
	list_add(entrenador2->pokemonesAtrapados, pokemonEntrenador1);
	pthread_mutex_lock(&mutexLogCatedra);
	log_info(loggerCatedra, "Se realizó el intercambio del pokemon %s del entrenador %i por el pokemon %s del entrenador %i", pokemonEntrenador1, entrenador1->idEntrenador, pokemonEntrenador2, entrenador2->idEntrenador);
	pthread_mutex_unlock(&mutexLogCatedra);
	pthread_mutex_lock(&mxCiclosCPU);
	ciclosEnCPU = ciclosEnCPU + 5;
	pthread_mutex_unlock(&mxCiclosCPU);
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

void verificarEstado(entrenadorPokemon* entrenador) {
	int pokemonEncontrado;
	int entrenadorEnDeadlock;
	t_list* listaObjetivosAuxiliar = list_duplicate(entrenador->pokemonesObjetivo);
	int cantidadPokemonesAtrapados = list_size(entrenador->pokemonesAtrapados);
	int cantidadPokemonesObjetivo = list_size(entrenador->pokemonesObjetivo);
	int cantidadObjetivosAuxiliar = cantidadPokemonesObjetivo;
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
				log_info(loggerCatedra,"Se pasa entrenador %d a estado Deadlock", entrenador->idEntrenador);
				list_add(entrenadoresEnDeadlock, entrenador);
				//printf("Entrenador %i en deadlock\n", entrenador->idEntrenador);
			}

			//verificarIntercambios();
			break;
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
		pthread_mutex_lock(&mutexColaExit);
		list_add(colaExit, entrenador);
		pthread_mutex_unlock(&mutexColaExit);
		pthread_mutex_lock(&mutexColaBlocked);
		quitarDeColaBlocked(entrenador);
		pthread_mutex_unlock(&mutexColaBlocked);
		pthread_mutex_lock(&mutexLogCatedra);
		log_info(loggerCatedra, "Se movió al entrenador de id %i a Exit ya que atrapó a todos los Pokemon que tenía como objetivo", entrenador->idEntrenador);
		pthread_mutex_unlock(&mutexLogCatedra);
	}
	//destroyListaChar(listaObjetivosAuxiliar);
}

//Verifica si el entrenador esta en deadlock. Si no lo esta, lo quita de blocked y lo pasa a ready seteandole "" como proxima accion
void verificarDeadlock(entrenadorPokemon* entrenador) {
	int cantidadPokemonesAtrapados = list_size(entrenador->pokemonesAtrapados);
	int cantidadPokemonesObjetivo = list_size(entrenador->pokemonesObjetivo);
	if (cantidadPokemonesAtrapados == cantidadPokemonesObjetivo) {
		pthread_mutex_lock(&mxEntrenadoresDeadLock);
		verificarEstado(entrenador);
		if (cantEntrenadores == list_size(colaExit)) {
			pthread_mutex_lock(&mutexLogCatedra);
			pthread_mutex_lock(&mxCiclosCPU);
			printf("Cantidad de ciclos de CPU totales: %i\n", ciclosEnCPU);
			log_info(loggerCatedra, "Cantidad de ciclos de CPU totales: %i", ciclosEnCPU);
			pthread_mutex_unlock(&mxCiclosCPU);
			printf("Cantidad de cambios de contexto realizados: %i\n", cantCambiosContexto);
			log_info(loggerCatedra, "Cantidad de cambios de contexto realizados: %i", cantCambiosContexto);
			printf("Cantidad de ciclos de CPU realizados por entrenador:\n");
			log_info(loggerCatedra, "Cantidad de ciclos de CPU realizados por entrenador:");
			pthread_mutex_unlock(&mutexLogCatedra);
			for (int posicionEntrenador = 0; posicionEntrenador < list_size(colaExit); posicionEntrenador++) {
				entrenadorPokemon* entrenadorEnExit = list_get(colaExit, posicionEntrenador);
				pthread_mutex_lock(&mutexLogCatedra);
				printf("Entrenador %i - Ciclos de CPU: %i\n", entrenadorEnExit->idEntrenador, entrenadorEnExit->ciclosEnCPU);
				log_info(loggerCatedra, "Entrenador %i - Ciclos de CPU: %i", entrenadorEnExit->idEntrenador, entrenadorEnExit->ciclosEnCPU);
				pthread_mutex_unlock(&mutexLogCatedra);
			}
			pthread_mutex_lock(&mutexLogCatedra);
			printf("Deadlocks producidos y resueltos: %i\n", cantDeadlocks);
			log_info(loggerCatedra, "Deadlocks producidos y resueltos: %i", cantDeadlocks);
			pthread_mutex_unlock(&mutexLogCatedra);
		}
		pthread_mutex_unlock(&mxEntrenadoresDeadLock);
	} else {
		entrenador->proximaAccion = realloc(entrenador->proximaAccion,string_length("") + 1);
		strcpy(entrenador->proximaAccion,"");
		sem_post(&entrenadoresLibres);
	}
	if (list_size(objetivoTeam) == 0){
		verificarIntercambios();
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

void quitarDeColaNew(entrenadorPokemon* entrenador) {
	for(int posicionEnListaNew = 0; posicionEnListaNew < list_size(colaNew); posicionEnListaNew++) {
		entrenadorPokemon* entrenadorNew = list_get(colaNew, posicionEnListaNew);
		if (entrenadorNew->idEntrenador == entrenador->idEntrenador) {
			list_remove(colaNew, posicionEnListaNew);
			//list_remove_and_destroy_element(colaNew, posicionEnListaNew, (void*) destroyItem);
			break;
		}
	}
}

void quitarDeColaReady(entrenadorPokemon* entrenador) {
	for(int posicionEnListaReady = 0; posicionEnListaReady < list_size(colaReady); posicionEnListaReady++) {
		entrenadorPokemon* entrenadorEnReady = list_get(colaReady, posicionEnListaReady);
		if (entrenadorEnReady->idEntrenador == entrenador->idEntrenador) {
			list_remove(colaReady, posicionEnListaReady);
			//list_remove_and_destroy_element(colaReady, posicionEnListaReady, (void*) destroyItem);
			break;
		}
	}
}

/*void verificarIntercambios() {
	int hayIntercambios = FALSE;
	pthread_mutex_lock(&mxEntrenadoresDeadLock);
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
					entrenador1->proximaAccion = malloc(string_length("") +1);
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
					quitarDeColaBlocked(entrenador1);
					pthread_mutex_lock(&mutexColaReady);
					list_add(colaReady, entrenador1);
					pthread_mutex_unlock(&mutexColaReady);
					free(id2);
					free(pos_x2);
					free(pos_y2);
					free(proximaAccionEntrenador);
					hayIntercambios = TRUE;
					break;
				}
			}
			break;
		}
	}
	pthread_mutex_unlock(&mxEntrenadoresDeadLock);
	if (hayIntercambios == TRUE) {
		sem_post(&elementosEnReady);
	}
}*/

char* analizarIntercambios(t_list* primerLista, t_list* segundaLista) {
	_Bool pokeSeleccionado = true;
	for (int posicionPrimerLista = 0; posicionPrimerLista < list_size(primerLista); posicionPrimerLista++) {
		char* pokemonPrimerLista = list_get(primerLista, posicionPrimerLista);
		_Bool buscar_poke(char* lista){return string_equals_ignore_case(lista, pokemonPrimerLista);}
		pokeSeleccionado = list_any_satisfy(segundaLista, (void*)buscar_poke);
		if (pokeSeleccionado) {
			return pokemonPrimerLista;
		}
	}
	return "";
}

void establecerIntercambio(entrenadorPokemon* entrenador1, entrenadorPokemon* entrenador2, char* atrapadoInnecesarioEntrenador1, char* atrapadoInnecesarioEntrenador2) {
	int idEntrenador2 = entrenador2->idEntrenador;
	int posicionXEntrenador2 = entrenador2->posicion_x;
	int posicionYEntrenador2 = entrenador2->posicion_y;
	entrenador1->proximaAccion = strdup("");
	/*entrenador1->proximaAccion = malloc(string_length("") +1);
	strcpy(entrenador1->proximaAccion,"");*/
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
	/*entrenador1->proximaAccion = malloc(strlen(proximaAccionEntrenador) + 1);
	strcpy(entrenador1->proximaAccion, proximaAccionEntrenador);*/
	entrenador1->proximaAccion = strdup(proximaAccionEntrenador);

	pthread_mutex_lock(&mutexColaReady);
	list_add(colaReady, entrenador1);
	pthread_mutex_unlock(&mutexColaReady);
	quitarDeColaBlocked(entrenador1);
	free(id2);
	free(pos_x2);
	free(pos_y2);
	free(proximaAccionEntrenador);
}

void establecerIntercambioTriple(entrenadorPokemon* entrenador1, entrenadorPokemon* entrenador2, entrenadorPokemon* entrenador3, char* atrapadoInnecesarioEntrenador1, char* atrapadoInnecesarioEntrenador2) {
	int idEntrenador2 = entrenador2->idEntrenador;
	int posicionXEntrenador2 = entrenador2->posicion_x;
	int posicionYEntrenador2 = entrenador2->posicion_y;
	entrenador1->proximaAccion = malloc(string_length("") +1);
	strcpy(entrenador1->proximaAccion,"");
	pthread_mutex_lock(&mutexLogCatedra);
	log_info(loggerCatedra, "Se detectó deadlock entre los entrenadores %i, %i y %i", entrenador1->idEntrenador, idEntrenador2, entrenador3->idEntrenador);
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

	pthread_mutex_lock(&mutexColaReady);
	list_add(colaReady, entrenador1);
	pthread_mutex_unlock(&mutexColaReady);
	quitarDeColaBlocked(entrenador1);
	free(id2);
	free(pos_x2);
	free(pos_y2);
	free(proximaAccionEntrenador);
}

void verificarIntercambios() {

	int tamLista = 0;
	int hayIntercambios = FALSE;
	t_list* atrapadosInnecesarios1 = list_create();
	t_list* atrapadosInnecesarios2 = list_create();
	t_list* atrapadosInnecesarios3 = list_create();
	t_list* objetivosFaltantes1 = list_create();
	t_list* objetivosFaltantes2 = list_create();
	t_list* objetivosFaltantes3 = list_create();
	pthread_mutex_lock(&mxEntrenadoresDeadLock);
	if (list_size(entrenadoresEnDeadlock) > 1) {
		pthread_mutex_lock(&mutexLogCatedra);
		log_info(loggerCatedra, "Inicia algoritmo de detección de deadlock");
		pthread_mutex_unlock(&mutexLogCatedra);
		for (int posicionEntrenador1 = 0; posicionEntrenador1 < list_size(entrenadoresEnDeadlock); posicionEntrenador1++) {
			entrenadorPokemon* entrenador1 = list_get(entrenadoresEnDeadlock, posicionEntrenador1);
		/*	for (int i = 0; i < list_size(entrenador1->pokemonesAtrapados); i++) {
				printf("Pokemon atrapado %s\n", list_get(entrenador1->pokemonesAtrapados, i));
			}
			for (int i = 0; i < list_size(entrenador1->pokemonesObjetivo); i++) {
				printf("Pokemon objetivo %s\n", list_get(entrenador1->pokemonesObjetivo, i));
			}*/
			tamLista = list_size(atrapadosInnecesarios1);
			for(int i = 0; i < tamLista;i++){
				char * poke = list_get(atrapadosInnecesarios1,0);
				list_remove(atrapadosInnecesarios1,0);
				//free(poke);
			}
			tamLista = list_size(objetivosFaltantes1);
			for(int i = 0; i < tamLista;i++){
				char * poke = list_get(objetivosFaltantes1,0);
				list_remove(objetivosFaltantes1,0);
			//	free(poke);
			}
			obtenerObjetivosFaltantes(entrenador1, objetivosFaltantes1);
			obtenerAtrapadosInnecesarios(entrenador1, atrapadosInnecesarios1);
			for (int posicionEntrenador2 = posicionEntrenador1 + 1; posicionEntrenador2 < list_size(entrenadoresEnDeadlock); posicionEntrenador2++) {
				char* pokemonFaltanteEntrenador1 = "";
				char* pokemonFaltanteEntrenador2 = "";
				tamLista = list_size(atrapadosInnecesarios2);
				for(int i = 0; i < tamLista;i++){
					char * poke = list_get(atrapadosInnecesarios2,0);
					list_remove(atrapadosInnecesarios2,0);
				//	free(poke);
				}
				tamLista = list_size(objetivosFaltantes2);
				for(int i = 0; i < tamLista;i++){
					char * poke = list_get(objetivosFaltantes2,0);
					list_remove(objetivosFaltantes2,0);
					//free(poke);
				}
				entrenadorPokemon* entrenador2 = list_get(entrenadoresEnDeadlock, posicionEntrenador2);
				/*for (int i = 0; i < list_size(entrenador2->pokemonesAtrapados); i++) {
					printf("Pokemon atrapado %s\n", list_get(entrenador2->pokemonesAtrapados, i));
				}
				for (int i = 0; i < list_size(entrenador2->pokemonesObjetivo); i++) {
					printf("Pokemon objetivo %s\n", list_get(entrenador2->pokemonesObjetivo, i));
				}*/
				obtenerObjetivosFaltantes(entrenador2, objetivosFaltantes2);
				obtenerAtrapadosInnecesarios(entrenador2, atrapadosInnecesarios2);

				pokemonFaltanteEntrenador1 = analizarIntercambios(objetivosFaltantes1, atrapadosInnecesarios2);
				pokemonFaltanteEntrenador2 = analizarIntercambios(objetivosFaltantes2, atrapadosInnecesarios1);
				if (!string_equals_ignore_case(pokemonFaltanteEntrenador1, "") && (!string_equals_ignore_case(pokemonFaltanteEntrenador2, ""))) {
					establecerIntercambio(entrenador1, entrenador2, pokemonFaltanteEntrenador2, pokemonFaltanteEntrenador1);
					hayIntercambios = TRUE;
					break;
				}
			}
			if (hayIntercambios == TRUE) {
				break;
			}
		}

		if (hayIntercambios == FALSE) {
			for (int posicionEntrenador1 = 0; posicionEntrenador1 < list_size(entrenadoresEnDeadlock); posicionEntrenador1++) {
				entrenadorPokemon* entrenador1 = list_get(entrenadoresEnDeadlock, posicionEntrenador1);
				tamLista = list_size(atrapadosInnecesarios1);
				for(int i = 0; i < tamLista;i++){
					char * poke = list_get(atrapadosInnecesarios1,0);
					list_remove(atrapadosInnecesarios1,0);
					//free(poke);
				}
				tamLista = list_size(objetivosFaltantes1);
				for(int i = 0; i < tamLista;i++){
					char * poke = list_get(objetivosFaltantes1,0);
					list_remove(objetivosFaltantes1,0);
					//free(poke);
				}
				obtenerObjetivosFaltantes(entrenador1, objetivosFaltantes1);
				obtenerAtrapadosInnecesarios(entrenador1, atrapadosInnecesarios1);
				for (int posicionEntrenador2 = posicionEntrenador1 + 1; posicionEntrenador2 < list_size(entrenadoresEnDeadlock); posicionEntrenador2++) {
					char* pokemonFaltanteEntrenador2 = "";
					tamLista = list_size(atrapadosInnecesarios2);
					for(int i = 0; i < tamLista;i++){
						char * poke = list_get(atrapadosInnecesarios2,0);
						list_remove(atrapadosInnecesarios2,0);
					//	free(poke);
					}
					tamLista = list_size(objetivosFaltantes2);
					for(int i = 0; i < tamLista;i++){
						char * poke = list_get(objetivosFaltantes2,0);
						list_remove(objetivosFaltantes2,0);
					//	free(poke);
					}
					entrenadorPokemon* entrenador2 = list_get(entrenadoresEnDeadlock, posicionEntrenador2);
					obtenerObjetivosFaltantes(entrenador2, objetivosFaltantes2);
					obtenerAtrapadosInnecesarios(entrenador2, atrapadosInnecesarios2);
					pokemonFaltanteEntrenador2 = analizarIntercambios(atrapadosInnecesarios1, objetivosFaltantes2);
					if (!string_equals_ignore_case(pokemonFaltanteEntrenador2, "")) {
						for (int posicionEntrenador3 = posicionEntrenador2 + 1; posicionEntrenador3 < list_size(entrenadoresEnDeadlock); posicionEntrenador3++) {
							char* pokemonFaltanteEntrenador1 = "";
							char* pokemonFaltanteEntrenador3 = "";
							tamLista = list_size(atrapadosInnecesarios3);
							for(int i = 0; i < tamLista;i++){
								char * poke = list_get(atrapadosInnecesarios3,0);
								list_remove(atrapadosInnecesarios3,0);
							//	free(poke);
							}
							tamLista = list_size(objetivosFaltantes3);
							for(int i = 0; i < tamLista;i++){
								char * poke = list_get(objetivosFaltantes3,0);
								list_remove(objetivosFaltantes3,0);
							//	free(poke);
							}
							entrenadorPokemon* entrenador3 = list_get(entrenadoresEnDeadlock, posicionEntrenador3);
							obtenerObjetivosFaltantes(entrenador3, objetivosFaltantes3);
							obtenerAtrapadosInnecesarios(entrenador3, atrapadosInnecesarios3);
							pokemonFaltanteEntrenador3 = analizarIntercambios(atrapadosInnecesarios2, objetivosFaltantes3);
							pokemonFaltanteEntrenador1 = analizarIntercambios(atrapadosInnecesarios3, objetivosFaltantes1);
							if (!string_equals_ignore_case(pokemonFaltanteEntrenador1, "") && (!string_equals_ignore_case(pokemonFaltanteEntrenador3, ""))) {
								establecerIntercambioTriple(entrenador1, entrenador2, entrenador3, pokemonFaltanteEntrenador2, pokemonFaltanteEntrenador3);
								hayIntercambios = TRUE;
								break;
							}
						}
					}
					if (hayIntercambios == TRUE) {
						break;
					}
				}
				if (hayIntercambios == TRUE) {
					break;
				}
			}
		}

		if (hayIntercambios == FALSE) {
			for (int posicionEnListaDeadlock = 0; posicionEnListaDeadlock < list_size(entrenadoresEnDeadlock); posicionEnListaDeadlock++) {
				entrenadorPokemon* entrenador1 = list_get(entrenadoresEnDeadlock, posicionEnListaDeadlock);
				char* objetivoFaltanteEntrenador1 = obtenerPokemonObjetivoFaltante(entrenador1);
				char* atrapadoInnecesarioEntrenador1 = obtenerPokemonAtrapadoInnecesario(entrenador1);
				for (int posicionEntrenadorParaComparar = posicionEnListaDeadlock + 1; posicionEntrenadorParaComparar < list_size(entrenadoresEnDeadlock); posicionEntrenadorParaComparar++) {
					entrenadorPokemon* entrenador2 = list_get(entrenadoresEnDeadlock, posicionEntrenadorParaComparar);
					char* objetivoFaltanteEntrenador2 = obtenerPokemonObjetivoFaltante(entrenador2);
					char* atrapadoInnecesarioEntrenador2 = obtenerPokemonAtrapadoInnecesario(entrenador2);
					if ((string_equals_ignore_case(objetivoFaltanteEntrenador1, atrapadoInnecesarioEntrenador2)) || (string_equals_ignore_case(atrapadoInnecesarioEntrenador1, objetivoFaltanteEntrenador2))) {
						establecerIntercambio(entrenador1, entrenador2, atrapadoInnecesarioEntrenador1, atrapadoInnecesarioEntrenador2);
						hayIntercambios = TRUE;
						break;
					}
				}
				break;
			}
		}
	}
	/*destroyListaChar(atrapadosInnecesarios1);
	destroyListaChar(atrapadosInnecesarios2);
	destroyListaChar(atrapadosInnecesarios3);
	destroyListaChar(objetivosFaltantes1);
	destroyListaChar(objetivosFaltantes2);
	destroyListaChar(objetivosFaltantes3);*/
	pthread_mutex_unlock(&mxEntrenadoresDeadLock);
	if (hayIntercambios == TRUE) {
		sem_post(&elementosEnReady);
	}
}

char* obtenerPokemonObjetivoFaltante(entrenadorPokemon* entrenador) {
	int pokemonObjetivoFaltanteEncontrado;
	char* pokemonObjetivo;
	t_list* listaAtrapadosAuxiliar = list_create();
	int cantidadPokemonesAtrapados = list_size(entrenador->pokemonesAtrapados);
	int cantidadPokemonesObjetivo = list_size(entrenador->pokemonesObjetivo);
	listaAtrapadosAuxiliar = list_duplicate(entrenador->pokemonesAtrapados);
	int cantidadAtrapadosAuxiliar = cantidadPokemonesAtrapados;
	for (int posicionObjetivo = 0; posicionObjetivo < cantidadPokemonesObjetivo; posicionObjetivo++) {
		pokemonObjetivoFaltanteEncontrado = FALSE;
		char* pokemonObjetivo = list_get(entrenador->pokemonesObjetivo, posicionObjetivo);
		//pokemonObjetivo = list_get(entrenador->pokemonesObjetivo, posicionObjetivo);
		for (int posicionAtrapado = 0; posicionAtrapado < cantidadAtrapadosAuxiliar; posicionAtrapado++) {
			char* pokemonAtrapado = list_get(listaAtrapadosAuxiliar, posicionAtrapado);
			//pokemonAtrapado = list_get(listaAtrapadosAuxiliar, posicionAtrapado);
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
//	destroyListaChar(listaAtrapadosAuxiliar);
	return pokemonObjetivo;
}

void obtenerObjetivosFaltantes(entrenadorPokemon* entrenador, t_list* objetivosFaltantes) {
	int pokemonObjetivoFaltanteEncontrado;
	t_list* listaAtrapadosAuxiliar = list_create();
	listaAtrapadosAuxiliar = list_duplicate(entrenador->pokemonesAtrapados);
	int cantidadPokemonesAtrapados = list_size(entrenador->pokemonesAtrapados);
	int cantidadPokemonesObjetivo = list_size(entrenador->pokemonesObjetivo);
	int cantidadAtrapadosAuxiliar = cantidadPokemonesAtrapados;
	for (int posicionObjetivo = 0; posicionObjetivo < cantidadPokemonesObjetivo; posicionObjetivo++) {
		pokemonObjetivoFaltanteEncontrado = FALSE;
		char* pokemonObjetivo = list_get(entrenador->pokemonesObjetivo, posicionObjetivo);
		//strcpy(pokemonObjetivo,	(list_get(entrenador->pokemonesObjetivo, posicionObjetivo)));
		for (int posicionAtrapado = 0; posicionAtrapado < cantidadAtrapadosAuxiliar; posicionAtrapado++) {
			char* pokemonAtrapado = list_get(listaAtrapadosAuxiliar, posicionAtrapado);
			if (string_equals_ignore_case(pokemonAtrapado, pokemonObjetivo)) {
				list_remove(listaAtrapadosAuxiliar, posicionAtrapado);
				cantidadAtrapadosAuxiliar = list_size(listaAtrapadosAuxiliar);
				pokemonObjetivoFaltanteEncontrado = TRUE;
				break;
			}
		}
		if (pokemonObjetivoFaltanteEncontrado == FALSE) {
			list_add(objetivosFaltantes, pokemonObjetivo);
		}
	}
//	destroyListaChar(listaAtrapadosAuxiliar);
}

char* obtenerPokemonAtrapadoInnecesario(entrenadorPokemon* entrenador) {
	int pokemonAtrapadoInnecesarioEncontrado;
	char* pokemonAtrapado;
	t_list* listaObjetivosAuxiliar = list_create();
	listaObjetivosAuxiliar = list_duplicate(entrenador->pokemonesObjetivo);
	int cantidadPokemonesAtrapados = list_size(entrenador->pokemonesAtrapados);
	int cantidadPokemonesObjetivo = list_size(entrenador->pokemonesObjetivo);
	int cantidadObjetivosAuxiliar = cantidadPokemonesObjetivo;
	for (int posicionAtrapado = 0; posicionAtrapado < cantidadPokemonesAtrapados; posicionAtrapado++) {
		pokemonAtrapadoInnecesarioEncontrado = FALSE;
		char* pokemonAtrapado = list_get(entrenador->pokemonesAtrapados, posicionAtrapado);
		//pokemonAtrapado = list_get(entrenador->pokemonesAtrapados, posicionAtrapados);
		for (int posicionObjetivo = 0; posicionObjetivo < cantidadObjetivosAuxiliar; posicionObjetivo++) {
			char* pokemonObjetivo = list_get(entrenador->pokemonesObjetivo, posicionObjetivo);
			//pokemonObjetivo = list_get(listaObjetivosAuxiliar, posicionObjetivos);
			if (string_equals_ignore_case(pokemonAtrapado, pokemonObjetivo)) {
				list_remove(listaObjetivosAuxiliar, posicionObjetivo);
				cantidadObjetivosAuxiliar = list_size(listaObjetivosAuxiliar);
				pokemonAtrapadoInnecesarioEncontrado = TRUE;
				break;
			}
		}
		if (pokemonAtrapadoInnecesarioEncontrado == FALSE) {
			return pokemonAtrapado;
		}
	}
	//destroyListaChar(listaObjetivosAuxiliar);
	return pokemonAtrapado;
}

void obtenerAtrapadosInnecesarios(entrenadorPokemon* entrenador, t_list* atrapadosInnecesarios) {
	int pokemonAtrapadoInnecesarioEncontrado;
	t_list* listaObjetivosAuxiliar = list_create();
	listaObjetivosAuxiliar = list_duplicate( entrenador->pokemonesObjetivo);
	int cantidadPokemonesAtrapados = list_size(entrenador->pokemonesAtrapados);
	int cantidadPokemonesObjetivo = list_size(entrenador->pokemonesObjetivo);
	int cantidadObjetivosAuxiliar = cantidadPokemonesObjetivo;
	for (int posicionAtrapado = 0; posicionAtrapado < cantidadPokemonesAtrapados; posicionAtrapado++) {
		pokemonAtrapadoInnecesarioEncontrado = FALSE;
		char* pokemonAtrapado = list_get(entrenador->pokemonesAtrapados, posicionAtrapado);
		//pokemonAtrapado = list_get(entrenador->pokemonesAtrapados, posicionAtrapados);
		for (int posicionObjetivo = 0; posicionObjetivo < cantidadObjetivosAuxiliar; posicionObjetivo++) {
			//char* pokemonObjetivo = list_get(entrenador->pokemonesObjetivo, posicionObjetivo);
			char* pokemonObjetivo = list_get(listaObjetivosAuxiliar, posicionObjetivo);

			//pokemonObjetivo = list_get(listaObjetivosAuxiliar, posicionObjetivos);
			if (string_equals_ignore_case(pokemonAtrapado, pokemonObjetivo)) {
				list_remove(listaObjetivosAuxiliar, posicionObjetivo);
				cantidadObjetivosAuxiliar = list_size(listaObjetivosAuxiliar);
				pokemonAtrapadoInnecesarioEncontrado = TRUE;
				break;
			}
		}
		if (pokemonAtrapadoInnecesarioEncontrado == FALSE) {
			list_add(atrapadosInnecesarios, pokemonAtrapado);
		}
	}
//	destroyListaChar(listaObjetivosAuxiliar);
}

int verificarMensajeRecibido (int idMensajeRecibido, entrenadorPokemon* entrenador) {
	pthread_mutex_lock(&mutexColaBlocked);
	for (int posicionEntrenador = 0; posicionEntrenador < list_size(colaBlocked); posicionEntrenador++) {
		entrenador = list_get(colaBlocked, posicionEntrenador);
		if (idMensajeRecibido == entrenador->idMsjEsperado) {
			pthread_mutex_unlock(&mutexColaBlocked);
			return TRUE;
		}
	}
	pthread_mutex_unlock(&mutexColaBlocked);
	return FALSE;
}

//Agrega el pokemon atrapado a la lista de pokemones atrapados del entrenador, quita al pokemon de la lista CatchPokemon y verifica si el entrenador esta en Deadlock
void pokemonAtrapado(entrenadorPokemon* entrenador, cola_CAUGHT_POKEMON* pokemonRecibido) {
	pthread_mutex_lock(&mutexObjetivoTeam);
	pthread_mutex_lock(&mxListaCatch);
	for(int posicionPokemon = 0; posicionPokemon < list_size(listaCatchPokemon); posicionPokemon++) {
		cola_CATCH_POKEMON* pokemonCatch = list_get(listaCatchPokemon, posicionPokemon);
		if(pokemonCatch->id_mensaje == pokemonRecibido->id_tracking) {
			//entrenador->proximaAccion = realloc(entrenador->proximaAccion,string_length("") + 1);
			//strcpy(entrenador->proximaAccion,"");
			list_add(entrenador->pokemonesAtrapados, string_duplicate(pokemonCatch->nombre_pokemon));
			for (int posicionObjetivo = 0; posicionObjetivo < list_size(objetivoTeam); posicionObjetivo++) {
				char* pokemonObjetivo = list_get(objetivoTeam, posicionObjetivo);
				if (string_equals_ignore_case(pokemonObjetivo, pokemonCatch->nombre_pokemon)){
					list_remove(objetivoTeam, posicionObjetivo);
					break;
				}
			}
			pthread_mutex_lock(&mutexLogCatedra);
			log_info(loggerCatedra, "Pokemon atrapado: %s", pokemonCatch->nombre_pokemon);
			pthread_mutex_unlock(&mutexLogCatedra);
		//	list_destroy_and_destroy_elements(listaCatchPokemon, destroyItemCatch);
			list_remove_and_destroy_element(listaCatchPokemon, posicionPokemon,destroyItemCatch);
			break;
		}
	}


	verificarDeadlock(entrenador);
	pthread_mutex_unlock(&mxListaCatch);
	pthread_mutex_unlock(&mutexObjetivoTeam);
}

void pokemonAtrapadoSinConexion(entrenadorPokemon* entrenador, cola_CATCH_POKEMON* pokemonRecibido) {
	list_add(entrenador->pokemonesAtrapados, string_duplicate(pokemonRecibido->nombre_pokemon));
	pthread_mutex_lock(&mutexObjetivoTeam);
	for (int posicionObjetivo = 0; posicionObjetivo < list_size(objetivoTeam); posicionObjetivo++) {
		char* pokemonObjetivo = list_get(objetivoTeam, posicionObjetivo);
			if (string_equals_ignore_case(pokemonObjetivo, pokemonRecibido->nombre_pokemon)){
				list_remove(objetivoTeam, posicionObjetivo);
				break;
			}
	}

	verificarDeadlock(entrenador);
	pthread_mutex_unlock(&mutexObjetivoTeam);
}

int pokemonNecesario(char* pokemonAparecido) {
	pthread_mutex_lock(&mutexObjetivoTeam);
	for (int posicionObjetivo = 0; posicionObjetivo < list_size(objetivoTeam); posicionObjetivo++) {
		char* pokemonObjetivo = list_get(objetivoTeam, posicionObjetivo);
		if (string_equals_ignore_case(pokemonObjetivo, pokemonAparecido)){
			pthread_mutex_unlock(&mutexObjetivoTeam);
			return TRUE;
		}
	}
	pthread_mutex_unlock(&mutexObjetivoTeam);
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
	/*if (crearLista == TRUE) {
		listaCatchPokemon = list_create();
		crearLista = FALSE;
	}*/
	int head , bufferTam;
	//pokemon* pokemonEnLista;
	pthread_mutex_lock(&mxListaEjecutando);
	_Bool buscar_poke(pokemon * lista){return lista->posicion_x == posicionX && lista->posicion_y == posicionY && string_equals_ignore_case(lista->nombre_pokemon, nombrePokemon);}
    list_remove_by_condition(pokemonesEjecutando, (void*)buscar_poke);
    pthread_mutex_unlock(&mxListaEjecutando);
	cola_CATCH_POKEMON* pokemonEnCatch = malloc(sizeof(cola_CATCH_POKEMON));
	pokemonEnCatch->id_mensaje = 0;
	pokemonEnCatch->nombre_pokemon = malloc(string_length(nombrePokemon) + 1);
	pokemonEnCatch->nombre_pokemon = string_duplicate(nombrePokemon);
	//pokemonEnCatch->nombre_pokemon = strdup(nombrePokemon);
	pokemonEnCatch->tamanio_nombre = string_length(nombrePokemon);
	pokemonEnCatch->posicion_x = posicionX;
	pokemonEnCatch->posicion_y = posicionY;
	//pthread_mutex_lock(&entrenador->semaforMutex);

	exec = NULL;

	pthread_mutex_lock(&mutexLogCatedra);
	log_info(loggerCatedra, "Se movió al entrenador id %i a la cola de Blocked a la espera del resultado del catch", entrenador->idEntrenador);
	pthread_mutex_unlock(&mutexLogCatedra);

	int socket = nuevoSocket();
	pthread_mutex_lock(&mutexLog);
	int result = conectarCon( socket ,configFile->ipBroker,configFile->puertoBroker,logger);
	//int result = FALSE;
	pthread_mutex_unlock(&mutexLog);
	if (result != FALSE  ){
		pthread_mutex_lock(&mutexLog);
		handshake_cliente(socket,"Team","Broker",logger);
		pthread_mutex_unlock(&mutexLog);
			result = aplicar_protocolo_enviar(socket,CATCH_POKEMON,pokemonEnCatch);
			if(result != ERROR){

				int recibido = recibirProtocolo(&head,&bufferTam,socket);
				void * mensaje = malloc(bufferTam);
				recibirMensaje(socket, bufferTam, mensaje);
				if(head == ACK){
					respuesta_ACK * ack = malloc(sizeof(respuesta_ACK));
					deserealizar_ACK( head, mensaje, bufferTam, ack);
					pokemonEnCatch->id_mensaje = ack->id_msj;
					pthread_mutex_lock(&mutexColaBlocked);
					entrenador->idMsjEsperado = ack->id_msj;
					entrenador->proximaAccion= malloc(1 + string_length("Aguardando Caught"));
					strcpy(entrenador->proximaAccion,"Aguardando Caught");
					list_add(colaBlocked, entrenador);
					pthread_mutex_unlock(&mutexColaBlocked);
					pthread_mutex_lock(&mxListaCatch);
					list_add(listaCatchPokemon, pokemonEnCatch);
					pthread_mutex_unlock(&mxListaCatch);
				}else{
					result=ERROR;
				}
						//pokemonEnCatch->id_mensaje = idRecibido;
			}			//entrenador->idMsjEsperado = idRecibido;
	}
	if (result == ERROR || result == FALSE){
		pthread_mutex_lock(&mutexLogCatedra);
		log_info(loggerCatedra, "No se pudo conectar por el Broker, se considera al Pokemon %s atrapado por el entrenador id %i",pokemonEnCatch->nombre_pokemon, entrenador->idEntrenador);
		pthread_mutex_unlock(&mutexLogCatedra);
		pthread_mutex_lock(&mutexColaBlocked);
		list_add(colaBlocked, entrenador);
		pthread_mutex_unlock(&mutexColaBlocked);
		pokemonAtrapadoSinConexion(entrenador, pokemonEnCatch);
		free(pokemonEnCatch);
	}
	pthread_mutex_lock(&mxCiclosCPU);
	ciclosEnCPU++;
	pthread_mutex_unlock(&mxCiclosCPU);
}

void getPokemon() {
	int getYaRealizado;
	pthread_mutex_lock(&mxInitAppeared);
	char* primerObjetivo = list_get(objetivoTeam, 0);

	cola_GET_POKEMON* pokemonAGet = malloc(sizeof(cola_GET_POKEMON));
	pokemonAGet->id_mensaje = 0;
	pokemonAGet->id_tracking = 0;
	pokemonAGet->nombre_pokemon = strdup(primerObjetivo);
	pokemonAGet->tamanio_nombre = strlen(pokemonAGet->nombre_pokemon);

	pthread_mutex_lock(&mutexLogCatedra);
	pthread_mutex_lock(&mutexLog);
	int envio = conectar_y_enviar("BROKER" , configFile->ipBroker,configFile->puertoBroker, "Team" , "Broker",GET_POKEMON, pokemonAGet,logger ,loggerCatedra );
	if(envio > 0){
		initAppeared++;
	}
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
			pokemonAGet1->id_tracking = 0;
			pokemonAGet1->nombre_pokemon = malloc(string_length(pokemonObjetivo1)+1);
			strcpy(pokemonAGet1->nombre_pokemon, pokemonObjetivo1);
			pokemonAGet1->tamanio_nombre = strlen(pokemonObjetivo1);
			pthread_mutex_lock(&mutexLogCatedra);
			pthread_mutex_lock(&mutexLog);
			int res = conectar_y_enviar("BROKER" , configFile->ipBroker,configFile->puertoBroker, "Team" , "Broker",GET_POKEMON, pokemonAGet1 ,logger ,loggerCatedra );
			if(res > 0){
				initAppeared++;
			}
			pthread_mutex_unlock(&mutexLogCatedra);
			pthread_mutex_unlock(&mutexLog);
			free(pokemonAGet1->nombre_pokemon);
			free(pokemonAGet1);
			//aplicar_protocolo_enviar(fdBroker, GET_POKEMON, pokemonObjetivo1);
		}
	}
	free(pokemonAGet->nombre_pokemon );
	free(pokemonAGet);
	pthread_mutex_unlock(&mxInitAppeared);
}

void planificador() {

	entrenadorPokemon* proximoEntrenadorEnEjecutar;

	while(EXIT_TEAM) {
		sem_wait(&elementosEnReady);
		if(EXIT_TEAM == TRUE){
		//if(hayIntercambios == FALSE) {
/*		} else {
			proximoEntrenadorEnEjecutar = malloc(sizeof(entrenadorPokemon));
			buscarEntrenadorParaIntercambio(proximoEntrenadorEnEjecutar);
		}*/
		if (string_equals_ignore_case("FIFO", configFile->algoritmoPlanificacion)) {

			cantCambiosContexto++;
			pthread_mutex_lock(&mutexColaReady);
			proximoEntrenadorEnEjecutar = list_get(colaReady, 0);
			pthread_mutex_unlock(&mutexColaReady);

			pthread_mutex_lock(&mxEntrenadoresList);
			pthread_mutex_unlock(mxEntrenadores + proximoEntrenadorEnEjecutar->idEntrenador);

		} else if (string_equals_ignore_case("RR", configFile->algoritmoPlanificacion)) {

			cantCambiosContexto++;
			pthread_mutex_lock(&mutexColaReady);
			proximoEntrenadorEnEjecutar = list_get(colaReady, 0);
			pthread_mutex_unlock(&mutexColaReady);

			pthread_mutex_lock(&mxEntrenadoresList);
			pthread_mutex_unlock(mxEntrenadores + proximoEntrenadorEnEjecutar->idEntrenador);
		} else if (string_equals_ignore_case("SJF-SD", configFile->algoritmoPlanificacion)) {
			cantCambiosContexto++;
			pthread_mutex_lock(&mutexColaReady);
			proximoEntrenadorEnEjecutar = list_get(colaReady, 0);
			pthread_mutex_unlock(&mutexColaReady);

			pthread_mutex_lock(&mxEntrenadoresList);
			pthread_mutex_unlock(mxEntrenadores + proximoEntrenadorEnEjecutar->idEntrenador);
		} else if (string_equals_ignore_case("SJF-CD", configFile->algoritmoPlanificacion)){
			for(int i= 0; i<list_size(colaReady);i++){
					entrenadorPokemon* ent = list_get(colaReady,i);
					ent->estimacionUltimaRafaga = calcularRafagaCPU(ent);
				}
			_Bool ordenarSJF(entrenadorPokemon* a, entrenadorPokemon* b){return a->estimacionUltimaRafaga < b->estimacionUltimaRafaga;}
			list_sort(colaReady, (void*)ordenarSJF);

			pthread_mutex_lock(&mutexColaReady);
			proximoEntrenadorEnEjecutar = list_get(colaReady, 0);
			pthread_mutex_unlock(&mutexColaReady);

			pthread_mutex_lock(&mxEntrenadoresList);
			if(proximoEntrenadorEnEjecutar->idEntrenador != idEntrenadorAnterior) {
				cantCambiosContexto++;
				idEntrenadorAnterior = proximoEntrenadorEnEjecutar->idEntrenador;
			}
			pthread_mutex_unlock(mxEntrenadores + proximoEntrenadorEnEjecutar->idEntrenador);
			} else {
				log_info(loggerCatedra, "El algoritmo %s es incorrecto", configFile->algoritmoPlanificacion);
			}
		pthread_mutex_lock(&mxEjecutando);
		//free(proximoEntrenadorEnEjecutar);
		/*pthread_mutex_lock(&mutexObjetivoTeam);
		if (list_size(objetivoTeam) == 0) {
			pthread_mutex_unlock(&mutexObjetivoTeam);
			verificarIntercambios();
		}else{
		pthread_mutex_unlock(&mutexObjetivoTeam);
		}*/
	}
	}
	free(proximoEntrenadorEnEjecutar);
}

double calcularRafagaCPU(entrenadorPokemon* entrenador) {
	double rafagaActual = entrenador->estimacionUltimaRafaga;
	int tiempoCPUEntrenador = entrenador->ciclosEnCPU;
	double alpha = configFile->alpha;
	return (alpha * tiempoCPUEntrenador) + ((1 - alpha) * rafagaActual);
}

void inicializar_semaforos(){
	//inicializo semaforos de nodos
	pthread_mutex_init(&mxSocketsFD, NULL);
	pthread_mutex_init(&mxHilos, NULL);
	pthread_mutex_init(&mutex_idMensaje, NULL);
	pthread_mutex_init(&mutexColaReady, NULL);
	pthread_mutex_init(&mutexColaBlocked, NULL);
	pthread_mutex_init(&mutexColaExit, NULL);
	pthread_mutex_init(&mutexBuscados, NULL);
	pthread_mutex_init(&mxEjecutando, NULL);
	pthread_mutex_lock(&mxEjecutando);
	pthread_mutex_init(&mxListaEjecutando, NULL);
	pthread_mutex_init(&mxListaCatch, NULL);
	pthread_mutex_init(&mutexColaNew, NULL);
	pthread_mutex_init(&mutexObjetivoTeam, NULL);
	pthread_mutex_init(&mxEntrenadoresList, NULL);
	pthread_mutex_init(&mxEntrenadoresDeadLock, NULL);
	pthread_mutex_init(&mxCiclosCPU, NULL);
	pthread_mutex_init(&mxInitAppeared, NULL);

	//semEntrenadores = malloc(sizeof(sem_t));
	sem_init(&semEntrenadores, 0, cantEntrenadores);
	sem_init(&semPokemonesBuscados, 0, 0);
	sem_init(&elementosEnReady, 0, 0);
	sem_init(&entrenadoresLibres, 0, 0);
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

	pthread_create(&hilo_APPEARED, NULL, (void*) suscripcion_APPEARED_POKEMON, NULL);
	pthread_create(&hilo_LOCALIZED, NULL, (void*) suscripcion_LOCALIZED_POKEMON, NULL);
	pthread_create(&hilo_CAUGHT, NULL, (void*) suscripcion_CAUGHT_POKEMON, NULL);
	pthread_create(&hilo_gameboy, NULL, (void*) planificador_GameBoy, NULL);
	pthread_create(&hilo_consola, NULL, (void*) consola, NULL);
	pthread_create(&hilo_ejecucion, NULL, (void*) planificador, NULL);

	pthread_join(hilo_consola, NULL);




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
	free(comando);
	log_destroy(logger);
	log_destroy(loggerCatedra);
	//free(comando);
	EXIT_TEAM = FALSE;
	sem_post(&elementosEnReady);

	for(int i = 0; i < cantEntrenadores; i++){
		pthread_mutex_unlock(mxEntrenadores + i);
	}


	pthread_cancel(hilo);
		pthread_detach(hilo);
	//	for (int i = 0; i < cantEntrenadores; i++){
		//	pthread_kill(hilo,9);
	//	}
		//pthread_join(hilo, NULL);

		pthread_cancel(hilo_APPEARED);
	 	pthread_detach(hilo_APPEARED);
		/*pthread_kill(hilo_APPEARED,9);
	 	pthread_join(hilo_APPEARED, NULL);*/

	 	pthread_cancel(hilo_LOCALIZED);
	 	pthread_detach(hilo_LOCALIZED);
	 //	pthread_kill(hilo_LOCALIZED,9);
	// 	pthread_join(hilo_LOCALIZED, NULL);

	 	pthread_cancel(hilo_CAUGHT);
	 	pthread_detach(hilo_CAUGHT);
	 //	pthread_kill(hilo_CAUGHT,9);
	 //	pthread_join(hilo_CAUGHT, NULL);

	 	pthread_cancel(hilo_gameboy);
	 	pthread_detach(hilo_gameboy);
	 //	pthread_kill(hilo_gameboy,9);
	 //	pthread_join(hilo_gameboy, NULL);

		pthread_cancel(hilo_servidor);
	 	pthread_detach(hilo_servidor);
	 	pthread_join(hilo_servidor, NULL);

		pthread_cancel(hilo_ejecucion);
	 	pthread_detach(hilo_ejecucion);
	 	pthread_join(hilo_ejecucion, NULL);


		for(int i = 0; i < cantEntrenadores; i++){
			pthread_mutex_lock(mxEntrenadores + i);
		}

		pthread_mutex_lock(&mutexColaExit);
			list_destroy_and_destroy_elements(colaExit, destroyItemExit);
		pthread_mutex_unlock(&mutexColaExit);

		pthread_mutex_lock(&mutexColaReady);
			list_destroy_and_destroy_elements(colaReady, destroyItemExit);
		pthread_mutex_unlock(&mutexColaReady);

		pthread_mutex_lock(&mutexColaBlocked);
			list_destroy_and_destroy_elements(colaBlocked, destroyItemExit);
		pthread_mutex_unlock(&mutexColaBlocked);
		pthread_mutex_lock(&mxEntrenadoresDeadLock);
			list_destroy_and_destroy_elements(entrenadoresEnDeadlock, destroyItemExit);
		pthread_mutex_unlock(&mxEntrenadoresDeadLock);
		pthread_mutex_lock(&mutexColaNew);
			list_destroy_and_destroy_elements(colaNew, destroyItemExit);
		pthread_mutex_unlock(&mutexColaNew);
		pthread_mutex_lock(&mutexListaCatch);
			list_destroy_and_destroy_elements(listaCatchPokemon, destroyItemCatch);
		pthread_mutex_unlock(&mutexListaCatch);

			list_destroy_and_destroy_elements(objetivoTeam, destroySubItem);
			list_destroy_and_destroy_elements(pokemonesBuscados, destroyItemPosBus);
			list_destroy(pokemonesEjecutando);



		free(mxEntrenadores);

		int tamLista = list_size(configFile->objetivosEntrenadores);
			for(int i = 0;i < tamLista ;i++){
				char* obj = list_get(configFile->objetivosEntrenadores,0);
				list_remove(configFile->objetivosEntrenadores,0);
				free(obj);
			}

		free(configFile->objetivosEntrenadores);

		tamLista = list_size(configFile->pokemonEntrenadores);

			for(int i = 0;i < tamLista ;i++){
				char* obj = list_get(configFile->pokemonEntrenadores,0);
				list_remove(configFile->pokemonEntrenadores,0);
				free(obj);
			}

		free(configFile->pokemonEntrenadores);

		tamLista = list_size(configFile->posicionEntrenadores);


			for(int i = 0; i < tamLista ;i++){
				char* obj = list_get(configFile->posicionEntrenadores,0);
				list_remove(configFile->posicionEntrenadores,0);
				free(obj);
			}


		free(configFile->posicionEntrenadores);
		free(configFile->algoritmoPlanificacion);
		free(configFile->ipBroker);
		free(configFile->logFile);
		free(configFile);

		config_destroy(config);






//para liberar a los entrenadores hay que ponerle sincro y no ejecutarlo hasta que todos los hilos terminen


	//pthread_detach( pthread_self() );
}

void servidor() {
	fdTeam = nuevoSocket();
	asociarSocket(fdTeam, configFile->puertoTeam);
	escucharSocket(fdTeam, CONEXIONES_PERMITIDAS);
	pthread_mutex_lock(&mutexLog);
	log_info(logger,"Escuchando conexiones");
	pthread_mutex_unlock(&mutexLog);
	while(EXIT_TEAM) {
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
	pthread_detach(pthread_self());
	pthread_cancel(pthread_self());
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
	ack.token = configFile->token;
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
	int conBroker = FALSE;
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
						//printf("Instrucción no reconocida");
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
	pthread_mutex_lock(&mxInitAppeared);
	if(initAppeared <= 0){
		pthread_mutex_unlock(&mxInitAppeared);
		pthread_mutex_lock(&mutexLogCatedra);
		log_info(loggerCatedra,"Recibí en la cola APPEARED_POKEMON . POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",app_poke->nombre_pokemon,app_poke->posicion_x,app_poke->posicion_y);
		pthread_mutex_unlock(&mutexLogCatedra);
		sem_wait(&entrenadoresLibres);
		if (pokemonNecesario(app_poke->nombre_pokemon) == TRUE) {

			posicionPokemon* newPoke = malloc(sizeof(posicionPokemon));
			newPoke->posiciones = list_create();
			posicion* pos = malloc(sizeof(posicion));
			pos->posicion_x = app_poke->posicion_x;
			pos->posicion_y = app_poke->posicion_y;
			list_add(newPoke->posiciones,pos);
			newPoke->tipoPokemon = malloc(1+string_length(app_poke->nombre_pokemon));
			strcpy(newPoke->tipoPokemon,app_poke->nombre_pokemon);
			_Bool pokeSeleccionado = true;
			//desBloquearSemaforoEnt(colaNew,1);
			pthread_mutex_lock(&mxListaCatch);
			pthread_mutex_lock(&mxListaEjecutando);
			//AHORA VERIFICO SI EL POKEMON BUSCADO NO ESTÁ EN CATCH O EJECUTANDOSE (NO DEBERÍAN)
			//entrenadorPokemon* proximoEntrenadorEnEjecutar = malloc(sizeof(entrenadorPokemon));
			entrenadorPokemon* proximoEntrenadorEnEjecutar = malloc(sizeof(entrenadorPokemon));
			seleccionarEntrenadorMasCercano(newPoke,pos, proximoEntrenadorEnEjecutar);
			pthread_mutex_unlock(&mxListaCatch);
			pthread_mutex_unlock(&mxListaEjecutando);
			char* pos_x = string_itoa(pos->posicion_x);
			char* pos_y = string_itoa(pos->posicion_y);
			char* proximaAccionEntrenador = malloc(string_length("AtraparPokemon   ") + string_length(newPoke->tipoPokemon) + string_length(pos_x) + string_length(pos_y) + 1);
			strcpy(proximaAccionEntrenador, "AtraparPokemon ");
			strcat(proximaAccionEntrenador, newPoke->tipoPokemon);
			strcat(proximaAccionEntrenador, " ");
			strcat(proximaAccionEntrenador, pos_x);
			strcat(proximaAccionEntrenador, " ");
			strcat(proximaAccionEntrenador, pos_y);
			//string_append_with_format(&proximaAccionEntrenador, "AtraparPokemon %s %i %i", app_poke.nombre_pokemon, app_poke.posicion_x, app_poke.posicion_y);
			proximoEntrenadorEnEjecutar->proximaAccion = malloc(strlen(proximaAccionEntrenador) + 1);
			strcpy(proximoEntrenadorEnEjecutar->proximaAccion, proximaAccionEntrenador);
			pthread_mutex_lock(&mutexColaReady);
			list_add(colaReady, proximoEntrenadorEnEjecutar);
			if(string_equals_ignore_case(configFile->algoritmoPlanificacion,"SJF-CD") || string_equals_ignore_case(configFile->algoritmoPlanificacion,"SJF-SD")){
				for(int i= 0; i<list_size(colaReady);i++){
					entrenadorPokemon* ent = list_get(colaReady,i);
					ent->estimacionUltimaRafaga = calcularRafagaCPU(ent);
				}
				_Bool ordenarSJF(entrenadorPokemon* a, entrenadorPokemon* b){return a->estimacionUltimaRafaga < b->estimacionUltimaRafaga;}
				list_sort(colaReady, (void*)ordenarSJF);
			}
			pthread_mutex_unlock(&mutexColaReady);
			free(pos_x);
			free(pos_y);
			free(proximaAccionEntrenador);
			free(newPoke->tipoPokemon);
			free(newPoke);
			sem_post(&elementosEnReady);
		} else {
			sem_post(&entrenadoresLibres);
		}
	}else{
		pthread_mutex_unlock(&mxInitAppeared);
	}
	free(app_poke->nombre_pokemon);
	free(app_poke);
	pthread_cancel(pthread_self());
	pthread_detach(pthread_self());

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
	int conBroker = FALSE;
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
				free(mensaje);
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
						free(mensaje);
						pthread_t hilo_CAUGHT;
						pthread_create(&hilo_CAUGHT, NULL, (void*) threadCaught, caug_poke);
					break;
					}
					default:
						//log_info(logger, "Instrucción no reconocida");
						//printf("Instrucción no reconocida");
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
	entrenadorPokemon* entrenador = malloc(sizeof(entrenadorPokemon));
	int res = FALSE;
	//int res = verificarMensajeRecibido(caug_poke->id_mensaje, entrenador);
	pthread_mutex_lock(&mutexColaBlocked);
		for (int posicionEntrenador = 0; posicionEntrenador < list_size(colaBlocked); posicionEntrenador++) {
			entrenador = list_get(colaBlocked, posicionEntrenador);
			if (caug_poke->id_tracking == entrenador->idMsjEsperado) {
				res = TRUE;
				break;
			}
		}
		pthread_mutex_unlock(&mutexColaBlocked);
	if (res == TRUE) {
		if (caug_poke->atrapo_pokemon == OK) {
			pokemonAtrapado(entrenador, caug_poke);
		}else{ //si no atrapó el Pokemon
			pthread_mutex_lock(&mxListaCatch);
			for(int posicionPokemon = 0; posicionPokemon < list_size(listaCatchPokemon); posicionPokemon++) {
					cola_CATCH_POKEMON* pokemonCatch = list_get(listaCatchPokemon, posicionPokemon);
					if(pokemonCatch->id_mensaje == caug_poke->id_tracking) {
						list_remove_and_destroy_element(listaCatchPokemon, posicionPokemon,destroyItemCatch);
						break;
					}
			}
			pthread_mutex_unlock(&mxListaCatch);

			entrenador->proximaAccion= realloc(entrenador->proximaAccion,1 + string_length(""));
			strcpy(entrenador->proximaAccion,"");
			caug_poke->id_tracking = 0;

		}
	}
	pthread_mutex_lock(&mutexLogCatedra);
	log_info(loggerCatedra,"Recibí en la cola CAUGHT_POKEMON . MENSAJE ID: %d  , ATRAPO: %d",caug_poke->id_mensaje,caug_poke->atrapo_pokemon);
	pthread_mutex_unlock(&mutexLogCatedra);
	free(caug_poke);
	pthread_cancel(pthread_self());
	pthread_detach(pthread_self());
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
	int conBroker = FALSE;
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
			/*if ((reg != ERROR) && (getEnviado == FALSE)) {
				getPokemon();
				getEnviado = TRUE;
			}*/
		}
		while (conBroker == TRUE) {

			int recibido = recibirProtocolo(&head,&bufferTam,fdLocalized); // recibo head y tamaño de msj
			void * mensaje = malloc(bufferTam);
			if (head < 1 || recibido <= 0){ // DESCONEXIÓN, DEBE RECONECTARSE, SALE DEL WHILE Y SE RECONECTA
				free(mensaje);
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
						loc_poke->lista_posiciones =list_create();
						deserealizar_LOCALIZED_POKEMON(head, mensaje, bufferTam, loc_poke);
						pthread_t hilo_LOCALIZED;
						pthread_create(&hilo_LOCALIZED, NULL, (void*) threadLocalized, loc_poke);
					break;
					}
					default:
						//log_info(logger, "Instrucción no reconocida");
						//printf("Instrucción no reconocida");
					break;
				}
				free(mensaje);
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

	if(loc_poke->cantidad >0){
	if (mensajeNoRecibido(loc_poke) == TRUE) {

		for (int i = 0 ; i < loc_poke->cantidad; i++){
			pthread_mutex_lock(&mutexLogCatedra);
			posicion* pos = list_get(loc_poke->lista_posiciones,i);
			log_info(loggerCatedra,"Recibí en la cola LOCALIZED_POKEMON . POKEMON: %s  , CANTIDAD: %d , POSICIÓN X: %d , POSICIÓN Y: %d",loc_poke->nombre_pokemon,loc_poke->cantidad,pos->posicion_x,pos->posicion_y);
			pthread_mutex_unlock(&mutexLogCatedra);
						//printf("Recibí en la cola LOCALIZED_POKEMON . POKEMON: %s  , CANTIDAD: %d , POSICIÓN X: %d , POSICIÓN Y: %d\n",loc_poke.nombre_pokemon,loc_poke.cantidad,list_get(loc_poke.lista_posiciones,i),list_get(loc_poke.lista_posiciones,i + 1));
			i++;
		}
		for (int cantPedidos = 0; cantPedidos < list_size(loc_poke->lista_posiciones); cantPedidos++) {
		sem_wait(&entrenadoresLibres);
		_Bool pokeSeleccionado = true;
		posicion *pos = malloc(sizeof(posicion));
		int indexPos;
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
			pthread_mutex_lock(&mxListaCatch);
			pthread_mutex_lock(&mxListaEjecutando);
			for(int j = 0;j < list_size(newPoke->posiciones) && pokeSeleccionado; j++){
				indexPos = j;
				pos = list_get(newPoke->posiciones,j);

							//AHORA VERIFICO SI EL POKEMON BUSCADO NO ESTÁ EN CATCH O EJECUTANDOSE (NO DEBERÍAN)
				_Bool buscar_poke(pokemon * lista){return lista->posicion_x == pos->posicion_x && lista->posicion_y == pos->posicion_y && string_equals_ignore_case(lista->nombre_pokemon, newPoke->tipoPokemon);}
				pokeSeleccionado = list_any_satisfy(listaCatchPokemon, (void*)buscar_poke);
				if(!pokeSeleccionado){
					pokeSeleccionado = list_any_satisfy(pokemonesEjecutando, (void*)buscar_poke);
				}
			}
			entrenadorPokemon* proximoEntrenadorEnEjecutar = malloc(sizeof(entrenadorPokemon));
			seleccionarEntrenadorMasCercano(newPoke,pos, proximoEntrenadorEnEjecutar);
			pthread_mutex_unlock(&mxListaCatch);
			pthread_mutex_unlock(&mxListaEjecutando);
			char* pos_x = string_itoa(pos->posicion_x);
			char* pos_y = string_itoa(pos->posicion_y);
			char* proximaAccionEntrenador = malloc(string_length("AtraparPokemon   ") + string_length(newPoke->tipoPokemon) + string_length(pos_x) + string_length(pos_y) + 1);
			strcpy(proximaAccionEntrenador, "AtraparPokemon ");
			strcat(proximaAccionEntrenador, newPoke->tipoPokemon);
			strcat(proximaAccionEntrenador, " ");
			strcat(proximaAccionEntrenador, pos_x);
			strcat(proximaAccionEntrenador, " ");
			strcat(proximaAccionEntrenador, pos_y);
			//string_append_with_format(&proximaAccionEntrenador, "AtraparPokemon %s %i %i", app_poke.nombre_pokemon, app_poke.posicion_x, app_poke.posicion_y);
			proximoEntrenadorEnEjecutar->proximaAccion = malloc(strlen(proximaAccionEntrenador) + 1);
			strcpy(proximoEntrenadorEnEjecutar->proximaAccion, proximaAccionEntrenador);
			pthread_mutex_lock(&mutexColaReady);
			list_add(colaReady, proximoEntrenadorEnEjecutar);

			if(string_equals_ignore_case(configFile->algoritmoPlanificacion,"SJF-CD") || string_equals_ignore_case(configFile->algoritmoPlanificacion,"SJF-SD")){
					for(int i= 0; i<list_size(colaReady);i++){
						entrenadorPokemon* ent = list_get(colaReady,i);
						ent->estimacionUltimaRafaga = calcularRafagaCPU(ent);
					}

					_Bool ordenarSJF(entrenadorPokemon* a, entrenadorPokemon* b){return a->estimacionUltimaRafaga < b->estimacionUltimaRafaga;}
					list_sort(colaReady, (void*)ordenarSJF);
				}
			pthread_mutex_unlock(&mutexColaReady);
			free(pos_x);
			free(pos_y);
			free(proximaAccionEntrenador);
			sem_post(&elementosEnReady);
		} else {
			sem_post(&entrenadoresLibres);
		}
		}
		free(loc_poke->nombre_pokemon);
		list_destroy(loc_poke->lista_posiciones);
		free(loc_poke);
	}
	}else{
		pthread_mutex_lock(&mutexLogCatedra);
		log_info(loggerCatedra,"Recibí en la cola LOCALIZED_POKEMON . POKEMON: %s  ,sin posiciones.",loc_poke->nombre_pokemon);
		free(loc_poke->nombre_pokemon);
		list_destroy(loc_poke->lista_posiciones);
		free(loc_poke);
		pthread_mutex_unlock(&mutexLogCatedra);
	}
	pthread_mutex_lock(&mxInitAppeared);
	initAppeared--;
	pthread_mutex_unlock(&mxInitAppeared);
	pthread_cancel(pthread_self());
	pthread_detach(pthread_self());
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

	while(EXIT_TEAM) {
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
	pthread_detach(pthread_self());
	pthread_cancel(pthread_self());
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
			sem_wait(&entrenadoresLibres);
			if (pokemonNecesario(app_poke->nombre_pokemon) == TRUE) {

				posicionPokemon* newPoke = malloc(sizeof(posicionPokemon));
				newPoke->posiciones = list_create();
				posicion* pos = malloc(sizeof(posicion));
				pos->posicion_x = app_poke->posicion_x;
				pos->posicion_y = app_poke->posicion_y;
				list_add(newPoke->posiciones,pos);
				newPoke->tipoPokemon = malloc(1+string_length(app_poke->nombre_pokemon));
				strcpy(newPoke->tipoPokemon,app_poke->nombre_pokemon);

				_Bool pokeSeleccionado = true;
						//desBloquearSemaforoEnt(colaNew,1);
				pthread_mutex_lock(&mxListaCatch);
				pthread_mutex_lock(&mxListaEjecutando);
				//AHORA VERIFICO SI EL POKEMON BUSCADO NO ESTÁ EN CATCH O EJECUTANDOSE (NO DEBERÍAN)
				entrenadorPokemon* proxEntrenadorEnEjecutar = malloc(sizeof(entrenadorPokemon));
			   seleccionarEntrenadorMasCercano(newPoke,pos, proxEntrenadorEnEjecutar);
				pthread_mutex_unlock(&mxListaCatch);
				pthread_mutex_unlock(&mxListaEjecutando);
				char* pos_x = string_itoa(pos->posicion_x);
				char* pos_y = string_itoa(pos->posicion_y);
				char* proximaAccionEntrenador = malloc(string_length("AtraparPokemon   ") + string_length(newPoke->tipoPokemon) + string_length(pos_x) + string_length(pos_y) + 1);
				strcpy(proximaAccionEntrenador, "AtraparPokemon ");
				strcat(proximaAccionEntrenador, newPoke->tipoPokemon);
				strcat(proximaAccionEntrenador, " ");
				strcat(proximaAccionEntrenador, pos_x);
				strcat(proximaAccionEntrenador, " ");
				strcat(proximaAccionEntrenador, pos_y);
				//string_append_with_format(&proximaAccionEntrenador, "AtraparPokemon %s %i %i", app_poke.nombre_pokemon, app_poke.posicion_x, app_poke.posicion_y);
				proxEntrenadorEnEjecutar->proximaAccion = malloc(strlen(proximaAccionEntrenador) + 1);
				strcpy(proxEntrenadorEnEjecutar->proximaAccion, proximaAccionEntrenador);
				pthread_mutex_lock(&mutexColaReady);
				list_add(colaReady, proxEntrenadorEnEjecutar);
				if(string_equals_ignore_case(configFile->algoritmoPlanificacion,"SJF-CD") || string_equals_ignore_case(configFile->algoritmoPlanificacion,"SJF-SD")){
					for(int i= 0; i<list_size(colaReady);i++){
						entrenadorPokemon* ent = list_get(colaReady,i);
						ent->estimacionUltimaRafaga = calcularRafagaCPU(ent);
					}
					_Bool ordenarSJF(entrenadorPokemon* a, entrenadorPokemon* b){return a->estimacionUltimaRafaga < b->estimacionUltimaRafaga;}
					list_sort(colaReady, (void*)ordenarSJF);
				}
				pthread_mutex_unlock(&mutexColaReady);
				free(pos_x);
				free(pos_y);
				free(proximaAccionEntrenador);
				for(int i = 0; i< list_size(newPoke->posiciones);i++){
					posicion* pos = list_get(newPoke->posiciones,i);
					free(pos);
				}
				list_destroy(newPoke->posiciones);
				free(newPoke->tipoPokemon);
				free(newPoke);
				free(app_poke->nombre_pokemon);
				free(app_poke);
				sem_post(&elementosEnReady);
			} else {
				sem_post(&entrenadoresLibres);
			}
		break;
		}
		case CAUGHT_POKEMON :{
			cola_CAUGHT_POKEMON * caug_poke ;
			//responde por caught_pokemon
			deserealizar_CAUGHT_POKEMON(head, mensaje, bufferTam, caug_poke);
			entrenadorPokemon* entrenador = malloc(sizeof(entrenadorPokemon));
			int res = verificarMensajeRecibido(caug_poke->id_tracking, entrenador);
			if (res == TRUE) {
				if (caug_poke->atrapo_pokemon == OK) {
					pthread_mutex_lock(&mxListaCatch);
					pokemonAtrapado(entrenador, caug_poke);
					pthread_mutex_unlock(&mxListaCatch);
				}
			}
			pthread_mutex_lock(&mutexLogCatedra);
			log_info(loggerCatedra,"Recibí en la cola CAUGHT_POKEMON . MENSAJE ID: %d  , ATRAPO: %d",caug_poke->id_mensaje,caug_poke->atrapo_pokemon);
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
	}
		}
		//ejecutar();
	free(mensaje);

	//pthread_mutex_lock(&mxHilos);
	pthread_mutex_lock(&mutexLog);
	log_info(logger, "Fin de hilo GameBoy");
	pthread_mutex_unlock(&mutexLog);
	//pthread_cancel( pthread_self() );
	pthread_detach( pthread_self() );
}



void thread_Entrenador(int idEntrenador) {

	//char* arrayAccion;
	while(EXIT_TEAM){ //VALIDAR SI VA CON UN WHILE TRUE
		//pthread_mutex_lock(&mxEntrenadoresList);
		pthread_mutex_lock(mxEntrenadores + idEntrenador);
		if(EXIT_TEAM == TRUE){
			pthread_mutex_unlock(&mxEntrenadoresList);
		//	pthread_mutex_unlock(&mxEntrenadoresList);
			_Bool buscarEntrenadorPorID2(entrenadorPokemon* entrenadorABuscar) {
				return entrenadorABuscar->idEntrenador == idEntrenador;
			}
			pthread_mutex_lock(&mutexColaReady);
			entrenadorPokemon* entrenador = list_find(colaReady, (void*) buscarEntrenadorPorID2);

			quitarDeColaReady(entrenador);
			pthread_mutex_unlock(&mutexColaReady);

		int llegoADestino = 0;
		char* accion = malloc(1 + string_length(entrenador->proximaAccion));
		strcpy(accion,entrenador->proximaAccion);
		if (!string_equals_ignore_case(accion,"")) {
			char* arrayAccion = malloc(string_length(accion) + 1);
			strcpy(arrayAccion,accion);
			char* accionStr = strtok(arrayAccion, " ");
			if (strcmp("AtraparPokemon", accionStr) == 0) {
				char* pokemonAAtrapar = strtok(NULL, " ");
				char* posicionXPokemon = strtok(NULL, " ");
				char* posicionYPokemon = strtok(NULL, " ");
				int posicionXInt = atoi(posicionXPokemon);
				int posicionYInt = atoi(posicionYPokemon);
				//printf("La accion es %s\n", accion);
				llegoADestino = moverEntrenador(entrenador, posicionXInt, posicionYInt);
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
					sem_post(&elementosEnReady);
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
				llegoADestino = moverEntrenador(entrenador, posicionXEntrenador2Int, posicionYEntrenador2Int);
				if (llegoADestino == 1) {
					realizarIntercambio(entrenador, entrenador2, atrapadoInnecesarioEntrenador1, atrapadoInnecesarioEntrenador2);
					strcpy(entrenador->proximaAccion, "");
					strcpy(entrenador2->proximaAccion, "");
					verificarEstado(entrenador);
					verificarDeadlock(entrenador2);
				} else {
					pthread_mutex_lock(&mutexLogCatedra);
					log_info(loggerCatedra, "Se movió al entrenador id %i a la cola de Ready para continuar su ejecución proximamente", entrenador->idEntrenador);
					pthread_mutex_unlock(&mutexLogCatedra);
					pthread_mutex_lock(&mutexColaReady);
					list_add(colaReady, entrenador);
					pthread_mutex_unlock(&mutexColaReady);
					sem_post(&elementosEnReady);
				}
				//verificarIntercambios();
			}
		//	free(arrayAccion);
		}
		free(accion);
		pthread_mutex_unlock(&mxEjecutando);
	}
}
	pthread_mutex_unlock(mxEntrenadores + idEntrenador);
	//free(arrayAccion);
	//free(mxEntrenadores + idEntrenador );
	//pthread_mutex_unlock(mxExitEntrenadores);
	//pthread_mutex_unlock(mxEntrenadores + idEntrenador);
	//pthread_cancel(pthread_self());
//	pthread_detach(pthread_self());

//	pthread_cancel( pthread_self() );
//	pthread_detach( pthread_self() );

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
