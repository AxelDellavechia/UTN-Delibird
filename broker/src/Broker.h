/*
 * Broker.h
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#ifndef BROKER_H_
#define BROKER_H_

#include "generales.h"

typedef struct Mensaje{
	int id_msj;
	int tipo_msj;
	t_list* lista_suscriptores;
	t_list* lista_suscriptores_ACK;
} Mensaje;

typedef struct Particion{
	int tamano;
	_Bool libre;
	int colaAsignada;
	int idColaAsignada;
	void* punteroInicial;
	void* punteroFinal;
	int tiempoLRU;
}Particion;

typedef struct{

}Suscriptor;

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
pthread_mutex_t mutex_cola_new_pokemon;
pthread_mutex_t mutex_cola_localized_pokemon;
pthread_mutex_t mutex_cola_get_pokemon;
pthread_mutex_t mutex_cola_appeared_pokemon;
pthread_mutex_t mutex_cola_catch_pokemon;


t_list* lista_particiones;
t_list* cola_new_pokemon;
t_list* cola_localized_pokemon;
t_list* cola_get_pokemon;
t_list* cola_appeared_pokemon;
t_list* cola_catch_pokemon;

int cantidad_fallidas;

void iniciar_servicio_broker();
void esperar_conexion(int servidor);
void atender(int socket);
void iniciar_estructuras();
void reservar_particion(int tamano, void * msj);
void reservar_particion_dinamica(int tamano, void * mensaje);
void reservar_particion_bs(int tamano, void * mensaje);
Particion* algoritmo_primer_ajuste(int tamano);
Particion* algoritmo_mejor_ajuste(int tamano);
void compactacion();
void eliminar_particion();
void dummyDump();


int dumpMemoria(int senial);

#endif /* BROKER_H_ */
