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
	int tamano;
	_Bool libre;
	int colaAsignada;
	int idColaAsignada;
	void* punteroInicial;
	void* punteroFinal;
	int tiempoLRU;
}Particion;

void iniciar_servicio_broker();
void esperar_conexion(int servidor);
void atender(int socket);

void reservar_particion(int tamano, Mensaje msj);
void reservar_particion_dinamica(int tamano, Mensaje msj);
void reservar_particion_bs(int tamano, Mensaje msj);

void compactacion();
void eliminar_particion();
void dummyDump();

Particion * algoritmo_primer_ajuste(int tamano);
Particion * algoritmo_mejor_ajuste(int tamano);

int dumpMemoria(int senial);

#endif /* BROKER_H_ */
