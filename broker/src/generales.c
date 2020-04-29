/*
 * generales.c
 *
 *  Created on: 29 abr. 2020
 *      Author: utnso
 */
#include "generales.h"

void consola() {

	setlocale(LC_ALL,"");

	printf("Hola! este es el modulo BROKER y mi función es administrar las colas de mensajes\n");
	printf("--------------------------------------------------------\n");
	printf ("Para finalizar módulo ingrese \"SALIR\"\n");

	size_t buffer_size = 100;

	char* comando = (char *) calloc(1, buffer_size);

	do{
		printf(">");
		int bytes_read = getline(&comando, &buffer_size, stdin);
		if (bytes_read == -1) {
			free(comando);
			exit(0);
		}
		if (bytes_read == 1) {
			continue;
		}
	}while (strcasecmp(comando, "salir") != 1) ;
}

void* reservarMemoria(int size) {

		void *puntero = malloc(size);
		if(puntero == NULL) {
			fprintf(stderr, "Error al reservar %d bytes de memoria", size);
			exit(ERROR);
		}
		return puntero;
}

void leerArchivoDeConfiguracion(char *ruta,t_log * logger) {

	t_config *config;

	config = config_create(ruta);

	if (config != NULL) {
		log_info(logger, "Leyendo Archivo de Configuracion..");

		if (config_has_property(config, "TAMANO_MEMORIA")) {
			config_File->TAMANO_MEMORIA = config_get_int_value(config,
								"TAMANO_MEMORIA");
						log_info(logger,
								"Se encontró y cargó el contido del TAMANO_MEMORIA. Valor: %d",config_File->TAMANO_MEMORIA);
					} else {

						log_error(logger,
								"El archivo de configuracion no contiene el TAMANO_MEMORIA");

		}

		if (config_has_property(config, "TAMANO_MINIMO_PARTICION")) {
			config_File->TAMANO_MINIMO_PARTICION = config_get_int_value(config,
								"TAMANO_MINIMO_PARTICION");
						log_info(logger,
								"Se encontró y cargó el contido del TAMANO_MINIMO_PARTICION. Valor: %d",config_File->TAMANO_MINIMO_PARTICION);
					} else {

						log_error(logger,
								"El archivo de configuracion no contiene el TAMANO_MINIMO_PARTICION");

		}

		if (config_has_property(config, "ALGORITMO_MEMORIA")) {
					config_File->ALGORITMO_MEMORIA = strdup(
													config_get_string_value(config, "ALGORITMO_MEMORIA"));
											log_info(logger,
													"Se encontró y cargó el contenido de ALGORITMO_MEMORIA a la estructura. Valor: %s",config_File->ALGORITMO_MEMORIA);

				} else {
											log_error(logger,
													"El archivo de configuracion no contiene el ALGORITMO_MEMORIA");
		}

		if (config_has_property(config, "ALGORITMO_REEMPLAZO")) {
					config_File->ALGORITMO_REEMPLAZO = strdup(
													config_get_string_value(config, "ALGORITMO_REEMPLAZO"));
											log_info(logger,
													"Se encontró y cargó el contenido de ALGORITMO_REEMPLAZO a la estructura. Valor: %s",config_File->ALGORITMO_REEMPLAZO);

				} else {
											log_error(logger,
													"El archivo de configuracion no contiene el ALGORITMO_REEMPLAZO");
		}

		if (config_has_property(config, "ALGORITMO_PARTICION_LIBRE")) {
					config_File->ALGORITMO_PARTICION_LIBRE = strdup(
													config_get_string_value(config, "ALGORITMO_PARTICION_LIBRE"));
											log_info(logger,
													"Se encontró y cargó el contenido de ALGORITMO_PARTICION_LIBRE a la estructura. Valor: %s",config_File->ALGORITMO_PARTICION_LIBRE);

				} else {
											log_error(logger,
													"El archivo de configuracion no contiene el ALGORITMO_PARTICION_LIBRE");
		}

		if (config_has_property(config, "IP_BROKER")) {
					config_File->IP_BROKER = strdup(
													config_get_string_value(config, "IP_BROKER"));
											log_info(logger,
													"Se encontró y cargó el contenido de IP_BROKER a la estructura. Valor: %s",config_File->IP_BROKER);

				} else {
											log_error(logger,
													"El archivo de configuracion no contiene el IP_BROKER");
		}



		if (config_has_property(config, "PUERTO_BROKER")) {
			config_File->PUERTO_BROKER = config_get_int_value(config,
										"PUERTO_BROKER");
								log_info(logger,
										"Se encontró y cargó el contido del PUERTO_BROKER. Valor: %d",config_File->PUERTO_BROKER);
							} else {

								log_error(logger,
										"El archivo de configuracion no contiene el PUERTO_BROKER");

		}

		if (config_has_property(config, "FRECUENCIA_COMPACTACION")) {
			config_File->FRECUENCIA_COMPACTACION = config_get_int_value(config,
										"FRECUENCIA_COMPACTACION");
								log_info(logger,
										"Se encontró y cargó el contido del FRECUENCIA_COMPACTACION. Valor: %d",config_File->FRECUENCIA_COMPACTACION);
							} else {

								log_error(logger,
										"El archivo de configuracion no contiene el FRECUENCIA_COMPACTACION");

		}
	}
	free(config);
}

void iniciar_log(){

	char * archivoLog = strdup("broker.log");
	char * archivoLogCatedra = strdup("CatedraGB.log");

	logger = log_create(LOG_PATH_INTERNO, archivoLog, FALSE, LOG_LEVEL_INFO);
	loggerCatedra = log_create(LOG_PATH, archivoLogCatedra, FALSE, LOG_LEVEL_INFO);

	free(archivoLog);
	archivoLog = NULL;

	free(archivoLogCatedra);
	archivoLogCatedra = NULL;
}
