#include "Generales.h"


void inicializar_semaforos(){

	pthread_mutex_init(&semaforo, NULL);

}

void crearLogger( char * nombre , char * otroLog ) {

	char * archivoLog = strdup(nombre);
	char * archivoLogCatedra = strdup(otroLog);

	logger = log_create(LOG_PATH_INTERNO, archivoLog, FALSE, LOG_LEVEL_INFO);
	loggerCatedra = log_create(LOG_PATH, archivoLogCatedra, FALSE, LOG_LEVEL_INFO);

	free(archivoLog);
	archivoLog = NULL;

	free(archivoLogCatedra);
	archivoLogCatedra = NULL;
}

void leerArchivoDeConfiguracion(char *ruta,t_log * logger) {

	t_config *config;
	config = config_create(ruta);
	if (config != NULL) {
		log_info(logger, "Leyendo Archivo de Configuracion..");

		if (config_has_property(config, "IP_BROKER")) {
			configGB->ipBroker = strdup(
											config_get_string_value(config, "IP_BROKER"));
									log_info(logger,
											"Se encontró y cargó el contenido de IP_BROKER a la estructura. Valor: %s",configGB->ipBroker);

		} else {
									log_error(logger,
											"El archivo de configuracion no contiene el IP_BROKER");
		}

		if (config_has_property(config, "PUERTO_BROKER")) {
			configGB->puertoBroker = config_get_int_value(config,
								"PUERTO_BROKER");
						log_info(logger,
								"Se encontró y cargó el contido del PUERTO_BROKER. Valor: %d",configGB->puertoBroker);
					} else {

						log_error(logger,
								"El archivo de configuracion no contiene el PUERTO_BROKER");

		}

		if (config_has_property(config, "IP_TEAM")) {
					configGB->ipTeam = strdup(
													config_get_string_value(config, "IP_TEAM"));
											log_info(logger,
													"Se encontró y cargó el contenido de IP_BROKER a la estructura. Valor: %s",configGB->ipTeam);

				} else {
											log_error(logger,
													"El archivo de configuracion no contiene el IP_TEAM");
				}

				if (config_has_property(config, "PUERTO_TEAM")) {
					configGB->puertoTeam = config_get_int_value(config,
										"PUERTO_TEAM");
								log_info(logger,
										"Se encontró y cargó el contido del PUERTO_TEAM. Valor: %d",configGB->puertoTeam);
							} else {

								log_error(logger,
										"El archivo de configuracion no contiene el PUERTO_TEAM");

		}

		if (config_has_property(config, "IP_GAMECARD")) {
			configGB->ipGameCard = strdup(
							config_get_string_value(config, "IP_GAMECARD"));
													log_info(logger,
															"Se encontró y cargó el contenido de IP_GAMECARD a la estructura. Valor: %s",configGB->ipGameCard);

						} else {
													log_error(logger,
															"El archivo de configuracion no contiene el IP_GAMECARD");
						}

						if (config_has_property(config, "PUERTO_GAMECARD")) {
							configGB->puertoGameCard = config_get_int_value(config,
												"PUERTO_GAMECARD");
										log_info(logger,
												"Se encontró y cargó el contido del PUERTO_GAMECARD. Valor: %d",configGB->puertoGameCard);
									} else {

										log_error(logger,
												"El archivo de configuracion no contiene el PUERTO_GAMECARD");

		}





	}
	free(config);
}

void* reservarMemoria(int size) {

		void *puntero = malloc(size);
		if(puntero == NULL) {
			fprintf(stderr, "Error al reservar %d bytes de memoria", size);
			exit(ERROR);
		}
		return puntero;
}

void conectaryLoguear(int modulo , int fdServer , char * ipServer , int portServer,t_log* logger,t_log * loggerCatedra) {

	int conexion = conectarCon(fdServer,ipServer,portServer,logger) ;

	char * comando ;

	switch(modulo){
				case TEAM :
					comando = strdup("TEAM") ;
					break;
				case BROKER:
					comando = strdup("BROKER") ;
					break;
				case GAMECARD :
					comando = strdup("GAMECARD") ;
					break;
	}

	if (conexion == 1 ) {
			log_info(loggerCatedra,"Me conecte correctamente con el %s IP: %s y Puerto: %d",comando,ipServer,portServer);
			return
			free(comando);
		} else {
			log_info(loggerCatedra,"No se pudo realizar correctamente la conexión con el %s IP: %s y Puerto: %d",comando,ipServer,portServer);
			free(comando);
	}

}

