#ifndef GENERALES_H
#define GENERALES_H

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <signal.h>

#include <limits.h>

#define LOG_PATH "../CatedraGB.log"
#define LOG_PATH_INTERNO "../GameBoy.log"
#define RUTA_CONFIG_MEM "../configGB.txt"
#define FALSE 0
#define TRUE 1
#define ERROR -1
#define CONEXIONES_PERMITIDAS 100

t_log * logger;
t_log * loggerCatedra;
t_list* pokemonesRequeridos;
t_list* pokemonesNecesarios;
t_list* colaNew;
t_list* colaReady;
t_list* colaBlocked;
t_list* colaExit;
t_list* mapaPokemon;
t_list* posicion;
t_list* pokemonesObjetivo;
t_list* pokemonesAtrapados;

pthread_mutex_t semaforo;

typedef struct config {
	char* posicionEntrenadores;
	char* pokemonEntrenadores;
	char* objetivosEntrenadores;
	int tiempoReconexion;
	int retardoCicloCPU;
	char* algoritmoPlanificacion;
	int quantum;
	char* ipBroker;
	int estimacionInicial;
	int puertoBroker;
	char* logFile;
} archivoConfigFile;

archivoConfigFile *configFile;

typedef struct posicionPokemon {
	char* tipoPokemon;
	t_list* posicion;
} posicionPokemon;

typedef struct entrenadorPokemon {
	t_list* pokemonesObjetivo;
	t_list* pokemonesAtrapados;
} entrenadorPokemon;

int fdBroker;

void leerArchivoDeConfiguracion(char *ruta,t_log * logger);
void crearLogger(char * nombre , char * path);
void capturarError(int signal);
void * reservarMemoria(int size);
void inicializar_semaforos();

#endif
