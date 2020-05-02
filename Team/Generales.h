#ifndef GENERALES_H
#define GENERALES_H

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <signal.h>

#include <limits.h>

#define LOG_PATH "../CatedraTeam.log"
#define LOG_PATH_INTERNO "../Team.log"
#define RUTA_CONFIG_MEM "configTeam.txt"
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

pthread_mutex_t semaforo;

typedef struct archivoConfigFile {
	t_list* posicionEntrenadores;
	t_list* pokemonEntrenadores;
	t_list* objetivosEntrenadores;
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
	int posicion_x;
	int posicion_y;
} posicionPokemon;

typedef struct entrenadorPokemon {
	int posicion_x;
	int posicion_y;
	t_list* pokemonesAtrapados;
	t_list* pokemonesObjetivo;
} entrenadorPokemon;

int fdBroker;

void leerArchivoDeConfiguracion(char *ruta,t_log * logger);
void crearLogger(char * nombre , char * path);
void capturarError(int signal);
void * reservarMemoria(int size);
void inicializar_semaforos();
char* obtenerElementoPorPosicion(char* array, int posicionEnArray, char* corte);

#endif
