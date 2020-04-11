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

int fdBroker;
int fdTeam;
int fdGameCard;

void leerArchivoDeConfiguracion(char *ruta,t_log * logger);
void crearLogger(char * nombre , char * path);
void capturarError(int signal);
void * reservarMemoria(int size);
void inicializar_semaforos();

#endif
