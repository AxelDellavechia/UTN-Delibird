#ifndef GENERALES_H
#define GENERALES_H

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>

#include "../digiCommons/src/protocolos_comunicacion.h"

#include "../digiCommons/src/mensajeria.h"

#include <math.h>
#include <signal.h>
#include <limits.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <locale.h>

#define LOG_PATH_INTERNO "../Team.log"
#define RUTA_CONFIG_MEM "../configTeam.txt"
#define FALSE 0
#define TRUE 1
#define ERROR -1
#define CONEXIONES_PERMITIDAS 100

t_log * logger;
t_log * loggerCatedra;

t_list* objetivoTeam;
t_list* colaNew;
t_list* colaReady;
t_list* colaBlocked;
t_list* colaExit;
t_list* mapaPokemon;

pthread_mutex_t mxHilos;
pthread_mutex_t mxSocketsFD;

pthread_t hilo_servidor;
pthread_t hilo_consola;

fd_set setMaestro;

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
	int puertoTeam;
	char* logFile;
} archivoConfigFile;

archivoConfigFile *configFile;

typedef struct posicionPokemon {
	char* tipoPokemon;
	int posicion_x;
	int posicion_y;
} posicionPokemon;

typedef struct entrenadorPokemon {
	int idEntrenador;
	int posicion_x;
	int posicion_y;
	t_list* pokemonesAtrapados;
	t_list* pokemonesObjetivo;
} entrenadorPokemon;

int fdTeam;

void leerArchivoDeConfiguracion(char *ruta,t_log * logger);
void iniciar_log();
void iniciar_logCatedra();
//void capturarError(int signal);
void * reservarMemoria(int size);
void inicializar_semaforos();
//char* obtenerElementoPorPosicion(char* array, int posicionEnArray, char* corte);
void consola();
void servidor();
void crearHilos();
int thread_Team(int fdCliente);

#endif
