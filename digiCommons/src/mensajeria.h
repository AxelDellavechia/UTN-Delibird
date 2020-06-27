/*
 * mensajeria.h
 *
 *  Created on: 22 abr. 2020
 *      Author: utnso
 */

#ifndef SRC_MENSAJERIA_H_
#define SRC_MENSAJERIA_H_
#include <commons/collections/list.h>
#include <stdint.h>

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

typedef struct{
	char * mi_ip;
	int tam_mi_ip;
	int mi_puerto;
	int tam_mac_adress;
	char * mac_adress;
	t_list * cola_a_suscribir; // USA EL ENUM PROTOCOLO DEFINIDO EN PROTOCOLOS COMUNICACION H
	int modulo ; // USA EL ENUM MODULOS DEFINIDO EN PROTOCOLOS COMUNICACION H
	char tipo_suscripcion; // VALOR POSIBLES -> G (GOBLAL) ó L (LOCAL POR ID MSJ)
} suscriptor;

typedef struct{
	int ack;
	int id_msj;
}ACK;


#endif /* SRC_MENSAJERIA_H_ */
