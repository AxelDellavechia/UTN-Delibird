#ifndef GENERALES_H
#define GENERALES_H

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <signal.h>

#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define LOG_PATH "../CatedraGB.log"
#define LOG_PATH_INTERNO "../GameBoy.log"
#define RUTA_CONFIG_MEM "../configGB.txt"
#define FALSE 0
#define TRUE 1
#define ERROR -1
#define CONEXIONES_PERMITIDAS 100

t_log * logger;

t_log * loggerCatedra;

pthread_mutex_t semaforo;

typedef struct config {
	int puertoBroker;
	int puertoTeam;
	int puertoGameCard;
	char* ipBroker;
	char* ipTeam;
	char* ipGameCard;
} archivoConfigGB;

archivoConfigGB *configGB;

typedef enum {
	NEW_POKEMON = 1,
	APPEARED_POKEMON,
	CATCH_POKEMON,
	CAUGHT_POKEMON,
	GET_POKEMON,
	LOCALIZED_POKEMON,
	FIN_DEL_PROTOCOLO
} protocolo;

typedef enum {
	GAMECARD = 1,
	BROKER,
	TEAM
} modulos;

int fdBroker;
int fdTeam;
int fdGameCard;
int cxBroker;
int cxTeam;
int cxGameCard;

void leerArchivoDeConfiguracion(char *ruta,t_log * logger);
void crearLogger( char * nombre , char * otroLog );
void * reservarMemoria(int size);
void inicializar_semaforos();
void conectaryLoguear(int modulo , int conexion , int fdServer , char * ipServer , int portServer,t_log* logger,t_log * loggerCatedra);

#endif
