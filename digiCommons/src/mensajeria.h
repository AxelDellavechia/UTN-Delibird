/*
 * mensajeria.h
 *
 *  Created on: 22 abr. 2020
 *      Author: utnso
 */

#ifndef SRC_MENSAJERIA_H_
#define SRC_MENSAJERIA_H_
#include <commons/collections/list.h>

typedef struct{
	uint32_t id_mensaje;
	char* nombre_pokemon;
	uint32_t  posicion_x;
	uint32_t posicion_y;
	uint32_t cantidad;
}cola_NEW_POKEMON;

typedef struct{
	uint32_t id_mensaje;
	char* nombre_pokemon;
	uint32_t cantidad;
	t_list* lista_posiciones;
}cola_LOCALIZED_POKEMON;

typedef struct{
	uint32_t posicion_x;
	uint32_t posicion_y;
}posicion;

typedef struct{
	uint32_t id_mensaje;
	char* nombre_pokemon;
}cola_GET_POKEMON;

typedef struct{
	uint32_t id_mensaje;
	char* nombre_pokemon;
	uint32_t posicion_x;
	uint32_t posicion_y;
}cola_APPEARED_POKEMON;

typedef struct{
	uint32_t id_mensaje;
	char* nombre_pokemon;
	uint32_t posicion_x;
	uint32_t posicion_y;
}cola_CATCH_POKEMON;

typedef struct{
	uint32_t id_mensaje;
	uint32_t atrapo_pokemon;
}cola_CAUGHT_POKEMON;


#endif /* SRC_MENSAJERIA_H_ */
