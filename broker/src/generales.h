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

typedef struct {
	suscriptor laSus;
	int suSocket;
} losSuscriptores;

ConfigFile* config_File;

t_log* logger;
t_log* loggerCatedra;

pthread_mutex_t mxHilos;
pthread_mutex_t mxSocketsFD;

pthread_t hilo_servidor;
pthread_t hilo_consola;
pthread_t hilo_Publisher;

fd_set setMaestro;

int fdBroker; //Socket Base
int comandoNuevo; // Socket de Escucha

int cantidad_fallidas;
int id_msj;
int contador_msjs_en_cola;


void* memoria_cache;

pthread_mutex_t mutex_memoria_cache;
pthread_mutex_t mutex_id_msj;
pthread_mutex_t mutex_contador_msjs_cola;

pthread_mutex_t mutex_suscriptores_new_pokemon;
pthread_mutex_t mutex_suscriptores_localized_pokemon;
pthread_mutex_t mutex_suscriptores_get_pokemon;
pthread_mutex_t mutex_suscriptores_appeared_pokemon;
pthread_mutex_t mutex_suscriptores_catch_pokemon;
pthread_mutex_t mutex_suscriptores_caught_pokemon;

pthread_mutex_t mutex_contador_msjs_en_cola;

pthread_mutex_t mutex_cola_new_pokemon;
pthread_mutex_t mutex_cola_localized_pokemon;
pthread_mutex_t mutex_cola_get_pokemon;
pthread_mutex_t mutex_cola_appeared_pokemon;
pthread_mutex_t mutex_cola_catch_pokemon;
pthread_mutex_t mutex_cola_caught_pokemon;


t_list* lista_msjs;
t_list* lista_particiones;

t_list* suscriptores_new_pokemon;
t_list* suscriptores_localized_pokemon;
t_list* suscriptores_get_pokemon;
t_list* suscriptores_appeared_pokemon;
t_list* suscriptores_catch_pokemon;
t_list* suscriptores_caught_pokemon;

t_list* cola_new_pokemon;
t_list* cola_localized_pokemon;
t_list* cola_get_pokemon;
t_list* cola_appeared_pokemon;
t_list* cola_catch_pokemon;
t_list* cola_caught_pokemon;

void* reservarMemoria(int size);
void leerArchivoDeConfiguracion(char *ruta,t_log * logger);
void iniciar_log();
void iniciar_estructuras();

//FUNCIONES HILOS
void consola();
void servidor();
void crearHilosBroker();
void publisher();
int thread_Broker(int fdCliente);

//FUNCIONES MSJ
void reenviarMsjs_Cola(int head, t_list * lista_Msjs_Cola, t_list * lista_de_suscriptores);
void suscribirse(losSuscriptores * suscp);
void agregar_contador_msj();
void obtener_msj(int id_msj , Mensaje * msj);
int buscarEnLista( t_list * lista , suscriptor * buscado ) ;

#endif /* SRC_GENERALES_H_ */
