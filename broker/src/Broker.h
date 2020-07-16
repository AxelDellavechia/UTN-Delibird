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
	int punteroInicial;
	int punteroFinal;
	int tiempoLRU;
}Particion;

typedef enum {
	First_Fit,
	Best_Fit,
	FIFO,
	LRU
} Algoritmos;

void iniciar_servicio_broker();
void esperar_conexion(int servidor);
void atender(int socket);

void guardar_msj(int head, int tamano, void * msj);
void buscar_victima(int head,int tamano, Algoritmos First_Fit, void * msj);
//Particion * reservar_particion_dinamica(int tamano, Mensaje msj);
void reservar_particion_bs(int head, int tamano, void * msj);

void compactacion();
void eliminar_particion();
void dummyDump();

_Bool algoritmo_primer_ajuste(int head, int tamano, void *msj);
_Bool algoritmo_mejor_ajuste(int head, int tamano, void *msj);
void algoritmo_fifo();
void algoritmo_lru();

int dumpMemoria(int senial);

#endif /* BROKER_H_ */
