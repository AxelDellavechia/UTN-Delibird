/*
<<<<<<< HEAD
=======
 * mensajeria.h
 *
 *  Created on: 22 abr. 2020
 *      Author: utnso
 */

>>>>>>> origin/master
#ifndef SRC_MENSAJERIA_H_
#define SRC_MENSAJERIA_H_
#include <commons/collections/list.h>
#include <stdint.h>

<<<<<<< HEAD
=======




typedef struct{
	uint32_t fdSocket;
	uint32_t head;
} new_msj_broker;


typedef struct{
	uint32_t id_mensaje;
	char* nombre_pokemon;
	uint32_t  tamanio_nombre;
	uint32_t  posicion_x;
	uint32_t posicion_y;
	uint32_t cantidad;
	uint32_t fdSocket;
	uint32_t head;
}GENERIC_POKEMON;


>>>>>>> origin/master
typedef struct{
	uint32_t id_mensaje;
	char* nombre_pokemon;
	uint32_t  tamanio_nombre;
	uint32_t  posicion_x;
	uint32_t posicion_y;
	uint32_t cantidad;
}cola_NEW_POKEMON;

typedef struct{
	uint32_t id_mensaje;
	char* nombre_pokemon;
	uint32_t  tamanio_nombre;
	uint32_t cantidad;
	t_list* lista_posiciones;
}cola_LOCALIZED_POKEMON;

typedef struct{
	uint32_t posicion_x;
	uint32_t posicion_y;
}posicion;

<<<<<<< HEAD
=======

typedef struct{
	int Pos_x;
	int Pos_y;
	int Cantidad;
} t_positions;

>>>>>>> origin/master
typedef struct{
	uint32_t id_mensaje;
	char* nombre_pokemon;
	uint32_t  tamanio_nombre;
}cola_GET_POKEMON;

typedef struct{
	uint32_t id_mensaje;
	char* nombre_pokemon;
	uint32_t  tamanio_nombre;
	uint32_t posicion_x;
	uint32_t posicion_y;
}cola_APPEARED_POKEMON;

typedef struct{
	uint32_t id_mensaje;
	char* nombre_pokemon;
	uint32_t  tamanio_nombre;
	uint32_t posicion_x;
	uint32_t posicion_y;
}cola_CATCH_POKEMON;

typedef struct{
	uint32_t id_mensaje;
	uint32_t atrapo_pokemon;
}cola_CAUGHT_POKEMON;


<<<<<<< HEAD
#endif SRC_MENSAJERIA_H_
*/
=======
#endif /* SRC_MENSAJERIA_H_ */
>>>>>>> origin/master
