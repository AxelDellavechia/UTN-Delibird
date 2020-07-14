/*
 * Broker.h
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#ifndef BROKER_H_
#define BROKER_H_

#include "generales.h"

typedef struct {
	int id_msj;
	int tamano;
	_Bool libre;
	int colaAsignada;
	int idColaAsignada;
	void* punteroInicial;
	void* punteroFinal;
	int tiempoLRU;
}Particion;

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

/*
typedef struct{
	uint32_t posicion_x;
	uint32_t posicion_y;
}posicion;
*/

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
}tamano_CAUGHT_POKEMON;

void* memoria_cache;
pthread_mutex_t mutex_memoria_cache;
pthread_mutex_t mutex_id_msj;
pthread_mutex_t mutex_contador_msjs_cola;
pthread_mutex_t mutex_lista_particiones;
pthread_mutex_t mutex_suscriptores_new_pokemon;
pthread_mutex_t mutex_suscriptores_localized_pokemon;
pthread_mutex_t mutex_suscriptores_get_pokemon;
pthread_mutex_t mutex_suscriptores_appeared_pokemon;
pthread_mutex_t mutex_suscriptores_catch_pokemon;
pthread_mutex_t mutex_contador_msjs_en_cola;
pthread_mutex_t mutex_cola_new_pokemon;
pthread_mutex_t mutex_cola_localized_pokemon;
pthread_mutex_t mutex_cola_get_pokemon;
pthread_mutex_t mutex_cola_appeared_pokemon;
pthread_mutex_t mutex_cola_catch_pokemon;

t_list* lista_msjs;
t_list* lista_particiones;
t_list* suscriptores_new_pokemon;
t_list* suscriptores_localized_pokemon;
t_list* suscriptores_get_pokemon;
t_list* suscriptores_appeared_pokemon;
t_list* suscriptores_catch_pokemon;
t_list* cola_new_pokemon;
t_list* cola_localized_pokemon;
t_list* cola_get_pokemon;
t_list* cola_appeared_pokemon;
t_list* cola_catch_pokemon;

typedef enum {
	First_Fit,
	Best_Fit,
	FIFO,
	LRU
} Algoritmos;

int cantidad_fallidas;
int32_t id_msj;
int contador_msjs_en_cola;

void iniciar_servicio_broker();
void esperar_conexion(int servidor);
void atender(int socket);

void guardar_msj(int head, void * msj);
void reservar_particion(int tamano, Mensaje msj);
Particion * reservar_particion_dinamica(int tamano, Mensaje msj);
void reservar_particion_bs(int tamano, Mensaje msj);

void compactacion();
void eliminar_particion();
void dummyDump();

Particion * algoritmo_primer_ajuste(int tamano);
Particion * algoritmo_mejor_ajuste(int tamano);

int dumpMemoria(int senial);

#endif /* BROKER_H_ */
