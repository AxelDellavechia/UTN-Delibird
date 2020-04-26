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
	leer_configFile();

	//Iniciar Estructuras
	iniciar_estructuras();

	//Multi-Hilos por conexion
	iniciar_servicio_broker();

	printf("Hola Mundo");
	return EXIT_SUCCESS;
}

void leer_configFile(){
	t_config *config;
		config = config_create("/BROKER.config");
		if (config != NULL) {
			config_File->TAMANO_MEMORIA = config_get_int_value(config, "TAMANO_MEMORIA");
			config_File->TAMANO_MINIMO_PARTICION = config_get_int_value(config, "TAMANO_MINIMO_PARTICION");
			config_File->ALGORITMO_MEMORIA = config_get_string_value(config, "ALGORITMO_MEMORIA");
			config_File->ALGORITMO_REEMPLAZO = config_get_string_value(config,"ALGORITMO_REEMPLAZO");
			config_File->ALGORITMO_PARTICION_LIBRE = config_get_string_value(config,"ALGORITMO_PARTICION_LIBRE");
			config_File->ALGORITMO_REEMPLAZO = config_get_string_value(config,"ALGORITMO_REEMPLAZO");
			config_File->IP_BROKER = config_get_string_value(config,"IP_BROKER");
			config_File->PUERTO_BROKER = config_get_int_value(config,"PUERTO_BROKER");
			config_File->FRECUENCIA_COMPACTACION = config_get_int_value(config,"FRECUENCIA_COMPACTACION");
			config_File->LOG_FILE = config_get_string_value(config,"LOG_FILE");
			config_destroy(config);
		}
}

void iniciar_log(){
	char *archivoLog = string_duplicate("Broker.log");
	logger = log_create(config_File->LOG_FILE, archivoLog, false, LOG_LEVEL_INFO);
	free(archivoLog);
	archivoLog = NULL;
}

void iniciar_servicio_broker(){
	int socket = nuevoSocket();
	asociarSocket(socket, config_File->PUERTO_BROKER);
	while(true){
		esperar_conexion(socket);
	}
}

void esperar_conexion(int servidor){
	int socket = aceptarConexionSocket(servidor);
	pthread_t cliente;
	pthread_create(&cliente, NULL,(void*)atender,(void*)socket);
	pthread_detach(cliente);
}
void atender(int socket){
	void operacion;
	while(recv(socket,&operacion,4,MSG_WAITALL) >0){
		switch(operacion){
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

	//SE DEFINE MUTEX PARA DUMP DE MEMORIA CACHE
	pthread_mutex_init(mutex_memoria_cache, NULL);

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
