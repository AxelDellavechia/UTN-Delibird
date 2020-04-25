#include "GameCard.h"
#include <commons/config.h>

#define LOG_PATH "./GAMECARD.log"

typedef struct{
	int TIEMPO_DE_REINTENTO_CONEXION;
	int TIEMPO_DE_REINTENTO_OPERACION;
	char* PUNTO_MONTAJE_TALLGRASS;
	char* IP_BROKER;
	int PUERTO_BROKER;
} ConfigFile;


void leer_configFile();


ConfigFile* config_File;
t_log* logger;

