#include "GameCard.h"
#include <commons/config.h>
#include <pthread.h>

#define LOG_PATH "./GAMECARD.log"

typedef struct{
	int TIEMPO_DE_REINTENTO_CONEXION;
	int TIEMPO_DE_REINTENTO_OPERACION;
	char* PUNTO_MONTAJE_TALLGRASS;
	char* IP_BROKER;
	int PUERTO_BROKER;
} ConfigFile;


void leer_configFile();
void consola();
void servidor();

ConfigFile* config_File;
t_log* logger;
pthread_t hilo_servidor;
pthread_t hilo_consola;
