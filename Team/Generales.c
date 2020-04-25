#include "Generales.h"
#include "../digiCommons/src/protocolos_comunicacion.h"

void crearLogger(char * nombre , char * path) {
	char * archivoLog = strdup(nombre);
	logger = log_create(path, archivoLog, FALSE, LOG_LEVEL_INFO);
	free(archivoLog);
	archivoLog = NULL;
}

void leerArchivoDeConfiguracion(char *ruta, t_log *logger) {

	configFile = reservarMemoria(sizeof(archivoConfigFile));
	t_config *config;
	config = config_create(ruta);
	log_info(logger, "Por setear los valores del archivo de configuracion");
	if (config != NULL) {
		log_info(logger, "TEAM: Leyendo Archivo de Configuracion..");
		if (config_has_property(config, "POSICIONES_ENTRENADORES")) {
			configFile->posicionEntrenadores = config_get_string_value(config, "POSICIONES_ENTRENADORES");
				log_info(logger, "Se encontró y cargó el contenido de POSICIONES_ENTRENADORES a la estructura TEAM. Valor: %s",configFile->posicionEntrenadores);
		} else {
			log_error(logger,"El archivo de configuracion no contiene el parametro POSICIONES_ENTRENADORES");
		}
		if (config_has_property(config, "POKEMON_ENTRENADORES")) {
			configFile->pokemonEntrenadores = config_get_int_value(config, "POKEMON_ENTRENADORES");
			log_info(logger, "Se encontró y cargó el contenido de POKEMON_ENTRENADORES a la estructura TEAM. Valor: %d",configFile->pokemonEntrenadores);
		} else {
			log_error(logger,"El archivo de configuracion no contiene el parametro POKEMON_ENTRENADORES");
		}
		if (config_has_property(config, "OBJETIVOS_ENTRENADORES")) {
			pokemonesNecesarios = config_get_int_value(config, "OBJETIVOS_ENTRENADORES");
			log_info(logger, "Se encontró y cargó el contenido de OBJETIVOS_ENTRENADORES a la estructura TEAM. Valor: %d",configFile->objetivosEntrenadores);
		} else {
			log_error(logger, "El archivo de configuracion no contiene el parametroOBJETIVOS_ENTRENADORES");
		}
		if (config_has_property(config, "TIEMPO_RECONEXION")) {
			//configFile->semIDs = (char *) malloc(sizeof config_get_array_value(config, "SEM_IDS"));
			configFile->tiempoReconexion = config_get_array_value(config, "TIEMPO_RECONEXION");
			log_info(logger, "Se encontró y cargó el contenido de TIEMPO_RECONEXION a la estructura TEAM. Valor: %p", configFile->tiempoReconexion);
		} else {
			log_error(logger, "El archivo de configuracion no contiene el parametro TIEMPO_RECONEXION");
		}
		if (config_has_property(config, "RETARDO_CICLO_CPU")) {
			configFile->retardoCicloCPU = config_get_array_value(config, "RETARDO_CICLO_CPU");
			log_info(logger, "Se encontró y cargó el contenido del RETARDO_CICLO_CPU a la estructura TEAM. Valor: %d",configFile->retardoCicloCPU);
		} else {
			log_error(logger, "El archivo de configuracion no contiene el parametro RETARDO_CICLO_CPU");
		}
		if (config_has_property(config, "ALGORITMO_PLANIFICACION")) {
			configFile->algoritmoPlanificacion = strdup(config_get_array_value(config, "ALGORITMO_PLANIFICACION"));
			log_info(logger, "Se encontró y cargó el contenido del ALGORITMO_PLANIFICACION a la estructura TEAM. Valor: %d",configFile->algoritmoPlanificacion);
		} else {
			log_error(logger,"El archivo de configuracion no contiene el parametro ALGORITMO_PLANIFICACION");
		}
		if (config_has_property(config, "QUANTUM")) {
			configFile->quantum = config_get_int_value(config, "QUANTUM");
			log_info(logger, "Se encontró y cargó el contenido del QUANTUM a la estructura TEAM. Valor: %d", configFile->quantum);
		} else {
			log_error(logger, "El archivo de configuracion no contiene el parametro QUANTUM");
		}
		if (config_has_property(config, "IP_BROKER")) {
			configFile->ipBroker = config_get_int_value(config, "IP_BROKER");
			log_info(logger, "Se encontró y cargó el contenido del IP_BROKER a la estructura TEAM. Valor: %d", configFile->ipBroker);
		} else {
			log_error(logger, "El archivo de configuracion no contiene el parametro IP_BROKER");
		}
		if (config_has_property(config, "ESTIMACION_INICIAL")) {
			configFile->estimacionInicial = config_get_int_value(config, "ESTIMACION_INICIAL");
			log_info(logger, "Se encontró y cargó el contenido del ESTIMACION_INICIAL a la estructura TEAM. Valor: %d", configFile->estimacionInicial);
		} else {
			log_error(logger, "El archivo de configuracion no contiene el parametro ESTIMACION_INICIAL");
		}
		if (config_has_property(config, "PUERTO_BROKER")) {
			configFile->puertoBroker = config_get_int_value(config, "PUERTO_BROKER");
			log_info(logger, "Se encontró y cargó el contenido del PUERTO_BROKER a la estructura TEAM. Valor: %d", configFile->puertoBroker);
		} else {
			log_error(logger, "El archivo de configuracion no contiene el parametro PUERTO_BROKER");
		}
		if (config_has_property(config, "LOG_FILE")) {
			configFile->logFile = config_get_int_value(config, "LOG_FILE");
			log_info(logger, "Se encontró y cargó el contenido del LOG_FILE a la estructura TEAM. Valor: %d", configFile->logFile);
		} else {
			log_error(logger, "El archivo de configuracion no contiene el parametro LOG_FILE");
		}
		config_destroy(config); // Libero la estructura archivoConfig
		log_info(logger,"El archivo de configuración ha sido leído correctamente");
	}
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
	pokemonesRequeridos = list_create();
	pokemonesNecesarios = list_create();
	colaNew = list_create();
	colaReady = list_create();
	int exec;
	colaBlocked = list_create();
	colaExit = list_create();
	mapaPokemon = list_create();
	posicion = list_create();
	pokemonesObjetivo = list_create();
	pokemonesAtrapados = list_create();
}

void appeared_pokemon(posicionPokemon pokemonAparecido) {
	list_add(mapaPokemon, pokemonAparecido.tipoPokemon);
	verificarPokemonesNecesarios(pokemonAparecido.tipoPokemon);
}

void verificarPokemonesNecesarios(char* especiePokemon) {
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
}

void get_pokemon() {
	if (validar_conexion) {
		//int listaPokemon = 0;
		//while (pokemonesNecesarios[listaPokemon].head->next != NULL) {
		for (int listaPokemon = 0; listaPokemon <= sizeof(pokemonesNecesarios); listaPokemon++) {
			t_list* objetivosEntrenadores = list_get(pokemonesNecesarios, listaPokemon);
			//int objetivoEntrenador = 0;
			for (int objetivoEntrenador = 0; objetivoEntrenador <= sizeof(objetivosEntrenadores); objetivoEntrenador++) {
			//while (objetivosEntrenadores[objetivoEntrenador].head->next != NULL) {
				char* objetivoEntrenadorStr = list_get(objetivosEntrenadores, objetivoEntrenador);
				//enviar_mensaje(GET_POKEMON(objetivoEntrenadorStr));
				//void* posicionesPokemon = aplicar_protocolo_recibir(fdBroker, 0);
				//objetivoEntrenador++;
			}
			//listaPokemon++;
		}
		/*for (int listaPokemon = 0; listaPokemon < sizeof(pokemonesNecesarios); listaPokemon++) {

		for (int objetivoEntrenador = 0; objetivoEntrenador < sizeof(objetivosEntrenadores); objetivoEntrenador++) {

			}
		}*/
	} else {
		void* posicionesPokemon = NULL;
	}
}

void catch_pokemon(posicionPokemon pokemon, int entrenador) {
	if (validar_conexion) {
		//aplicar_protocolo_enviar(fdBroker, 0, CATCH_POKEMON(pokemon));
		void* resultadoCatch = aplicar_protocolo_recibir(fdBroker, 0);
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
