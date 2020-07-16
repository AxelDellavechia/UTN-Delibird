#ifndef GENERALES_H
#define GENERALES_H

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <signal.h>

#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../digiCommons/src/mensajeria.h"
#include "../digiCommons/src/protocolos_comunicacion.h"

#define LOG_PATH "../GameBoyCatedra.log"
#define LOG_PATH_INTERNO "../GameBoy.log"
#define RUTA_CONFIG_MEM "../configGB.txt"
#define FALSE 0
#define TRUE 1
#define ERROR -1
#define CONEXIONES_PERMITIDAS 100

int fdCliente ;

t_log * logger;

t_log * loggerCatedra;

pthread_t hilo_servidor;
pthread_t hilo_consola;
pthread_t hilo_finalizar;

pthread_mutex_t mxHilos;

int cantidadSegundos;
int segundosMaximos;
int tiempoSuscripcion;
char * comando;
void * mensaje;

suscriptor * laSuscripcion ;

pthread_mutex_t semaforo;
pthread_mutex_t semaforo2;

typedef struct config {
	int puertoBroker;
	int puertoTeam;
	int puertoGameCard;
	char* ipBroker;
	char* ipTeam;
	char* ipGameCard;
} archivoConfigGB;

archivoConfigGB *configGB;

int conexion;

void leerArchivoDeConfiguracion(char *ruta,t_log * logger);
void crearLogger( char * nombre , char * otroLog );
void * reservarMemoria(int size);
void inicializar_semaforos();
int conectaryLoguear(char * modulo , int fdServer , char * ipServer , int portServer,t_log* logger,t_log * loggerCatedra);

void servidor();
void crearHilos() ;
void consola() ;

void liberarRecursosComunes();
int finalizar();

void responderACK(int id_msj);

#endif
