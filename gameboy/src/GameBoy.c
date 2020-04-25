/*
 ============================================================================
 Name        : GameBoy.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "GameBoy.h"


int main (int argc, char *argv[]) {


		crearLogger("GameBoy.log","CatedraGB.log");

		configGB = reservarMemoria(sizeof(archivoConfigGB));

		/*
		printf("argv[0]: %s \n",argv[0]);
		printf("argv[1]: %s \n",argv[1]);
		printf("argv[2]: %s \n",argv[2]);
		printf("argv[3]: %s \n",argv[3]);
		printf("argv[4]: %s \n",argv[4]);
		printf("argv[5]: %s \n",argv[5]);
		printf("argv[6]: %s \n",argv[6]);
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

		/*
			Logs obligatorios

			Conexión a cualquier proceso.
			Suscripción a una cola de mensajes.
			Llegada de un nuevo mensaje a una cola de mensajes.
			Envío de un mensaje a un suscriptor específico.
		 */


		if (strcasecmp("BROKER",comando) == 0 ) flujoBroker(  comando, argc , argv) ;

		else if (strcasecmp("GAMECARD",comando) == 0 ) flujoGameCard(  comando, argc , argv) ;

		else if (strcasecmp("TEAM",comando) == 0 ) flujoTeam(  comando, argc , argv) ;

		else if (strcasecmp("SUSCRIPTOR",comando ) == 0 ) flujoSuscriptor( comando, argc , argv) ;

		else {
				printf("Modulo no reconocido\n");
				free(comando);
				return EXIT_FAILURE;
		}


}


int flujoGameCard( char * comando,int argc, char *argv[]) {

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

					pokemon = strdup(argv[3]);

					log_info(loggerCatedra,"Le envio a la cola GET_POKEMON -> POKEMON: %s ",pokemon);

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

					pokemon = strdup(argv[3]);
					pokemon_posx = atoi(argv[4]) ;
					pokemon_posy = atoi(argv[5]) ;

					log_info(loggerCatedra,"Le envio a la cola CATCH_POKEMON -> POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",pokemon,pokemon_posx,pokemon_posy);

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

					pokemon = strdup(argv[3]);
					pokemon_posx = atoi(argv[4]) ;
					pokemon_posy = atoi(argv[5]) ;
					pokemon_cantidad = atoi(argv[6]);

					log_info(loggerCatedra,"Le envio a la cola NEW_POKEMON -> POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d , CANTIDAD: %d ",pokemon,pokemon_posx,pokemon_posy,pokemon_cantidad);

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

int flujoTeam( char * comando,int argc, char *argv[]) {

if ( cxTeam != 0 ) {
			conectaryLoguear(TEAM , cxTeam ,fdTeam,configGB->ipTeam,configGB->puertoTeam,logger, loggerCatedra);

		}
			log_info(logger,"Trabajando con el TEAM");
			free(comando);

			//./gameboy TEAM APPEARED_POKEMON [POKEMON] [POSX] [POSY]

			comando = strdup(argv[2]);

			if (strcasecmp("APPEARED_POKEMON",comando) == 0 ) {
				if ( argc < 5 || argc >6){
					printf("No se ingreso la cantidad de parametros necesarios\n");
					free(comando);
					return EXIT_FAILURE;
				}

				pokemon = strdup(argv[3]);
				pokemon_posx = atoi(argv[4]) ;
				pokemon_posy = atoi(argv[5]) ;

				log_info(loggerCatedra,"Le envio a la cola APPEARED_POKEMON -> POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",pokemon,pokemon_posx,pokemon_posy);
				//aplicar_protocolo_enviar(fdBroker,)
				free(comando);
				return EXIT_SUCCESS;
			} else {
				printf("Comando no reconocido\n");
				free(comando);
				return EXIT_FAILURE;
			}
}

int flujoBroker( char * comando,int argc, char *argv[]){

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
							pokemon = strdup(argv[3]);
							log_info(loggerCatedra,"Le envio a la cola GET_POKEMON -> POKEMON: %s  ",pokemon);
							//aplicar_protocolo_enviar(fdBroker,)
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

							int id_mensaje = atoi(argv[4]);
							char * estado = strdup(argv[5]);

							log_info(loggerCatedra,"Le envio a la cola CAUGHT_POKEMON -> ID_MENSAJE: %d , ESTADO: %d",id_mensaje,estado);
							//aplicar_protocolo_enviar(fdBroker,)
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

							pokemon = strdup(argv[3]);
							pokemon_posx = atoi(argv[4]) ;
							pokemon_posy = atoi(argv[5]) ;

							log_info(loggerCatedra,"Le envio a la cola CATCH_POKEMON -> POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",pokemon,pokemon_posx,pokemon_posy);

							//aplicar_protocolo_enviar(fdBroker,)

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

							pokemon = strdup(argv[3]);
							pokemon_posx = atoi(argv[4]) ;
							pokemon_posy = atoi(argv[5]) ;
							int id_mensaje = atoi(argv[6]);

							log_info(loggerCatedra,"Le envio a la cola APPEARED_POKEMON -> POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d , ID_MENSAJE: %d ",pokemon,pokemon_posx,pokemon_posy,id_mensaje);

							//aplicar_protocolo_enviar(fdBroker,)

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

							pokemon = strdup(argv[3]);
							pokemon_posx = atoi(argv[4]) ;
							pokemon_posy = atoi(argv[5]) ;
							pokemon_cantidad = atoi(argv[6]);

							log_info(loggerCatedra,"Le envio a la cola NEW_POKEMON -> POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d , CANTIDAD: %d ",pokemon,pokemon_posx,pokemon_posy,pokemon_cantidad);

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

int flujoSuscriptor( char * comando,int argc, char *argv[]) {

	//./gameboy SUSCRIPTOR [COLA_DE_MENSAJES] [TIEMPO]

	log_info(logger,"Trabajando con el SUSCRIPTOR");

		if ( argc < 4){
			printf("No se ingreso la cantidad de parametros necesarios\n");
			free(comando);
			return EXIT_FAILURE;
		} else {
				free(comando);
				comando = strdup(argv[2]);
				int tiempoSuscripcion = atoi(argv[3]);
				if ( cxBroker != 0 ) {
					conectaryLoguear(BROKER , cxBroker ,fdBroker,configGB->ipBroker,configGB->puertoBroker,logger, loggerCatedra);

				}

				log_info(loggerCatedra,"Me estoy suscribiendo a la cola -> %s durante %d segundos ",comando,tiempoSuscripcion);
				//aplicar_protocolo_enviar(fdBroker,)
				log_info(loggerCatedra,"Aqui loguearia los mensajes de los x segundos");
				//aplicar_protocolo_recibir(fdBroker)
				free(comando);
				return EXIT_SUCCESS;
		}
}
