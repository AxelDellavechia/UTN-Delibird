/*
 * generales.c
 *
 *  Created on: 29 abr. 2020
 *      Author: utnso
 */
#include "generales.h"

/*
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
*/

void crearHilosBroker() {


	hilo_servidor= 0;
	hilo_consola= 0;

	pthread_create(&hilo_servidor, NULL, (void*) servidor, NULL);
	pthread_create(&hilo_consola, NULL, (void*) consola, NULL);

	pthread_join(hilo_servidor, NULL);
	pthread_join(hilo_consola, NULL);

}

void consola() {

	printf("Hola! Ingresá \"salir\" para finalizar módulo\n");
	size_t buffer_size = 100; //por el momento restringido a 100 caracteres
	char* comando = (char *) calloc(1, buffer_size);

	while (!string_equals_ignore_case(comando, "salir\n")) {
		printf(">");
		int bytes_read = getline(&comando, &buffer_size, stdin);
		if (bytes_read == -1) {
			log_error(logger,"Error en getline");
		}
		if (bytes_read == 1) {
			continue;
		}
	}

	log_destroy(logger);
	free(comando);

	pthread_detach(hilo_servidor);
	pthread_detach( pthread_self() );
	pthread_cancel(hilo_servidor);
}

void inicializar_semaforos(){
	//inicializo semaforos de nodos
	pthread_mutex_init(&semaforo, NULL);
}

void servidor() {

	fdBroker = nuevoSocket();
	asociarSocket(fdBroker, config_File->PUERTO_BROKER);
	escucharSocket(fdBroker, CONEXIONES_PERMITIDAS);

	fd_set setAux;
	int maxFD,i,socket_nuevo;

	FD_ZERO(&setMaestro); 	// borra los conjuntos maestro y temporal
	FD_ZERO(&setAux);

	maxFD = fdBroker; //Llevo control del FD maximo de los sockets
	FD_SET(fdBroker, &setMaestro); //agrego el FD del socketEscucha al setMaestro


	log_info(logger," Escuchando conexiones. Socket: %d",fdBroker);

	while(TRUE) {
		pthread_mutex_lock(&semaforo);
		setAux = setMaestro;
		pthread_mutex_unlock(&semaforo);

				if (select((maxFD + 1), &setAux, NULL, NULL, NULL ) == -1) {
					printf("Error en la escucha.\n");
					log_error(logger,"Error en la escucha.\n" );
					return;
				}

		int conexionNueva = 0;
		int comandoNuevo;//= reservarMemoria(INT);

		for (i = 0; i <= maxFD; i++) {
					pthread_mutex_lock(&semaforo);
					int check = FD_ISSET(i,&setAux);
					pthread_mutex_unlock(&semaforo);
					if (check) { // Me fijo en el set de descriptores a ver cual respondió
						if (i == fdBroker) { //Tengo un nuevo hilo de Sac Cli queriendose conectar
							//Esta funcion acepta una nueva conexion de Sac Cli
							//y agrega un nuevo nodo a la lista de scripts con el nuevo socket

							log_info(logger,"Nuevo Cliente Conectado");
							comandoNuevo = aceptarConexionSocket(fdBroker);
							if (comandoNuevo == -1){
								return;
							}

							conexionNueva = handshake_servidor(comandoNuevo, "Broker" , "Team", logger);

							if( validar_conexion(conexionNueva, 0,logger) == FALSE ) {
									pthread_mutex_lock(&semaforo); //desbloquea el semaforo
									FD_CLR(i, &setMaestro); // borra el file descriptor del set
									pthread_mutex_unlock(&semaforo);
									cerrarSocket(i);

									continue; // vuelve al inicio del while
							}else{
									pthread_mutex_lock(&semaforo);
									FD_SET(comandoNuevo, &setMaestro); //agrego el nuevo socket al setMaestro
									pthread_mutex_unlock(&semaforo);
									if (comandoNuevo > maxFD) maxFD = comandoNuevo;
						}
						}else { // Hay actividad nueva en algún hilo de kernel
							//SI RECIBO TRUE EN CONEXIONNUEVA, ABRO UN HILO POR CADA UNO QUE SE CONECTE
							pthread_t hilo_SacCli;

							pthread_create(&hilo_SacCli, NULL, (void*) thread_Broker, i);
							//pthread_join(hilo_SacCli, NULL);
						}
					}

		}
		//free(comandoNuevo);

	}
}

int thread_Broker(int fdCliente) {

	int head = 0;
	int result = 0;
	//int iterar = 1;

//	while(iterar > 0){
		void* mensaje = aplicar_protocolo_recibir(fdCliente, &head); // recibo mensajes
		if (mensaje != NULL)
		{
			log_info(logger, "head: %i\n",head);


			switch(head){

						case NEW_POKEMON :
							log_info(logger, "NEW_POKEMON");
						break;
						case APPEARED_POKEMON :
							log_info(logger, "APPEARED_POKEMON");
						break;
						case CATCH_POKEMON :
							log_info(logger, "CATCH_POKEMON");
						break;
						case CAUGHT_POKEMON :
							log_info(logger, "CAUGHT_POKEMON");
						break;
						case GET_POKEMON :
							log_info(logger, "GET_POKEMON");
						break;
						case LOCALIZED_POKEMON :
							log_info(logger, "LOCALIZED_POKEMON");
						break;
						default:
							log_error(logger, "Instrucción no reconocida");
							break;
					}
			free(mensaje);

			//pthread_detach( pthread_self() );
			//return TRUE;
				//return FALSE;
		}else
		{
			//FD_CLR(fdCliente, &setMaestro);
			//free(mensaje);
//			iterar = -1;
		}
//	}
	//FD_CLR(fdCliente, &setMaestro);
	pthread_detach( pthread_self() );
	return FALSE;

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
	config_destroy(config);
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
