#include "Inicializacion.h"
#include <commons/config.h>

void leer_configFile(){
	t_config *config;
		config = config_create("/GameCard.config");
		if (config != NULL) {
			config_File->TIEMPO_DE_REINTENTO_CONEXION = config_get_int_value(config, "TIEMPO_DE_REINTENTO_CONEXION");
			config_File->TIEMPO_DE_REINTENTO_OPERACION = config_get_int_value(config, "TIEMPO_DE_REINTENTO_OPERACION");
			config_File->PUNTO_MONTAJE_TALLGRASS = config_get_string_value(config, "PUNTO_MONTAJE_TALLGRASS");
			config_File->IP_BROKER = config_get_string_value(config,"IP_BROKER");
			config_File->PUERTO_BROKER = config_get_int_value(config,"PUERTO_BROKER");
			config_destroy(config);
		}
}



void iniciar_log(){
	char *archivoLog = string_duplicate("GameCard.log");
	logger = log_create(LOG_PATH, archivoLog, false, LOG_LEVEL_INFO);
	free(archivoLog);
	archivoLog = NULL;
}

