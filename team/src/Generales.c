#include "Generales.h"


void iniciar_logCatedra(){

	char ** aux =  string_split(configFile->logFile,"/") ;
	int i = 0 ;
	while (aux[i] != NULL) {
		if ( string_contains(aux[i],"log") ) {
			char * archivoLogCatedra = strdup(aux[i]);
			loggerCatedra = log_create(configFile->logFile, archivoLogCatedra, FALSE, LOG_LEVEL_INFO);
			free(archivoLogCatedra);
			archivoLogCatedra = NULL;
		}
		i++;
	}
	free(aux);
}

void iniciar_log(){

	char * archivoLog = strdup("broker.log");

	logger = log_create(LOG_PATH_INTERNO, archivoLog, FALSE, LOG_LEVEL_INFO);

	free(archivoLog);
	archivoLog = NULL;
}

void leerArchivoDeConfiguracion(char *ruta, t_log *logger) {

	setlocale(LC_ALL,"");

	int posicionLista = 0;

	configFile = reservarMemoria(sizeof(archivoConfigFile));

	t_config *config;

	log_info(logger, "Leyendo Archivo de Configuracion..");

	config = config_create(ruta);

	char ** posicionEntrenadoresAux =  config_get_array_value(config, "POSICIONES_ENTRENADORES") ;
	char ** pokemonEntrenadoresAux =  config_get_array_value(config, "POKEMON_ENTRENADORES") ;
	char ** objetivosEntrenadoresAux =  config_get_array_value(config, "OBJETIVOS_ENTRENADORES") ;

	configFile->tiempoReconexion = config_get_int_value(config, "TIEMPO_RECONEXION");
	log_info(logger,"Se encontró y cargó el contido del TIEMPO_RECONEXION. Valor: %d",configFile->tiempoReconexion);

	configFile->retardoCicloCPU = config_get_int_value(config, "RETARDO_CICLO_CPU");
	log_info(logger,"Se encontró y cargó el contido del RETARDO_CICLO_CPU. Valor: %d",configFile->retardoCicloCPU);

	configFile->algoritmoPlanificacion = strdup ( config_get_string_value(config, "ALGORITMO_PLANIFICACION"));
	log_info(logger,"Se encontró y cargó el contido del ALGORITMO_PLANIFICACION. Valor: %s",configFile->algoritmoPlanificacion);

	configFile->quantum = config_get_int_value(config, "QUANTUM");
	log_info(logger,"Se encontró y cargó el contido del QUANTUM. Valor: %d",configFile->quantum);

	configFile->ipBroker = strdup ( config_get_string_value(config, "IP_BROKER"));
	log_info(logger,"Se encontró y cargó el contido del IP_BROKER. Valor: %s",configFile->ipBroker);

	configFile->estimacionInicial = config_get_int_value(config, "ESTIMACION_INICIAL");
	log_info(logger,"Se encontró y cargó el contido del ESTIMACION_INICIAL. Valor: %d",configFile->estimacionInicial);

	configFile->puertoBroker = config_get_int_value(config, "PUERTO_BROKER");
	log_info(logger,"Se encontró y cargó el contido del PUERTO_BROKER. Valor: %d",configFile->puertoBroker);

	configFile->puertoTeam = config_get_int_value(config, "PUERTO_TEAM");
	log_info(logger,"Se encontró y cargó el contido del PUERTO_TEAM. Valor: %d",configFile->puertoTeam);

	configFile->logFile = strdup ( config_get_string_value(config, "LOG_FILE"));
	log_info(logger,"Se encontró y cargó el contido del LOG_FILE. Valor: %s",configFile->logFile);

	configFile->posicionEntrenadores = list_create();
	configFile->pokemonEntrenadores = list_create();
	configFile->objetivosEntrenadores = list_create();
	while(posicionEntrenadoresAux[posicionLista] != NULL) {
		list_add(configFile->posicionEntrenadores, string_duplicate(posicionEntrenadoresAux[posicionLista]));
		posicionLista++;
	}
	if (! list_is_empty(configFile->posicionEntrenadores)) log_info(logger,"Se encontró y cargó el contido del POSICIONES_ENTRENADORES");
	posicionLista = 0;
	while(pokemonEntrenadoresAux[posicionLista] != NULL) {
		list_add(configFile->pokemonEntrenadores, string_duplicate(pokemonEntrenadoresAux[posicionLista]));
		posicionLista++;
	}
	if (! list_is_empty(configFile->pokemonEntrenadores)) log_info(logger,"Se encontró y cargó el contido del POKEMON_ENTRENADORES");
	posicionLista = 0;
	while(objetivosEntrenadoresAux[posicionLista] != NULL) {
		list_add(configFile->objetivosEntrenadores, string_duplicate(objetivosEntrenadoresAux[posicionLista]));
		posicionLista++;
	}
	if (! list_is_empty(configFile->objetivosEntrenadores)) log_info(logger,"Se encontró y cargó el contido del OBJETIVOS_ENTRENADORES");
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
	int exec;
	colaBlocked = list_create();
	colaExit = list_create();
	mapaPokemon = list_create();
}

void obtenerEntrenadores(t_log *logger) {
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
	list_add(colaReady, proximoEntrenadorAReady);
}

void catch_pokemon(posicionPokemon pokemon, int entrenador) {
	if (validar_conexion) {
		//aplicar_protocolo_enviar(fdBroker, 0, CATCH_POKEMON(pokemon));
		void* resultadoCatch = aplicar_protocolo_recibir(fdTeam, 0);
		caught_pokemon(pokemon, resultadoCatch);
		list_add(colaBlocked, entrenador);
	} else {
		caught_pokemon(pokemon, "Capturado");
	}
}

void caught_pokemon(posicionPokemon pokemon, void* resultadoCatch) {

}

void localized_pokemon(posicionPokemon pokemonEncontrado) {
	int posicion = 0;
	while (mapaPokemon[posicion].head->next != NULL) {
		_Bool buscarPokemon(posicionPokemon* pokemonEncontrado) {
			return pokemonEncontrado == mapaPokemon[posicion].head->data;
		}
		void* pokemonYaConocido = list_find(mapaPokemon, (void*) buscarPokemon);
	}

}

void inicializar_semaforos(){
	//inicializo semaforos de nodos
	pthread_mutex_init(&semaforo, NULL);
	pthread_mutex_init(&mxHilos, NULL);
}

void crearHilos() {


	hilo_servidor= 0;
	hilo_consola= 0;
	pthread_create(&hilo_servidor, NULL, (void*) servidor, NULL);

	pthread_create(&hilo_consola, NULL, (void*) consola, NULL);

	pthread_join(hilo_servidor, NULL);
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
			log_error(logger,"Error en getline");
		}
		if (bytes_read == 1) {
			continue;
		}
	}

	log_destroy(logger);
	free(comando);

	pthread_detach(hilo_servidor);
	pthread_detach( pthread_self() );
	pthread_cancel(hilo_servidor);
}

void servidor() {

	fdTeam = nuevoSocket();
	asociarSocket(fdTeam, configFile->puertoTeam);
	escucharSocket(fdTeam, CONEXIONES_PERMITIDAS);

	fd_set setAux;
	int maxFD,i,socket_nuevo;

	FD_ZERO(&setMaestro); 	// borra los conjuntos maestro y temporal
	FD_ZERO(&setAux);

	maxFD = fdTeam; //Llevo control del FD maximo de los sockets
	FD_SET(fdTeam, &setMaestro); //agrego el FD del socketEscucha al setMaestro


	log_info(logger," Escuchando conexiones. Socket: %d",fdTeam);


	while(TRUE) {
		pthread_mutex_lock(&semaforo);
		setAux = setMaestro;
		pthread_mutex_unlock(&semaforo);

				if (select((maxFD + 1), &setAux, NULL, NULL, NULL ) == -1) {
					printf("Error en la escucha.\n");
					log_error(logger,"Error en la escucha.\n" );
					return;
				}

		int conexionNueva = 0;
		int comandoNuevo;//= reservarMemoria(INT);

		for (i = 0; i <= maxFD; i++) {
					pthread_mutex_lock(&semaforo);
					int check = FD_ISSET(i,&setAux);
					pthread_mutex_unlock(&semaforo);
					if (check) { // Me fijo en el set de descriptores a ver cual respondió
						if (i == fdTeam) { //Tengo un nuevo hilo queriendose conectar
							//Esta funcion acepta una nueva conexion
							//y agrega un nuevo nodo a la lista de scripts con el nuevo socket

							log_info(logger,"Nuevo Cliente Conectado");
							comandoNuevo = aceptarConexionSocket(fdTeam);
							if (comandoNuevo == -1){
								return;
							}

							conexionNueva = handshake_servidor (comandoNuevo, "Team" , "Broker",logger);
							if( ! validar_conexion(conexionNueva, 0,logger) ) {
									pthread_mutex_lock(&semaforo); //desbloquea el semaforo
									FD_CLR(i, &setMaestro); // borra el file descriptor del set
									pthread_mutex_unlock(&semaforo);
									cerrarSocket(i);
									continue; // vuelve al inicio del while
							}else{
									pthread_mutex_lock(&semaforo);
									FD_SET(comandoNuevo, &setMaestro); //agrego el nuevo socket al setMaestro
									pthread_mutex_unlock(&semaforo);
									if (comandoNuevo > maxFD) maxFD = comandoNuevo;
						}
						}else { // Hay actividad nueva en algún hilo de kernel
							//SI RECIBO TRUE EN CONEXIONNUEVA, ABRO UN HILO POR CADA UNO QUE SE CONECTE
							pthread_t hilo;
							pthread_mutex_lock(&mxHilos);
							pthread_create(&hilo, NULL, (void*) thread_Team, i);
							pthread_mutex_unlock(&mxHilos);
							//pthread_join(hilo_SacCli, NULL);
						}
					}

		}
		//free(comandoNuevo);

	}
}

int thread_Team(int fdCliente) {

	int head = 0;
	int result = 0;

		void* mensaje = aplicar_protocolo_recibir(fdCliente, &head); // recibo mensajes

		log_info(logger, "metodo thread_Team recibio: %d", mensaje);

		if ( mensaje != NULL ) {

			log_info(logger, "head: %i\n",head);


			switch(head){

						case NEW_POKEMON :
							log_info(logger, "NEW_POKEMON");
						break;
						case APPEARED_POKEMON :{
							cola_APPEARED_POKEMON * app_poke = (cola_APPEARED_POKEMON *) mensaje;
							log_info(logger,"Recibí en la cola APPEARED_POKEMON -> POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",app_poke->nombre_pokemon,app_poke->posicion_x,app_poke->posicion_y);

							break;
						}
						case CATCH_POKEMON :
							log_info(logger, "CATCH_POKEMON");
						break;
						case CAUGHT_POKEMON :
							log_info(logger, "CAUGHT_POKEMON");
						break;
						case GET_POKEMON :
							log_info(logger, "GET_POKEMON");
						break;
						case LOCALIZED_POKEMON :
							log_info(logger, "LOCALIZED_POKEMON");
						break;
						default:
							log_info(logger, "Instrucción no reconocida");
							break;
					}
			free(mensaje);

			//pthread_detach( pthread_self() );
			//return TRUE;
				//return FALSE;
		};
	pthread_detach( pthread_self() );
	log_info(logger, "saliendo metodo thread_Team");
	return FALSE;

}
