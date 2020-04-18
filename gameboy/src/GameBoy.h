/*
 * GameBoy.h
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#ifndef GAMEBOY_H_
#define GAMEBOY_H_

#include "Generales.h"

char * pokemon ;
int pokemon_posx;
int pokemon_posy;
int pokemon_cantidad;

int flujoGameCard( char * comando,int argc, char *argv[]);
int flujoTeam( char * comando,int argc, char *argv[]);
int flujoBroker( char * comando,int argc, char *argv[]);
int flujoSuscriptor( char * comando,int argc, char *argv[]);


#endif /* GAMEBOY_H_ */



