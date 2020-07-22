/*
 * memoria.h
 *
 *  Created on: 16 jul. 2020
 *      Author: utnso
 */

#ifndef SRC_MEMORIA_H_
#define SRC_MEMORIA_H_

#include "generales.h"
#include "Broker.h"
#include "../digiCommons/src/mensajeria.h"
#include "../digiCommons/src/protocolos_comunicacion.h"
#include "../digiCommons/src/generales.h"


void deserealizar_mem_NEW_POKEMON (Particion * laParti , cola_NEW_POKEMON * new_poke );
void deserealizar_mem_APPEARED_POKEMON (Particion * laParti ,  cola_APPEARED_POKEMON * app_poke) ;
void deserealizar_mem_CATCH_POKEMON (Particion * laParti , cola_CATCH_POKEMON* cat_poke);
void deserealizar_mem_CAUGHT_POKEMON (Particion * laParti ,cola_CAUGHT_POKEMON* cau_poke) ;
void deserealizar_mem_GET_POKEMON (Particion * laParti ,cola_GET_POKEMON * get_poke) ;
void deserealizar_mem_LOCALIZED_POKEMON (Particion * laParti , cola_LOCALIZED_POKEMON * loc_poke);


#endif /* SRC_MEMORIA_H_ */
