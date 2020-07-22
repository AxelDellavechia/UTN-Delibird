/*
 * Broker.h
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#ifndef BROKER_H_
#define BROKER_H_

#define LRU_MAX 4294967295

#include "generales.h"

typedef struct {
	uint32_t id_msj;
	int tamano;
	_Bool libre;
	int colaAsignada;
	int punteroInicial;
	int punteroFinal;
	unsigned long long tiempoLRU;
}Particion;

typedef enum {
	First_Fit,
	Best_Fit,
	FIFO,
	LRU
} Algoritmos;

typedef struct {
	_Bool esPadre;
	//Particion * nodo_padre;
	uint32_t id_msj;
	int tamano;
	_Bool libre;
	int colaAsignada;
	int punteroInicial;
	int punteroFinal;
	unsigned long long tiempoLRU;
}Particion_bs;


void iniciar_servicio_broker();
void esperar_conexion(int servidor);
void atender(int socket);

void guardar_msj(int head, int tamano, void * msj);
void buscar_victima(int head,int tamano, Algoritmos Algoritmo, void * msj);
void buscar_victima_bs(int head, int tamano, Algoritmos Algoritmo, void * msj);

void compactacion();
void eliminar_particion();
void dummyDump();
void imprimirCache(Particion * laParti);

_Bool algoritmo_primer_ajuste_bs(int head, int tamano, void * msj);
void consolidar_bs(Particion_bs * particion_liberada);
_Bool  algoritmo_mejor_ajuste_bs(int head, int tamano, void * msj);

_Bool algoritmo_primer_ajuste(int head, int tamano, void *msj);
_Bool algoritmo_mejor_ajuste(int head, int tamano, void *msj);
void algoritmo_fifo();
void algoritmo_lru();
void consolidar();//Particion *particion_victima);

int dumpMemoria(int senial);

#endif /* BROKER_H_ */
