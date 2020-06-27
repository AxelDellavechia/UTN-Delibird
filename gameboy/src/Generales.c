#include "Generales.h"


void inicializar_semaforos(){

	pthread_mutex_init(&semaforo, NULL);
	pthread_mutex_init(&semaforo2, NULL);

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
	config_destroy(config);
}

void* reservarMemoria(int size) {

		void *puntero = malloc(size);
		if(puntero == NULL) {
			fprintf(stderr, "Error al reservar %d bytes de memoria", size);
			exit(ERROR);
		}
		return puntero;
}

int conectaryLoguear(char * modulo , int fdServer , char * ipServer , int portServer,t_log* logger,t_log * loggerCatedra) {

	conexion = conectarCon(fdServer,ipServer,portServer,logger) ;

		if (conexion == 1 ) {
					log_info(loggerCatedra,"Me conecte correctamente con el %s IP: %s y Puerto: %d",modulo,ipServer,portServer);
					return 1 ;
				} else {
					log_info(loggerCatedra,"No se pudo realizar correctamente la conexión con el %s IP: %s y Puerto: %d",modulo,ipServer,portServer);
					return 0;
	}

}

void crearHilos(suscriptor * laSuscripcion) {

	hilo_servidor = 0;
	hilo_consola = 0 ;

	pthread_create(&hilo_servidor, NULL, (void*) servidor, NULL);
	pthread_create(&hilo_consola, NULL, (void*) consola, (void *) laSuscripcion);

	pthread_mutex_lock(&semaforo);

	pthread_join(hilo_servidor, NULL);
	pthread_join(hilo_consola, NULL);

}

void servidor() {

	pthread_mutex_lock(&semaforo);

	for ( cantidadSegundos ; cantidadSegundos < segundosMaximos ; cantidadSegundos ++) {
		sleep(1);
		printf("paso 1 segundo metodo servidor\n");
	}

	pthread_mutex_unlock(&semaforo);
	pthread_detach(hilo_servidor);
	pthread_cancel(hilo_servidor);
}

void consola(suscriptor * laSuscripcion) {

	int enviado = conectar_y_enviar("GAMEBOY",configGB->ipBroker,configGB->puertoBroker,"TEAM","BROKER",SUSCRIPCION,laSuscripcion,logger, loggerCatedra);

	if (enviado != ERROR) log_info(loggerCatedra,"Me estoy suscribiendo a la cola -> %s durante %d segundos ",comando,tiempoSuscripcion);

	pthread_mutex_unlock(&semaforo);

	while ( tiempoSuscripcion != 0 ) {
			log_info(logger,"El tiempo restante es %d",tiempoSuscripcion);
			tiempoSuscripcion = tiempoSuscripcion - cantidadSegundos ;
	}

	pthread_detach(hilo_consola);
	pthread_cancel(hilo_consola);

}
