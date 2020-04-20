/*
 * Broker.h
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#ifndef BROKER_H_
#define BROKER_H_
#define LOG_PATH "./BROKER.log"
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <digiCommons/src/sockets.h>
#include <pthread.h>
#include <signal.h>

typedef struct{
	int id_msj;
	int id_cola_msj;
	t_list* ALGORITMO_MEMORIA;
	char* ALGORITMO_REEMPLAZO;
	char* ALGORITMO_PARTICION_LIBRE;
	char* IP_BROKER;
	int PUERTO_BROKER;
	int FRECUENCIA_COMPACTACION;
	char* LOG_FILE;
} Mensaje;

typedef struct{

}Suscriptor;

typedef struct{
	int TAMANO_MEMORIA;
	int TAMANO_MINIMO_PARTICION;
	char* ALGORITMO_MEMORIA;
	char* ALGORITMO_REEMPLAZO;
	char* ALGORITMO_PARTICION_LIBRE;
	char* IP_BROKER;
	int PUERTO_BROKER;
	int FRECUENCIA_COMPACTACION;
	char* LOG_FILE;
} ConfigFile;

typedef struct{

}particion_memoria;

typedef struct{
	uint32_t largo_nombre;
	char* nombre_pokemon;
	uint32_t  posicion_x;
	uint32_t posicion_y;

}NEW_POKEMON;

typedef struct{
	uint32_t largo_nombre;
	char* nombre_pokemon;
	uint32_t cantidad;
	t_list* lista_posiciones;
}LOCALIZED_POKEMON;

typedef struct{
	uint32_t posicion_x;
	uint32_t posicion_y;
}posicion;

typedef struct{
	uint32_t largo_nombre;
	char* nombre_pokemon;
}GET_POKEMON;

typedef struct{
	uint32_t largo_nombre;
	char* nombre_pokemon;
	uint32_t posicion_x;
	uint32_t posicion_y;
}APPEARED_POKEMON;

void* memoria_cache;
pthread_mutex_t mutex_memoria_cache;
ConfigFile* config_File;
t_log* logger;
t_list* lista_particiones;

#endif /* BROKER_H_ */
