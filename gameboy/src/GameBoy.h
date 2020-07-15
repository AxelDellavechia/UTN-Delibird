/*
 * GameBoy.h
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#ifndef GAMEBOY_H_
#define GAMEBOY_H_

#include "Generales.h"
#include <time.h>


int gameCard( char * comando,int argc, char *argv[]);
int team( char * comando,int argc, char *argv[]);
int broker( char * comando,int argc, char *argv[]);
int prc_suscriptor( char * comando,int argc, char *argv[]);


int prc_get_pokemon( char * comando,int argc, char *argv[] , char * modulo , char * ipServer , int puertoServer , char * Hand , char * HandEsperado );
int prc_caught_pokemon( char * comando,int argc, char *argv[] , char * modulo , char * ipServer , int puertoServer , char * Hand , char * HandEsperado );
int prc_catch_pokemon( char * comando,int argc, char *argv[] , char * modulo , char * ipServer , int puertoServer , char * Hand , char * HandEsperado );
int prc_appeared_pokemon( char * comando,int argc, char *argv[] , char * modulo , char * ipServer , int puertoServer , char * Hand , char * HandEsperado );
int prc_new_pokemon( char * comando,int argc, char *argv[] , char * modulo , char * ipServer , int puertoServer , char * Hand , char * HandEsperado );
int prc_localized_pokemon( char * comando,int argc, char *argv[] , char * modulo , char * ipServer , int puertoServer , char * Hand , char * HandEsperado );

void comandoNoR();


#endif /* GAMEBOY_H_ */



