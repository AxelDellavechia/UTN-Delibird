#include "Generales.h"
#include "../digiCommons/src/protocolos_comunicacion.h"

void crearLogger(char * nombre , char * path) {
	char * archivoLog = strdup(nombre);
	logger = log_create(path, archivoLog, FALSE, LOG_LEVEL_INFO);
	free(archivoLog);
	archivoLog = NULL;
}

void leerArchivoDeConfiguracion(char *ruta, t_log *logger) {
	int posicionLista = 0;
	configFile = reservarMemoria(sizeof(archivoConfigFile));
	t_config *config;
	config = config_create(ruta);
	log_info(logger, "Por setear los valores del archivo de configuracion");
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
	log_info(logger,"El archivo de configuración ha sido leído correctamente");
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
}

void obtenerEntrenadores() {
	int listaEntrenadores = 0;
	while(configFile->posicionEntrenadores[listaEntrenadores] != NULL) {
		char* posicionEntrenador = configFile->posicionEntrenadores[listaEntrenadores].head->data;
		char* pokemonesEntrenador = configFile->pokemonEntrenadores[listaEntrenadores].head->data;
		char* objetivosEntrenador = configFile->objetivosEntrenadores[listaEntrenadores].head->data;
		char* arrayCortado = strtok(posicionEntrenador, "|");
		char* posicionXStr = arrayCortado;
		char*posicionYStr = strtok(NULL, "|");
		int posicionX = atoi(posicionXStr);
		int posicionY = atoi(posicionYStr);
		entrenadorPokemon* entrenador = reservarMemoria(sizeof(entrenadorPokemon));
		entrenador->pokemonesAtrapados = list_create();
		entrenador->pokemonesObjetivo = list_create();
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
	}
}

void crearEntrenador(int posicionX, int posicionY, char* pokemonesEntrenador, char* objetivosEntrenador) {
	int posicion = 0;
	entrenadorPokemon* entrenador = reservarMemoria(sizeof(entrenadorPokemon));
	entrenador->pokemonesAtrapados = list_create();
	entrenador->pokemonesObjetivo = list_create();
	entrenador->posicion_x = posicionX;
	entrenador->posicion_y = posicionY;
	while (pokemonesEntrenador[posicion] != NULL) {
		list_add(entrenador->pokemonesAtrapados, pokemonesEntrenador[posicion]);
		posicion++;
	}
	posicion = 0;
	while (objetivosEntrenador[posicion] != NULL) {
		list_add(entrenador->pokemonesObjetivo, objetivosEntrenador[posicion]);
		posicion++;
	}

	//log_info(logger, pokemonesEntrenador);
	//log_info(logger, objetivosEntrenador);
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
	//if (validar_conexion) {
		t_list* pokemonEntrenadores = configFile->pokemonEntrenadores;
		char* elemento = pokemonEntrenadores[0].head->data;
		log_info(logger, elemento);
	/*} else {
		void* posicionesPokemon = NULL;
	}*/
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
