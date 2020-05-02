/*
 * GameCard.h
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#ifndef GAMECARD_H_
#define GAMECARD_H_
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
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

typedef struct{
	uint32_t largo_nombre;
	char* nombre_pokemon;
	uint32_t posicion_x;
	uint32_t posicion_y;
}CATCH_POKEMON;

typedef struct{
	uint32_t atrapo_pokemon;
}CAUGHT_POKEMON;



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
int cantidad_fallidas;


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



#endif /* GAMECARD_H_ */
