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




void consola() {

	printf("Hola! Ingresá \"salir\" para finalizar módulo y \"ayuda\" para ver los comandos\n");
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

	free(comando);
/*
 *
 	bitarray_destroy(bitarray);
  log_destroy(logger);
	pthread_detach(hilo_servidor);
	pthread_detach( pthread_self() );
	pthread_cancel(hilo_servidor);
*/
}


void servidor() {
/*
	comandoIn = nuevoSocket();
	asociarSocket(comandoIn, puertoSacServer);
	escucharSocket(comandoIn, CONEXIONES_PERMITIDAS);

	fd_set setAux;
	int maxFD,i,socket_nuevo;

	FD_ZERO(&setMaestro); 	// borra los conjuntos maestro y temporal
	FD_ZERO(&setAux);

	maxFD = comandoIn; //Llevo control del FD maximo de los sockets
	FD_SET(comandoIn, &setMaestro); //agrego el FD del socketEscucha al setMaestro


	log_info(logger," Escuchando conexiones. Socket: %d\n",comandoIn);

	while(TRUE) {
		pthread_mutex_lock(&mxSocketsFD);
		setAux = setMaestro;
		pthread_mutex_unlock(&mxSocketsFD);

				if (select((maxFD + 1), &setAux, NULL, NULL, NULL ) == -1) {
					printf("Error en la escucha.\n");
					log_error(logger,"Error en la escucha.\n" );
					return;
				}

		int conexionNueva = 0;
		int comandoNuevo;//= reservarMemoria(INT);

		for (i = 0; i <= maxFD; i++) {
					pthread_mutex_lock(&mxSocketsFD);
					int check = FD_ISSET(i,&setAux);
					pthread_mutex_unlock(&mxSocketsFD);
					if (check) { // Me fijo en el set de descriptores a ver cual respondió
						if (i == comandoIn) { //Tengo un nuevo hilo de Sac Cli queriendose conectar
							//Esta funcion acepta una nueva conexion de Sac Cli
							//y agrega un nuevo nodo a la lista de scripts con el nuevo socket

							log_info(logger,"Nuevo Cliente Conectado");
							comandoNuevo = aceptarConexionSocket(comandoIn);
							if (comandoNuevo == -1){
								return;
							}

							conexionNueva = handshake_servidor(comandoNuevo, "SacServer");
							if( validar_conexion(conexionNueva, 0) == FALSE ) {
									pthread_mutex_lock(&mxSocketsFD); //desbloquea el semaforo
									FD_CLR(i, &setMaestro); // borra el file descriptor del set
									pthread_mutex_unlock(&mxSocketsFD);
									cerrarSocket(i);

									continue; // vuelve al inicio del while
							}else{
									pthread_mutex_lock(&mxSocketsFD);
									FD_SET(comandoNuevo, &setMaestro); //agrego el nuevo socket al setMaestro
									pthread_mutex_unlock(&mxSocketsFD);
									if (comandoNuevo > maxFD) maxFD = comandoNuevo;
						}
						}else { // Hay actividad nueva en algún hilo de kernel
							//SI RECIBO TRUE EN CONEXIONNUEVA, ABRO UN HILO POR CADA UNO QUE SE CONECTE
							pthread_t hilo_SacCli;
							pthread_mutex_lock(&mxHilos);
							pthread_create(&hilo_SacCli, NULL, (void*) thread_SacCli, i);
							pthread_mutex_unlock(&mxHilos);
							//pthread_join(hilo_SacCli, NULL);
						}
					}

		}
		//free(comandoNuevo);

	}

*/
}


