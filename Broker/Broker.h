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
#include <../digiCommons/src/sockets.h>
#include <../digiCommons/src/mensajeria.h>
#include <../digiCommons/src/protocolos_comunicacion.h>
#include <pthread.h>
#include <signal.h>

typedef struct Mensaje{
	int id_msj;
	int tipo_msj;
	t_list* lista_suscriptores;
	t_list* lista_suscriptores_ACK;
} Mensaje;

typedef struct Particion{
	int tamano;
	_Bool libre;
	//void* puntero;
}Particion;

typedef struct{

}Suscriptor;

typedef struct ConfigFile{
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
	uint32_t cantidad;
}tamano_NEW_POKEMON;

typedef struct{
	uint32_t largo_nombre;
	char* nombre_pokemon;
	uint32_t cantidad;
	t_list* lista_posiciones;
}tamano_LOCALIZED_POKEMON;

typedef struct{
	uint32_t posicion_x;
	uint32_t posicion_y;
}posicion;

typedef struct{
	uint32_t largo_nombre;
	char* nombre_pokemon;
}tamano_GET_POKEMON;

typedef struct{
	uint32_t largo_nombre;
	char* nombre_pokemon;
	uint32_t posicion_x;
	uint32_t posicion_y;
}tamano_APPEARED_POKEMON;

typedef struct{
	uint32_t largo_nombre;
	char* nombre_pokemon;
	uint32_t posicion_x;
	uint32_t posicion_y;
}tamano_CATCH_POKEMON;

typedef struct{
	uint32_t atrapo_pokemon;
}tamano_sCAUGHT_POKEMON;

void* memoria_cache;
pthread_mutex_t mutex_memoria_cache;
ConfigFile* config_File;
t_log* logger;
t_list* lista_particiones;
t_list* cola_new_pokemon;
t_list* cola_localized_pokemon;
t_list* cola_get_pokemon;
t_list* cola_appeared_pokemon;
t_list* cola_catch_pokemon;
char FIRST_FIT[2]= "FF";
char BEST_FIT[2] = "BF";
char PARTICIONES[11] = "PARTICIONES";
char BUDDY_SYSTEM[2] = "BS";
int cantidad_fallidas = config_File->FRECUENCIA_COMPACTACION;


void leer_configFile();
void iniciar_log();
void iniciar_servicio_broker();
void esperar_conexion(int servidor);
void atender(int socket);
void iniciar_estructuras();
void reservar_particion(int tamano, Mensaje msj);
void reservar_particion_dinamica(int tamano, Mensaje mensaje);
void reservar_particion_bs(int tamano, Mensaje mensaje);
Particion* algoritmo_primer_ajuste(int tamano);
Particion* algoritmo_mejor_ajuste(int tamano);
void compactacion();
void eliminar_particion();

#endif /* BROKER_H_ */
