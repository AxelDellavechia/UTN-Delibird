
#ifndef INICIALIZACION_H_
#define INICIALIZACION_H_
#include "GameCard.h"
#include <commons/config.h>
#include <commons/string.h>
#include <pthread.h>

#define LOG_PATH "./GAMECARD.log"
#define CONEXIONES_PERMITIDAS 100

#define FALSE 0
#define TRUE 1
//codigos de error
#define OK 1
#define ERROR -1
#define KEY_HANDSHAKE "GameCard"

typedef struct{
	int TIEMPO_DE_REINTENTO_CONEXION;
	int TIEMPO_DE_REINTENTO_OPERACION;
	char* PUNTO_MONTAJE_TALLGRASS;
	char* IP_BROKER;
	int PUERTO_BROKER;
	int PUERTO_GAMECARD;
} ConfigFile;

typedef struct{
	  	int tamanio_bloques;
	 	char* magic_number;
	 	int cantidad_bloques;
}t_config_MetaData;

typedef struct{
	char* PUNTOMONTAJE;
	char* METADATA;
	char* BLOCKS;
	char* FILES;

}t_config_PuntosMontaje;

void leer_configFile();
void crearPuntoMontaje();
void consola();
void servidor();

ConfigFile* config_File;
t_config_MetaData* config_MetaData;
t_config_PuntosMontaje* PuntoMontaje;
t_log* logger;
pthread_t hilo_servidor;
pthread_t hilo_consola;

int comandoIn;
int HANDSHAKE_SIZE;
fd_set setMaestro;


#endif /* INICIALIZACION_H_ */