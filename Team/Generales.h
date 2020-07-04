#ifndef GENERALES_H
#define GENERALES_H

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <signal.h>
#include <pthread.h>
#include <limits.h>
#include "../digiCommons/src/protocolos_comunicacion.h"
#include <math.h>
#include "../digiCommons/src/mensajeria.h"

#define LOG_PATH "../CatedraTeam.log"
#define LOG_PATH_INTERNO "../Team.log"
#define RUTA_CONFIG_MEM "configTeam.txt"
#define FALSE 0
#define TRUE 1
#define OK 0
#define ERROR -1
#define CONEXIONES_PERMITIDAS 100

t_log* logger;
t_log* loggerCatedra;
t_list* objetivoTeam;
t_list* colaNew;
t_list* colaReady;
t_list* colaBlocked;
t_list* colaExit;
t_list* entrenadoresEnDeadlock;
t_list* listaCatchPokemon;

int fdBroker;
int ciclosEnCPU;
int fdTeam;
int conBroker;
int idMensajeEsperado;
int crearLista;
pthread_t hilo_servidor;
pthread_t hilo_consola;
pthread_t * hilo;
pthread_t hilo_conexion;

pthread_mutex_t mxHilos;
pthread_mutex_t mxSocketsFD;
pthread_mutex_t h_reconectar;

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

typedef struct pokemon {
	int id_mensaje;
	char* nombre_pokemon;
	int tamanio_nombre;
	int posicion_x;
	int posicion_y;
}pokemon;

typedef struct entrenadorPokemon {
	int idEntrenador;
	int posicion_x;
	int posicion_y;
	t_list* pokemonesAtrapados;
	t_list* pokemonesObjetivo;
	int ciclosEnCPU;
	char* proximaAccion;
	pthread_mutex_t semaforMutex;
	int idMsjEsperado;
} entrenadorPokemon;

entrenadorPokemon* exec;
char ** aux ;

void leerArchivoDeConfiguracion(char *ruta,t_log * logger);
void capturarError(int signal);
void * reservarMemoria(int size);
void inicializar_semaforos();
char* obtenerElementoPorPosicion(char* array, int posicionEnArray, char* corte);
void quitarPokemonDeAtrapados(entrenadorPokemon* entrenador, char* pokemon);
void verificarIntercambios();
char* obtenerPokemonObjetivoFaltante(entrenadorPokemon* entrenador);
char* obtenerPokemonAtrapadoInnecesario(entrenadorPokemon* entrenador);
void quitarDeColaBlocked(entrenadorPokemon* entrenador);
void realizarAccion(entrenadorPokemon* entrenador, int tiempo);
void servidor();
void consola();
int thread_Team(int fdCliente);
void crearHilos();
void planificador();
void reconectar();
void thread_Entrenador(entrenadorPokemon * elEntrenador);
void iniciar_log();
void iniciar_logCatedra();
void crearLogger(char * nombre , char * path);
void obtenerEntrenadores();
void planificador();
void crearEstructuras();
entrenadorPokemon* seleccionarEntrenadorMasCercano(cola_APPEARED_POKEMON *pokemonAparecido);
void catchPokemon(entrenadorPokemon* entrenador, char* nombrePokemon, int posicionX, int posicionY);
entrenadorPokemon* verificarMensajeRecibido(int idMensajeRecibido);
void pokemonAtrapado(entrenadorPokemon* entrenador, cola_CAUGHT_POKEMON* pokemon);
void ejecutar();

int calcularRafagaCPU(accion);

#endif
