#include "Inicializacion.h"
#include <commons/config.h>
#include "src/sockets.h"


/*void leer_configFile(char* ruta) {


	t_config *config;
	char *ptrRuta = ruta;
	config = config_create(ptrRuta);
		if (config != NULL) {
			config_File->TIEMPO_DE_REINTENTO_CONEXION = config_get_int_value(config, "TIEMPO_DE_REINTENTO_CONEXION");
			config_File->TIEMPO_DE_REINTENTO_OPERACION = config_get_int_value(config, "TIEMPO_DE_REINTENTO_OPERACION");
			config_File->PUNTO_MONTAJE_TALLGRASS = config_get_string_value(config, "PUNTO_MONTAJE_TALLGRASS");
			config_File->IP_BROKER = config_get_string_value(config,"IP_BROKER");
			config_File->PUERTO_BROKER = config_get_int_value(config,"PUERTO_BROKER");
			config_File->PUERTO_GAMECARD = config_get_int_value(config,"PUERTO_GAMECARD");
			config_destroy(config);

			crearPuntoMontaje();
			leer_metaData_principal();
		}


}*/

void* reservarMemoria(int size) {
	void *puntero = malloc(size);
	if (puntero == NULL) {
		fprintf(stderr,
				"Lo imposible sucedió. Error al reservar %d bytes de memoria\n",
				size);
		exit(ERROR);
	}
	return puntero;
}

void leer_configFile(char* ruta) {

	config_File = reservarMemoria(sizeof(ConfigFile));
	t_config *config;
	config = reservarMemoria (sizeof(t_config));
	config = config_create(ruta);
	log_info(logger, "Por setear los valores del archivo de configuracion");
	if (config != NULL) {
		log_info(logger, "FS: Leyendo Archivo de Configuracion..");

		config_File->TIEMPO_DE_REINTENTO_CONEXION = config_get_int_value(config, "TIEMPO_DE_REINTENTO_CONEXION");
		config_File->TIEMPO_DE_REINTENTO_OPERACION = config_get_int_value(config, "TIEMPO_DE_REINTENTO_OPERACION");
		config_File->PUNTO_MONTAJE_TALLGRASS = config_get_string_value(config, "PUNTO_MONTAJE_TALLGRASS");
		config_File->IP_BROKER = config_get_string_value(config,"IP_BROKER");
		config_File->PUERTO_BROKER = config_get_int_value(config,"PUERTO_BROKER");
		config_File->PUERTO_GAMECARD = config_get_int_value(config,"PUERTO_GAMECARD");

		}
	//config_destroy(config);
	crearPuntoMontaje();
	leer_metaData_principal();

}

void crearPuntoMontaje()
{
		PuntoMontaje = malloc (sizeof(t_config_PuntosMontaje));

		PuntoMontaje->PUNTOMONTAJE = malloc (1 + string_length(config_File->PUNTO_MONTAJE_TALLGRASS));
		PuntoMontaje->FILES = malloc (1 + string_length(config_File->PUNTO_MONTAJE_TALLGRASS) + string_length("Files/") ) ;
		PuntoMontaje->BLOCKS = malloc (1 + string_length(config_File->PUNTO_MONTAJE_TALLGRASS) + string_length("Blocks/") ) ;
		PuntoMontaje->METADATA = malloc (1 + string_length(config_File->PUNTO_MONTAJE_TALLGRASS) + string_length("Metadata/") ) ;
		PuntoMontaje->METADATA_FILE = malloc (1 + string_length(config_File->PUNTO_MONTAJE_TALLGRASS) + string_length("Metadata/Metadata.bin") ) ;
		PuntoMontaje->BITMAP = malloc (1 + string_length(config_File->PUNTO_MONTAJE_TALLGRASS) + string_length("Metadata/Bitmap.bin") ) ;



		strcpy(PuntoMontaje->PUNTOMONTAJE,config_File->PUNTO_MONTAJE_TALLGRASS);
		//printf("puntoMontaje %s\n",configFile->puntoMontaje);

	//creo puntode montaje de Bloques
		strcpy(PuntoMontaje->BLOCKS,PuntoMontaje->PUNTOMONTAJE);
		strcat(PuntoMontaje->BLOCKS,"Blocks/");
		//creo puntode montaje de Metadata
		strcpy(PuntoMontaje->METADATA,PuntoMontaje->PUNTOMONTAJE);
		strcat(PuntoMontaje->METADATA,"Metadata/");

		strcpy(PuntoMontaje->METADATA_FILE,PuntoMontaje->PUNTOMONTAJE);
		strcat(PuntoMontaje->METADATA_FILE,"Metadata/Metadata.bin");

		strcpy(PuntoMontaje->BITMAP,PuntoMontaje->PUNTOMONTAJE);
		strcat(PuntoMontaje->BITMAP,"Metadata/Bitmap.bin");

		strcpy(PuntoMontaje->FILES,PuntoMontaje->PUNTOMONTAJE);
		strcat(PuntoMontaje->FILES,"Files/");

}

int leer_metaData_principal(){
/*	char *direccionArchivoMedata=(char *) malloc(strlen(PuntoMontaje->METADATA));

	string_append(&direccionArchivoMedata,PuntoMontaje->METADATA);
	printf("direccionArchivoMedata: %s\n",direccionArchivoMedata);*/

	config_MetaData = reservarMemoria(sizeof(t_config_MetaData));

	t_config *archivo_MetaData;
	archivo_MetaData=config_create(PuntoMontaje->METADATA_FILE);
	config_MetaData->cantidad_bloques=config_get_int_value(archivo_MetaData,"BLOCKS");
    config_MetaData->magic_number=string_duplicate(config_get_string_value(archivo_MetaData,"MAGIC_NUMBER"));
	config_MetaData->tamanio_bloques=config_get_int_value(archivo_MetaData,"BLOCK_SIZE");
	//free(direccionArchivoMedata);
	//config_destroy(archivo_MetaData);
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

	printf("\nHola! Ingresá \"salir\" para finalizar módulo y \"ayuda\" para ver los comandos\n");
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

/*
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
}*/

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

void crearBitmap(){

	//char *direccionArchivoBitMap = path_bitmap();

	int bitmap = open(PuntoMontaje->BITMAP, O_RDWR);


	if(bitmap == ERROR){
		printf("No se pudo abrir el archivo bitmap\n");
	}

	struct stat mystat;

	if (fstat(bitmap, &mystat) < 0) {
	    printf("Error al establecer fstat\n");
	    close(bitmap);
	}
    char *bmap ;


    //Subo a memoria el contenido del archivo bitmap desde 0 hasta mystat.st_size bytes.

    bmap = mmap(NULL, mystat.st_size, PROT_WRITE | PROT_READ, MAP_PRIVATE, bitmap, 0);


	if (bmap == MAP_FAILED) {
			printf("Error al mapear a memoria: %s\n", strerror(errno));

	}

	//bitarray = bitarray_create_with_mode(bmap, config_MetaData.cantidad_bloques / 8, MSB_FIRST);
	bitarray = bitarray_create_with_mode(mmap(NULL, config_MetaData->cantidad_bloques / 8, PROT_WRITE | PROT_READ, MAP_SHARED, bitmap, 0), config_MetaData->cantidad_bloques / 8, MSB_FIRST);

	size_t	cantidadDebits= bitarray_get_max_bit (bitarray);


	close(bitmap);

	printf("cantidadDebits: %i\n",cantidadDebits);
	int i;
	for (i=0;i<cantidadDebits;i++){

		bitarray_clean_bit(bitarray,i);

	}

}

int creacionDeArchivoBitmap(char *path,int cantidad){
    int x = 0;
    FILE *fh = fopen (path, "wb");
    if(fh == NULL){
    	printf("Error en el fopen\n");
    }
    int i;
    for( i=0;i<cantidad;i++){
        if (fh != NULL) {
                fwrite (&x, sizeof (x), 1, fh);
        }
    }
    fclose(fh);
    return 0;

}


int cantidadDeBloquesLibres (void){
	size_t	cantidadDebits= bitarray_get_max_bit (bitarray);
	int libre =ERROR;
	int i;
	for (i=0;i<cantidadDebits;i++){
		if (bitarray_test_bit(bitarray,i)==0){
			libre++;
		}
	}
	return libre;
}

int proximobloqueLibre (void){

	size_t	cantidadDebits= bitarray_get_max_bit (bitarray);
	int i;
	int libre= ERROR;
	//pthread_mutex_lock(&mxBitmap);
	for (i=0;i<cantidadDebits;i++){
		if(bitarray_test_bit(bitarray,i)==0){
			libre=i;
			break;
		}
	}
//	pthread_mutex_unlock(&mxBitmap);
	return libre;
}

void listarArchivos(char *archivo)
{

  FILE *fich;
  long ftam;

  fich=fopen(archivo, "r");
  if (fich)
    {
      fseek(fich, 0L, SEEK_END);
      ftam=ftell(fich);
      fclose(fich);
      /* Si todo va bien, decimos el tamaño */
      printf ("%30s (%ld bytes)\n", archivo, ftam);
    }
  else
    /* Si ha pasado algo, sólo decimos el nombre */
    printf ("%30s (No info.)\n", archivo);
}


