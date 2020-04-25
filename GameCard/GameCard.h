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



#endif /* GAMECARD_H_ */
