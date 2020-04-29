/*
 * Broker.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#include "Broker.h"


int main(){//int argc, char **argv) {

	//Inicio el Log
	iniciar_log();

	//Leo el Archivo de Configuracion

	config_File = reservarMemoria(sizeof(config_File));

	leerArchivoDeConfiguracion(RUTA_CONFIG_MEM,logger);

	//Iniciar Estructuras
	iniciar_estructuras();

	//Multi-Hilos por conexion
	iniciar_servicio_broker();

	dumpMemoria ();

	return EXIT_SUCCESS;
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

void iniciar_servicio_broker(){
	int socket = nuevoSocket();
	asociarSocket(socket, config_File->PUERTO_BROKER);
	while(true){
		//esperar_conexion(socket);
	}
}

/*
void esperar_conexion(int servidor){

	int socket = aceptarConexionSocket(servidor);

	pthread_t cliente;

	pthread_create(&cliente, NULL, (void *) atender , (void *) socket ) ;

	pthread_detach(cliente);
}
*/

void atender(int socket){
	void * operacion = NULL ;
	while(recv(socket,&operacion,4,MSG_WAITALL) >0){
		switch( (int) operacion){
		case NEW_POKEMON:;
			log_info(logger,"-NEW_POKEMON");
			aplicar_protocolo_recibir(socket, 1);
			break;
		case APPEARED_POKEMON:;
			log_info(logger,"-APPEARED_POKEMON");
			aplicar_protocolo_recibir(socket, 3);
			break;
		case CATCH_POKEMON:;
			log_info(logger,"-APPEARED_POKEMON");
			aplicar_protocolo_recibir(socket, 4);
			break;
		case CAUGHT_POKEMON:;
			log_info(logger,"-CAUGHT_POKEMON");
			aplicar_protocolo_recibir(socket, 5);
			break;
		case GET_POKEMON:;
			log_info(logger,"-GET_POKEMON");
			aplicar_protocolo_recibir(socket, 6);
			break;
		case LOCALIZED_POKEMON:;
			log_info(logger,"-LOCALIZED_POKEMON");
			aplicar_protocolo_recibir(socket, 2);
			break;
		}
	}
}

void iniciar_estructuras(){
	//Se reserva la Memoria total del Broker
	memoria_cache = malloc(config_File->TAMANO_MEMORIA);
	memset(memoria_cache, '\0', config_File->TAMANO_MEMORIA);
	//Setea cantidad fallidas
	int cantidad_fallidas = config_File->FRECUENCIA_COMPACTACION;

	//SE DEFINE MUTEX PARA DUMP DE MEMORIA CACHE
	pthread_mutex_init(&mutex_memoria_cache, NULL);

}

void reservar_particion(int tamano, Mensaje msj){
	if(tamano > config_File->TAMANO_MEMORIA && tamano < config_File->TAMANO_MINIMO_PARTICION){
		if(strcmp(config_File->ALGORITMO_MEMORIA, PARTICIONES) == 0){
			reservar_particion_dinamica(tamano, msj);
		}
		else if(strcmp(config_File->ALGORITMO_MEMORIA, BUDDY_SYSTEM) == 0){
			reservar_particion_bs(tamano, msj);
		}
		else{/*ERROR CONFIG*/}
	}
	else{}

}

void reservar_particion_dinamica(int tamano, Mensaje mensaje){
	Particion* particion_libre;
	int contador_busquedas_fallidas;
	if(config_File->ALGORITMO_PARTICION_LIBRE == FIRST_FIT){
		while(cantidad_fallidas <= contador_busquedas_fallidas || particion_libre != NULL){
		particion_libre = algoritmo_primer_ajuste(tamano);
		if(particion_libre == NULL){contador_busquedas_fallidas++;}
		}
		if(particion_libre == NULL && cantidad_fallidas == contador_busquedas_fallidas){
			compactacion();
			cantidad_fallidas = 0;
			while(cantidad_fallidas <= contador_busquedas_fallidas || particion_libre != NULL){
				particion_libre = algoritmo_primer_ajuste(tamano);
				if(particion_libre == NULL){contador_busquedas_fallidas++;}
			}
			if(particion_libre == NULL && cantidad_fallidas == contador_busquedas_fallidas){
				eliminar_particion();
			}
		}
	}
	else if(config_File->ALGORITMO_PARTICION_LIBRE == BEST_FIT){
		while(cantidad_fallidas <= contador_busquedas_fallidas){
		particion_libre = algoritmo_mejor_ajuste(tamano);
		if(particion_libre == NULL && cantidad_fallidas ){contador_busquedas_fallidas++;}
		}
		if(particion_libre == NULL && cantidad_fallidas == contador_busquedas_fallidas){
			compactacion();
		}

	}
	free(particion_libre);
}

void reservar_particion_bs(int tamano, Mensaje mensaje){

}

/*
Particion* algoritmo_primer_ajuste(int tamano){
	int cantidad_particiones = list_size(lista_particiones);
	for(int i = 1; i<= cantidad_particiones; i++){
		Particion* particion = list_get(lista_particiones, i);
		if(particion != NULL && particion->tamano >= tamano){return particion;}
	}
	return NULL;
}
*/

Particion* algoritmo_primer_ajuste(int tamano){
	_Bool particion_libre(Particion* particion){return particion->libre && particion->tamano >= tamano;}
	return list_find(lista_particiones, (void*)particion_libre);
}

/*
void algoritmo_mejor_ajuste(int tamano){
	int cantidad_particiones = list_size(lista_particiones);
	t_list* lista_particiones_ordenadas

}
*/

Particion* algoritmo_mejor_ajuste(int tamano){
	_Bool ordenar(Particion* particion1, Particion* particion2){return particion1->tamano < particion2->tamano;}
	t_list* lista_ordenada = lista_particiones;
	list_sort(lista_ordenada, (void*)ordenar);
	_Bool particion_libre(Particion* particion){return particion->libre && particion->tamano >= tamano;}
	return list_find(lista_ordenada, (void*)particion_libre);
	free(lista_ordenada);
}

void algoritmo_fifo(int tamano){}

void algoritmo_lru(int tamano){}

void compactacion(){}

void eliminar_particion(){}

int dumpMemoria () {

	setlocale(LC_ALL,"");

	FILE * dump ;

	time_t raw ;

	time(&raw);

	struct tm * tiempoActual ;

	tiempoActual = localtime(&raw);

	char filename[PATH_MAX];

	strftime(filename,PATH_MAX,"dump_%d%m%Y_%X.txt",tiempoActual);

	dump = fopen(filename,"a");

	if (dump != NULL) {
		fprintf(dump,"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
		fprintf(dump,"Dump: %s \n",ctime(&raw));
		fprintf(dump,"Partición <valor>: <valor>		[X]		Size: <valor>		LRU: <valor>		Cola: <valor>		ID: <ID>\n");
		fprintf(dump,"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
	} else {
		printf("No se puede abrir el archivo\n");
		return -1 ;
	}
	fclose(dump);
	return 0 ;
}
