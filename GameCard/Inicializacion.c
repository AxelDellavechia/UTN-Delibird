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
			config_File->PUERTO_GAMECARD = config_get_int_value(config,"PUERTO_GAMECARD");
			config_destroy(config);

			crearPuntoMontaje();
		}


}

void crearPuntoMontaje()
{
		//PuntoMontaje = (struct t_config_PuntosMontaje *) malloc (sizeof(struct PuntoMontaje));

		PuntoMontaje->PUNTOMONTAJE = malloc ( string_length(config_File->PUNTO_MONTAJE_TALLGRASS));
		PuntoMontaje->FILES = malloc ( string_length(config_File->PUNTO_MONTAJE_TALLGRASS) + string_length("Files/") ) ;
		PuntoMontaje->BLOCKS = malloc ( string_length(config_File->PUNTO_MONTAJE_TALLGRASS) + string_length("Blocks/") ) ;
		PuntoMontaje->METADATA = malloc (string_length(config_File->PUNTO_MONTAJE_TALLGRASS) + string_length("Metadata/") ) ;



		strcpy(PuntoMontaje->PUNTOMONTAJE,config_File->PUNTO_MONTAJE_TALLGRASS);
		//printf("puntoMontaje %s\n",configFile->puntoMontaje);

	//creo puntode montaje de Bloques
		strcpy(PuntoMontaje->BLOCKS,PuntoMontaje->PUNTOMONTAJE);
		strcat(PuntoMontaje->BLOCKS,"Blocks/");
		//creo puntode montaje de Metadata
		strcpy(PuntoMontaje->METADATA,PuntoMontaje->PUNTOMONTAJE);
		strcat(PuntoMontaje->METADATA,"Metadata/");

		strcpy(PuntoMontaje->FILES,PuntoMontaje->PUNTOMONTAJE);
		strcat(PuntoMontaje->FILES,"Files/");

}

int leer_metaData_principal(){
	char *direccionArchivoMedata=(char *) malloc(1 + strlen(PuntoMontaje->METADATA));

	string_append(&direccionArchivoMedata,PuntoMontaje->METADATA);
	printf("direccionArchivoMedata: %s\n",direccionArchivoMedata);
	t_config *archivo_MetaData;
	archivo_MetaData=config_create(direccionArchivoMedata);
	config_MetaData->cantidad_bloques=config_get_int_value(archivo_MetaData,"BLOCKS");
    config_MetaData->magic_number=string_duplicate(config_get_string_value(archivo_MetaData,"MAGIC_NUMBER"));
	config_MetaData->tamanio_bloques=config_get_int_value(archivo_MetaData,"BLOCK_SIZE");
	free(direccionArchivoMedata);
	config_destroy(archivo_MetaData);
	return 0;
}

void crearHilos() {


	hilo_servidor= 0;
	hilo_consola= 0;


	pthread_create(&hilo_servidor, NULL, (void*) servidor, NULL);
	pthread_create(&hilo_consola, NULL, (void*) consola, NULL);

	pthread_join(hilo_servidor, NULL);
	pthread_join(hilo_consola, NULL);

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

	comandoIn = nuevoSocket();
	asociarSocket(comandoIn, config_File->PUERTO_GAMECARD);
	escucharSocket(comandoIn, CONEXIONES_PERMITIDAS);

	fd_set setAux;
	int maxFD,i,socket_nuevo;

	FD_ZERO(&setMaestro); 	// borra los conjuntos maestro y temporal
	FD_ZERO(&setAux);

	maxFD = comandoIn; //Llevo control del FD maximo de los sockets
	FD_SET(comandoIn, &setMaestro); //agrego el FD del socketEscucha al setMaestro


	log_info(logger," Escuchando conexiones. Socket: %d\n",comandoIn);

	while(TRUE) {
		//pthread_mutex_lock(&mxSocketsFD);
		setAux = setMaestro;
		//pthread_mutex_unlock(&mxSocketsFD);

				if (select((maxFD + 1), &setAux, NULL, NULL, NULL ) == -1) {
					printf("Error en la escucha.\n");
					log_error(logger,"Error en la escucha.\n" );
					return;
				}

		int conexionNueva = 0;
		int comandoNuevo;//= reservarMemoria(INT);

		for (i = 0; i <= maxFD; i++) {
			//		pthread_mutex_lock(&mxSocketsFD);
					int check = FD_ISSET(i,&setAux);
			//		pthread_mutex_unlock(&mxSocketsFD);
					if (check) { // Me fijo en el set de descriptores a ver cual respondió
						if (i == comandoIn) { //Tengo un nuevo hilo de Sac Cli queriendose conectar
							//Esta funcion acepta una nueva conexion del Broker o del Gameboy
							//y agrega un nuevo nodo a la lista de scripts con el nuevo socket

							log_info(logger,"Nuevo Cliente Conectado");
							comandoNuevo = aceptarConexionSocket(comandoIn);
							if (comandoNuevo == -1){
								return;
							}

							conexionNueva = handshake_servidor(comandoNuevo, KEY_HANDSHAKE);
							if( validar_conexion(conexionNueva, 0) == FALSE ) {
				//					pthread_mutex_lock(&mxSocketsFD); //desbloquea el semaforo
									FD_CLR(i, &setMaestro); // borra el file descriptor del set
				//					pthread_mutex_unlock(&mxSocketsFD);
									cerrarSocket(i);

									continue; // vuelve al inicio del while
							}else{
					//				pthread_mutex_lock(&mxSocketsFD);
									FD_SET(comandoNuevo, &setMaestro); //agrego el nuevo socket al setMaestro
					//				pthread_mutex_unlock(&mxSocketsFD);
									if (comandoNuevo > maxFD) maxFD = comandoNuevo;
						}
						}else { // Hay actividad nueva en algún socket conectado
							//SI RECIBO TRUE EN CONEXIONNUEVA, ABRO UN HILO POR CADA UNO QUE SE CONECTE
						/*	pthread_t hilo_SacCli;
							pthread_mutex_lock(&mxHilos);
							pthread_create(&hilo_SacCli, NULL, (void*) thread_SacCli, i);
							pthread_mutex_unlock(&mxHilos);
*/
						printf("Ingresó un nuevo pedido, debo crear un hilo");
						}
					}

		}
		//free(comandoNuevo);

	}


}

// -- CONEXIONES ENTRE MÓDULOS --
int handshake_servidor(int sockCliente, char *mensaje) {

	enviarPorSocket(sockCliente, mensaje, string_length(mensaje));

	HANDSHAKE_SIZE = sizeof(char) * string_length(KEY_HANDSHAKE);
	char *buff = (char*) malloc(HANDSHAKE_SIZE);
	int status = recibirPorSocket(sockCliente, buff, HANDSHAKE_SIZE);

	if( validar_recive(status, 0) == TRUE ) { // El cliente envió un mensaje

		buff[HANDSHAKE_SIZE-1] = '\0';
		if (validar_cliente(buff)) {
			log_info(logger,"Hice el handshake y me respondieron: %s\n", buff);
			//enviarPorSocket(sockCliente, "SacServer", string_length("SacServer"));
			free(buff);
			return TRUE;
		} else {
			free(buff);
			return FALSE;
		}

	} else { // Hubo algún error o se desconectó el cliente

		free(buff);
		return FALSE;
	}
	return FALSE; // No debería llegar acá pero lo pongo por el warning
}

int validar_cliente(char *id) {
	if( !strcmp(id, KEY_HANDSHAKE)) {
		log_info(logger,"Cliente aceptado");
		return TRUE;
	} else {
		printf("Cliente rechazado\n");
		return FALSE;
	}
}

void crearBloques(void)
{
	int i;
	for (i=0; i< config_MetaData->cantidad_bloques;i++ )
	{
			FILE *block;
			char* bloque = (char*) malloc(string_length(PuntoMontaje->BLOCKS)+ string_length(string_itoa(i))+ string_length(".bin"));
			strcpy(bloque,PuntoMontaje->BLOCKS);
			strcat(bloque,string_itoa(i));
			strcat(bloque,".bin");
			block = fopen(bloque,"a");
			fclose(block);
			free(bloque);
	}
}
