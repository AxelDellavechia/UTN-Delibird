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

#include <commons/log.h>
#include <commons/config.h>

#define FALSE 0
#define TRUE 1
#define ERROR -1

#define LOG_PATH "../CatedraGB.log"
#define LOG_PATH_INTERNO "../broker.log"

#define RUTA_CONFIG_MEM "../configBroker.txt"

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


#endif /* SRC_GENERALES_H_ */
