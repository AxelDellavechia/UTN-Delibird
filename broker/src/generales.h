/*
 * generales.h
 *
 *  Created on: 29 abr. 2020
 *      Author: utnso
 */

#ifndef SRC_GENERALES_H_
#define SRC_GENERALES_H_

#include <locale.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <pthread.h>
#include <limits.h>

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>

#include <../broker/src/Broker.h>
#include <../digiCommons/src/sockets.h>
#include <../digiCommons/src/mensajeria.h>
#include <../digiCommons/src/protocolos_comunicacion.h>


#define FALSE 0
#define TRUE 1
#define ERROR -1

#define LOG_PATH "../brokerCatedra.log"
#define LOG_PATH_INTERNO "../broker.log"

#define CONFIG_BROKER "../configBroker.txt"

#define CONEXIONES_PERMITIDAS 100

typedef struct ConfigFile{
	int TAMANO_MEMORIA;
	int TAMANO_MINIMO_PARTICION;
	char* ALGORITMO_MEMORIA;
	char* ALGORITMO_REEMPLAZO;
	char* ALGORITMO_PARTICION_LIBRE;
	char* IP_BROKER;
	int PUERTO_BROKER;
	int FRECUENCIA_COMPACTACION;
} ConfigFile;

ConfigFile* config_File;

t_log* logger;
t_log* loggerCatedra;

pthread_mutex_t mxHilos;
pthread_mutex_t mxSocketsFD;

pthread_t hilo_servidor;
pthread_t hilo_consola;
pthread_t hilo_Publisher;

fd_set setMaestro;

int fdBroker;

void* reservarMemoria(int size);
void leerArchivoDeConfiguracion(char *ruta,t_log * logger);
void iniciar_log();
void consola();
void servidor();
void crearHilosBroker();
int thread_Broker(int fdCliente);
void inicializar_semaforos();
void reenviarMsjs_Cola(t_list * lista_Msjs_Cola, t_list * lista_de_suscriptores);
//void reenviarMsjCola_NEW_POKEMON(void * mensaje);
void Suscribirse(suscriptor * suscp);

#endif /* SRC_GENERALES_H_ */
