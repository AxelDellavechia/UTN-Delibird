/*
 * Broker.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#include "Broker.h"
#include "generales.h"

void dummyDump(){

	int a = 1 ;
	int b = 2;

	void * ptInicial = &a ;
	void * ptFinal = &b ;

		Particion * el_ejemplo;

		el_ejemplo = reservarMemoria(sizeof(Particion));

		el_ejemplo->libre = true ;
		el_ejemplo->tamano = 345 ;
		el_ejemplo->punteroInicial = ptInicial ;
		el_ejemplo->punteroFinal = ptFinal ;

		Particion * el_ejemplo2;

		el_ejemplo2 = reservarMemoria(sizeof(Particion));


		el_ejemplo2->colaAsignada = NEW_POKEMON ;
		el_ejemplo2->idColaAsignada = 1 ;
		el_ejemplo2->libre =false ;
		el_ejemplo2->punteroInicial = ptInicial ;
		el_ejemplo2->punteroFinal = ptFinal ;
		el_ejemplo2->tamano =456 ;
		el_ejemplo2->tiempoLRU = config_File->FRECUENCIA_COMPACTACION ;

	 	lista_particiones = list_create() ;

		list_add(lista_particiones,el_ejemplo);

		list_add(lista_particiones,el_ejemplo2);
}

int main(){//int argc, char **argv) {

	//Inicio el Log
	iniciar_log();

	//Leo el Archivo de Configuracion

	config_File = reservarMemoria(sizeof(ConfigFile));

	leerArchivoDeConfiguracion(CONFIG_BROKER,logger);

	//Iniciar Estructuras
	iniciar_estructuras();

	//Capturar señal SIGUSR1 para el Dump de Memoria
	dummyDump();
	signal(SIGUSR1,dumpMemoria);

	//Multi-Hilos por conexion
	crearHilosBroker();

	//return EXIT_SUCCESS;
}

/*
void iniciar_servicio_broker(){
	int socket = nuevoSocket();
	asociarSocket(socket, config_File->PUERTO_BROKER);
	while(true){
		//esperar_conexion(socket);
	}
}

void esperar_conexion(int servidor){

	int socket = aceptarConexionSocket(servidor);

	pthread_t cliente;

	pthread_create(&cliente, NULL, (void *) atender , (void *) socket ) ;

	pthread_detach(cliente);
}

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
*/

void iniciar_estructuras(){
	//Se reserva la Memoria total del Broker
	memoria_cache = malloc(config_File->TAMANO_MEMORIA);
	memset(memoria_cache, '\0', config_File->TAMANO_MEMORIA);
	//Setea cantidad fallidas
	int cantidad_fallidas = config_File->FRECUENCIA_COMPACTACION;

	//SE DEFINE MUTEX PARA DUMP DE MEMORIA CACHE
	pthread_mutex_init(&mutex_memoria_cache, NULL);
	pthread_mutex_init(&mutex_suscriptores_new_pokemon, NULL);
	pthread_mutex_init(&mutex_suscriptores_localized_pokemon, NULL);
	pthread_mutex_init(&mutex_suscriptores_get_pokemon, NULL);
	pthread_mutex_init(&mutex_suscriptores_appeared_pokemon, NULL);
	pthread_mutex_init(&mutex_suscriptores_catch_pokemon, NULL);
	pthread_mutex_init(&mutex_cola_new_pokemon, NULL);
	pthread_mutex_init(&mutex_cola_localized_pokemon, NULL);
	pthread_mutex_init(&mutex_cola_get_pokemon, NULL);
	pthread_mutex_init(&mutex_cola_appeared_pokemon, NULL);
	pthread_mutex_init(&mutex_cola_catch_pokemon, NULL);

}

void reservar_particion(int tamano, Mensaje msj){
	if(tamano > config_File->TAMANO_MEMORIA && tamano < config_File->TAMANO_MINIMO_PARTICION){
		if(strcmp(config_File->ALGORITMO_MEMORIA, "PARTICIONES") == 0){
			reservar_particion_dinamica(tamano, msj);
		}
		else if(strcmp(config_File->ALGORITMO_MEMORIA, "BS") == 0){
			reservar_particion_bs(tamano, msj);
		}
		else{/*ERROR CONFIG*/}
	}
	else{}

}

void reservar_particion_dinamica(int tamano, Mensaje mensaje){
	Particion* particion_libre;
	int contador_busquedas_fallidas;
	if(config_File->ALGORITMO_PARTICION_LIBRE == "FF"){
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
	else if(config_File->ALGORITMO_PARTICION_LIBRE == "BF"){
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

int dumpMemoria (int senial) {

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
		int i ;
		for ( i = 0 ; i < list_size(lista_particiones) ; i++) {
			Particion * actual = list_get(lista_particiones,i) ;
			if ( actual->libre ) {
				fprintf(dump,"Partición %d: %p - %p		[L]		Size: %db  \n",i,actual->punteroInicial,actual->punteroFinal,actual->tamano);
			} else {
				char * colaNombre ;
				switch (actual->colaAsignada) {
					case NEW_POKEMON :
						colaNombre = strdup("NEW_POKEMON");
					break;
					case APPEARED_POKEMON :
						colaNombre = strdup("APPEARED_POKEMON");
					break;
					case CATCH_POKEMON :
						colaNombre = strdup("CATCH_POKEMON");
					break;
					case CAUGHT_POKEMON :
						colaNombre = strdup("CAUGHT_POKEMON");
					break;
					case GET_POKEMON :
						colaNombre = strdup("GET_POKEMON");
					break;
					case LOCALIZED_POKEMON :
						colaNombre = strdup("LOCALIZED_POKEMON");
					break;
				}
				fprintf(dump,"Partición %d: %p - %p		[X]		Size: %db		LRU: %d		Cola: %s		ID: %d\n",i,actual->punteroInicial,actual->punteroFinal,actual->tamano,actual->tiempoLRU,colaNombre,actual->idColaAsignada);
				free(colaNombre);
			}

			free(actual);
		}
		fprintf(dump,"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
	} else {
		printf("No se puede abrir el archivo\n");
		return -1 ;
	}
	fclose(dump);
	return 0 ;
}
