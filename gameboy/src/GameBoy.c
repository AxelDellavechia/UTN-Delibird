/*
 ============================================================================
 Name        : GameBoy.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "GameBoy.h"


int main (int argc, char *argv[]) {

	crearLogger("CatedraGB.log",LOG_PATH);
	crearLogger("GameBoy.log",LOG_PATH_INTERNO);

	configGB = reservarMemoria(sizeof(archivoConfigGB));

	/*
	printf("argv[0]: %s \n",argv[0]);
	printf("argv[1]: %s \n",argv[1]);
	printf("argv[2]: %s \n",argv[2]);
	printf("argv[3]: %s \n",argv[2]);
	printf("argv[4]: %s \n",argv[2]);
	printf("argv[5]: %s \n",argv[2]);
	printf("argv[6]: %s \n",argv[2]);
	printf("Cantidad de Parametros: %d \n",argc);
	*/

	if (argc < 2 || argc > 6 ){
		printf("No se ingreso la cantidad de parametros necesarios\n");
		log_info(logger,"No se ingreso la cantidad de parametros necesarios");
		return EXIT_FAILURE ;
	}

	fdBroker = nuevoSocket();

	fdTeam = nuevoSocket() ;

	fdGameCard = nuevoSocket() ;

	inicializar_semaforos();

	log_info(logger, "Por setear los valores del archivo de configuracion");

	leerArchivoDeConfiguracion(RUTA_CONFIG_MEM,logger);

	char * comando = strdup( argv[1] ) ;

	if (strcasecmp("BROKER",comando) == 0 ) {

			log_info(logger,"Trabajando con el BROKER");

			if ( argc < 3 ){
				printf("No se ingreso la cantidad de parametros necesarios\n");
				free(comando);
				return EXIT_FAILURE;
			}

			else {

			comando = (char *) realloc(comando,strlen(argv[2]));

			strcpy(comando,argv[2]);

						if ( argc < 4 ){
							printf("No se ingreso la cantidad de parametros necesarios\n");
							free(comando);
							return EXIT_FAILURE;
						}

						//./gameboy BROKER GET_POKEMON [POKEMON]

						if (strcasecmp("GET_POKEMON",comando) == 0 ) {
							printf("Aca me suscribo a las colas amiwo\n");
							free(comando);
							return EXIT_SUCCESS;
						}

						//./gameboy BROKER CAUGHT_POKEMON [ID_MENSAJE] [OK/FAIL]

						if (strcasecmp("CAUGHT_POKEMON",comando) == 0 ) {

							if ( argc < 5 ){
								printf("No se ingreso la cantidad de parametros necesarios\n");
								free(comando);
								return EXIT_FAILURE;
							}

							printf("Aca me suscribo a las colas amiwo\n");
							free(comando);
							return EXIT_SUCCESS;
						}

						//./gameboy BROKER CATCH_POKEMON [POKEMON] [POSX] [POSY]

						if (strcasecmp("CATCH_POKEMON",comando) == 0 ) {

							if ( argc < 6 ){
								printf("3 No se ingreso la cantidad de parametros necesarios\n");
								free(comando);
								return EXIT_FAILURE;
							}

							printf("Aca me suscribo a las colas amiwo\n");
							free(comando);
							return EXIT_SUCCESS;
						}

						//./gameboy BROKER APPEARED_POKEMON [POKEMON] [POSX] [POSY] [ID_MENSAJE]

						if (strcasecmp("APPEARED_POKEMON",comando) == 0 ) {

							if ( argc < 7 ){
									printf("No se ingreso la cantidad de parametros necesarios\n");
									free(comando);
									return EXIT_FAILURE;
							}

							printf("Aca me suscribo a las colas amiwo\n");
							free(comando);
							return EXIT_SUCCESS;
						}

						//./gameboy BROKER NEW_POKEMON [POKEMON] [POSX] [POSY] [CANTIDAD]

						if (strcasecmp("NEW_POKEMON",comando) == 0 ) {

							if ( argc < 7 ){
									printf("No se ingreso la cantidad de parametros necesarios\n");
									free(comando);
									return EXIT_FAILURE;
							}

							printf("Aca me suscribo a las colas amiwo\n");
							free(comando);
							return EXIT_SUCCESS;
						}
						else {
							printf("Comando no reconocido\n");
							free(comando);
							return EXIT_FAILURE;
						}

			}
	}

	if (strcasecmp("GAMECARD",comando) == 0 ) {

			log_info(logger,"Trabajando con el GAMECARD");

			if ( argc < 3 ){
				printf("No se ingreso la cantidad de parametros necesarios\n");
				free(comando);
				return EXIT_FAILURE;
			}

			else {

				comando = (char *) realloc(comando,strlen(argv[2]));

				strcpy(comando,argv[2]);

				if ( argc < 3 ){
					printf("No se ingreso la cantidad de parametros necesarios\n");
					free(comando);
					return EXIT_FAILURE;
				}
				//./gameboy GAMECARD GET_POKEMON [POKEMON]

				if (strcasecmp("GET_POKEMON",comando) == 0 ) {
					printf("Aca me suscribo a las colas amiwo\n");
					free(comando);
					return EXIT_SUCCESS;
				}

				//./gameboy GAMECARD CATCH_POKEMON [POKEMON] [POSX] [POSY]

				if (strcasecmp("CATCH_POKEMON",comando) == 0 ) {

					if ( argc < 6 ){
						printf("No se ingreso la cantidad de parametros necesarios\n");
						free(comando);
						return EXIT_FAILURE;
					}
					printf("Aca me suscribo a las colas amiwo\n");
					free(comando);
					return EXIT_SUCCESS;
				}

				//./gameboy GAMECARD NEW_POKEMON [POKEMON] [POSX] [POSY] [CANTIDAD]

				if (strcasecmp("NEW_POKEMON",comando) == 0 ) {

					if ( argc < 7 ){
						printf("No se ingreso la cantidad de parametros necesarios\n");
						free(comando);
						return EXIT_FAILURE;
					}
					printf("Aca me suscribo a las colas amiwo\n");
					free(comando);
					return EXIT_SUCCESS;
				}

				else {
					printf("Comando no reconocido\n");
					free(comando);
					return EXIT_FAILURE;
				}
		}
	}

	//./gameboy TEAM APPEARED_POKEMON [POKEMON] [POSX] [POSY]

	if (strcasecmp("TEAM",comando) == 0 ) {
			log_info(logger,"Trabajando con el TEAM");
			free(comando);
			comando = strdup(argv[2]);
			if (strcasecmp("APPEARED_POKEMON",comando) == 0 ) {
				if ( argc < 5 || argc >6){
					printf("No se ingreso la cantidad de parametros necesarios\n");
					free(comando);
					return EXIT_FAILURE;
				}
				printf("Aca me suscribo a las colas amiwo\n");
				free(comando);
				return EXIT_SUCCESS;
			} else {
				printf("Comando no reconocido\n");
				free(comando);
				return EXIT_FAILURE;
			}
	}


	//./gameboy SUSCRIPTOR [COLA_DE_MENSAJES] [TIEMPO]

	if (strcasecmp("SUSCRIPTOR",comando ) == 0 ) {
		log_info(logger,"Trabajando con el SUSCRIPTOR");
		if ( argc < 4){
			printf("No se ingreso la cantidad de parametros necesarios\n");
			free(comando);
			return EXIT_FAILURE;
		} else {
				printf("Aca me suscribo a las colas amiwo\n");
				return EXIT_SUCCESS;
		}
	}
	else {
			printf("Modulo no reconocido\n");
			free(comando);
			return EXIT_FAILURE;
	}


	/*
	  	Logs obligatorios

	 	Conexión a cualquier proceso.
		Suscripción a una cola de mensajes.
		Llegada de un nuevo mensaje a una cola de mensajes.
		Envío de un mensaje a un suscriptor específico.
	 */
}
