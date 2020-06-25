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

		crearLogger("GameBoy.log","GameBoyCatedra.log");

		configGB = reservarMemoria(sizeof(archivoConfigGB));

		if (argc < 2 || argc > 7 ){
			printf("No se ingreso la cantidad de parametros necesarios\n");
			log_info(logger,"No se ingreso la cantidad de parametros necesarios");
			return EXIT_FAILURE ;
		}

		inicializar_semaforos();

		log_info(logger, "Por setear los valores del archivo de configuracion");

		leerArchivoDeConfiguracion(RUTA_CONFIG_MEM,logger);

		char * comando = strdup( argv[1] ) ;

		//char * comando = strdup( "MEME" ) ;

		/*
			Logs obligatorios

			Conexión a cualquier proceso.
			Suscripción a una cola de mensajes.
			Llegada de un nuevo mensaje afdReceptor una cola de mensajes.
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

				free(comando);

				comando = strdup(argv[2]);

				if ( argc < 3 ){
					printf("No se ingreso la cantidad de parametros necesarios\n");
					free(comando);
					return EXIT_FAILURE;
				}
				//./gameboy GAMECARD GET_POKEMON [POKEMON]

				if (strcasecmp("GET_POKEMON",comando) == 0 ) {



					cola_GET_POKEMON * get_poke = (cola_GET_POKEMON * ) malloc(sizeof(cola_GET_POKEMON));

					get_poke->id_mensaje = 0 ;

					get_poke->nombre_pokemon = strdup(argv[3]);

					get_poke->tamanio_nombre = string_length(get_poke->nombre_pokemon ) ;

					int enviado = conectar_y_enviar("GAMECARD", configGB->ipGameCard , configGB->puertoGameCard,"Broker" , "GAMECARD" ,GET_POKEMON, get_poke , logger , loggerCatedra);

					if (enviado != -1) log_info(loggerCatedra,"Le envio a la cola GET_POKEMON -> POKEMON: %s ",get_poke->nombre_pokemon);

					free(comando);
					free(get_poke);
					return EXIT_SUCCESS;
				}

				//./gameboy GAMECARD CATCH_POKEMON [POKEMON] [POSX] [POSY] ID_MSJ

				if (strcasecmp("CATCH_POKEMON",comando) == 0 ) {

					if ( argc < 6 ){
						printf("No se ingreso la cantidad de parametros necesarios\n");
						free(comando);
						return EXIT_FAILURE;
					}

					cola_CATCH_POKEMON * cat_poke = (cola_CATCH_POKEMON * ) malloc(sizeof(cola_CATCH_POKEMON));

					cat_poke->id_mensaje = atoi(argv[6]) ;
					cat_poke->nombre_pokemon = strdup(argv[3]);
					cat_poke->tamanio_nombre = string_length(cat_poke->nombre_pokemon ) ;
					cat_poke->posicion_x = atoi(argv[4]) ;
					cat_poke->posicion_y = atoi(argv[5]) ;

					int enviado = conectar_y_enviar("GAMECARD", configGB->ipGameCard , configGB->puertoGameCard,"Broker" , "GAMECARD" ,CATCH_POKEMON, cat_poke , logger , loggerCatedra);

					if (enviado != ERROR) log_info(loggerCatedra,"Le envio a la cola CATCH_POKEMON -> POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",cat_poke->nombre_pokemon,cat_poke->posicion_x,cat_poke->posicion_y);

					free(comando);
					free(cat_poke);
					return EXIT_SUCCESS;
				}

				//./gameboy GAMECARD NEW_POKEMON [POKEMON] [POSX] [POSY] [CANTIDAD] ID_MSJ

				if (strcasecmp("NEW_POKEMON",comando) == 0 ) {

					if ( argc < 7 ){
						printf("No se ingreso la cantidad de parametros necesarios\n");
						free(comando);
						return EXIT_FAILURE;
					}

					cola_NEW_POKEMON * new_poke = (cola_NEW_POKEMON * ) malloc(sizeof(cola_NEW_POKEMON));

					new_poke->id_mensaje = 0 ;
					new_poke->nombre_pokemon = strdup(argv[3]);
					new_poke->tamanio_nombre = string_length(new_poke->nombre_pokemon ) ;
					new_poke->posicion_x = atoi(argv[4]) ;
					new_poke->posicion_y = atoi(argv[5]) ;
					new_poke->cantidad = atoi(argv[6]) ;

					int enviado = conectar_y_enviar("GAMECARD", configGB->ipGameCard , configGB->puertoGameCard,"Broker" , "GameCard" ,NEW_POKEMON, new_poke , logger , loggerCatedra);

					if (enviado != ERROR) log_info(loggerCatedra,"Le envio a la cola NEW_POKEMON -> POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d , CANTIDAD: %d ",new_poke->nombre_pokemon,new_poke->posicion_x,new_poke->posicion_y,new_poke->cantidad);

					free(comando);
					free(new_poke);
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

			log_info(logger,"Trabajando con el TEAM");

			free(comando);

			//./gameboy TEAM APPEARED_POKEMON [POKEMON] [POSX] [POSY]

			comando = strdup(argv[2]);

			//comando = strdup("APPEARED_POKEMON");

			if (strcasecmp("APPEARED_POKEMON",comando) == 0 ) {

				 	if ( argc < 5 || argc >6){
						printf("No se ingreso la cantidad de parametros necesarios\n");
						free(comando);
						return EXIT_FAILURE;
					}

				cola_APPEARED_POKEMON * app_poke =  malloc( sizeof(cola_APPEARED_POKEMON) );

				app_poke->id_mensaje = 0 ;

				app_poke->nombre_pokemon = strdup(argv[3]);
				app_poke->tamanio_nombre = string_length(app_poke->nombre_pokemon);
				app_poke->posicion_x = atoi(argv[4]) ;
				app_poke->posicion_y = atoi(argv[5]) ;

				/*
				app_poke->nombre_pokemon = strdup("pikachu");
				app_poke->tamanio_nombre = string_length(app_poke->nombre_pokemon);
				app_poke->posicion_x = 2 ;
				app_poke->posicion_y = 10 ;
				*/


				int enviado = conectar_y_enviar("TEAM", configGB->ipTeam , configGB->puertoTeam,"BROKER" , "TEAM" ,APPEARED_POKEMON, app_poke , logger , loggerCatedra);

				if (enviado != ERROR) log_info(loggerCatedra,"Le envio a la cola APPEARED_POKEMON -> POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",app_poke->nombre_pokemon,app_poke->posicion_x,app_poke->posicion_y);

				free(comando);
				free(app_poke->nombre_pokemon);
				free(app_poke);
				return EXIT_SUCCESS;
			}

			//./gameboy TEAM CAUGHT_POKEMON [ID_MENSAJE] [OK/FAIL]

			else if (strcasecmp("CAUGHT_POKEMON",comando) == 0 ) {

				if ( argc < 4 ){
					printf("No se ingreso la cantidad de parametros necesarios\n");
					free(comando);
					return EXIT_FAILURE;
				}

				cola_CAUGHT_POKEMON * cau_poke = (cola_CAUGHT_POKEMON * ) malloc(sizeof(cola_CAUGHT_POKEMON));

				cau_poke->id_mensaje = atoi(argv[3]);

				if ( strcasecmp(argv[4],"OK") == 0 ) cau_poke->atrapo_pokemon = TRUE ;
				else cau_poke->atrapo_pokemon = ERROR ;


				int enviado = conectar_y_enviar("TEAM", configGB->ipTeam , configGB->puertoTeam,"BROKER" , "TEAM" ,CAUGHT_POKEMON, cau_poke , logger , loggerCatedra);

				if (enviado != ERROR) log_info(loggerCatedra,"Le envio a la cola CAUGHT_POKEMON -> ID_MENSAJE: %d , ESTADO: %d",cau_poke->id_mensaje,cau_poke->atrapo_pokemon);

				free(comando);

				return EXIT_SUCCESS;
			}
			//./gameboy TEAM LOCALIZED_POKEMON POKEMON CANTIDAD POSICIONES [ID_MENSAJE]

			else if (strcasecmp("LOCALIZED_POKEMON",comando) == 0 ) {


				cola_LOCALIZED_POKEMON * loc_poke = malloc( sizeof(cola_LOCALIZED_POKEMON));

						loc_poke->lista_posiciones = list_create(); // 16

						loc_poke->id_mensaje = atoi(argv[6])  ; //4
						loc_poke->cantidad = atoi(argv[4]) ; // 4
						loc_poke->nombre_pokemon = strdup(argv[3]);; //  "raichu" 6
						loc_poke->tamanio_nombre = string_length(loc_poke->nombre_pokemon); // 4

						char ** listapokemon = string_split(argv[5],",");

						int posicion = 0 ;
						while (listapokemon[posicion] != NULL){
							if ( string_starts_with(listapokemon[posicion],"[")) listapokemon[posicion] = string_substring_from(listapokemon[posicion],1);
							if ( string_ends_with(listapokemon[posicion],"]")) {
								listapokemon[posicion]= string_reverse(listapokemon[posicion]);
								listapokemon[posicion] = string_substring_from(listapokemon[posicion],1);
								listapokemon[posicion]= string_reverse(listapokemon[posicion]);
							}
							list_add(loc_poke->lista_posiciones,atoi(listapokemon[posicion]));
							posicion++;

						}

						log_info(logger,"estoy enviando un LOCALIZED_POKEMON con tamaño %d",calcularTamanioMensaje(LOCALIZED_POKEMON,loc_poke));

						int enviado = conectar_y_enviar("TEAM", configGB->ipTeam , configGB->puertoTeam,"BROKER" , "TEAM" ,LOCALIZED_POKEMON, loc_poke , logger , loggerCatedra);

						if (enviado != ERROR ) {
							for ( int i = 0 ; i < list_size(loc_poke->lista_posiciones); i ++){
							log_info(loggerCatedra,"Le envio a la cola LOCALIZED_POKEMON -> POKEMON: %s  , CORDENADAX: %d , CORDENADA Y: %d ",loc_poke->nombre_pokemon,loc_poke->cantidad,list_get(loc_poke->lista_posiciones,i),list_get(loc_poke->lista_posiciones,i + 1));
							i++;
							}
						}
			}
			else {
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
				free(comando);

			comando = strdup(argv[2]);

						if ( argc < 4 ){
							printf("No se ingreso la cantidad de parametros necesarios\n");
							free(comando);
							return EXIT_FAILURE;
						}


						//./gameboy BROKER GET_POKEMON [POKEMON]

						if (strcasecmp("GET_POKEMON",comando) == 0 ) {

							cola_GET_POKEMON * get_poke = (cola_GET_POKEMON * ) malloc(sizeof(cola_GET_POKEMON));

							get_poke->id_mensaje = 0 ;

							get_poke->nombre_pokemon = strdup(argv[3]);

							get_poke->tamanio_nombre = string_length(get_poke->nombre_pokemon ) ;

							int enviado = conectar_y_enviar("BROKER", configGB->ipBroker , configGB->puertoBroker,"Team" , "Broker" ,GET_POKEMON, get_poke , logger , loggerCatedra);

							if (enviado != ERROR) log_info(loggerCatedra,"Le envio a la cola GET_POKEMON -> POKEMON: %s ",get_poke->nombre_pokemon);

							free(comando);

							free(get_poke);

							return EXIT_SUCCESS;
						}

						//./gameboy BROKER CAUGHT_POKEMON [ID_MENSAJE] [OK/FAIL]

						if (strcasecmp("CAUGHT_POKEMON",comando) == 0 ) {

							if ( argc < 5 ){
								printf("No se ingreso la cantidad de parametros necesarios\n");
								free(comando);
								return EXIT_FAILURE;
							}

							cola_CAUGHT_POKEMON * cau_poke = (cola_CAUGHT_POKEMON * ) malloc(sizeof(cola_CAUGHT_POKEMON));

							cau_poke->id_mensaje = atoi(argv[3]);

							if ( strcasecmp(argv[4],"OK") == 0 ) cau_poke->atrapo_pokemon = TRUE ;
							else cau_poke->atrapo_pokemon = ERROR ;


							int enviado = conectar_y_enviar("BROKER", configGB->ipBroker , configGB->puertoBroker,"Team" , "Broker" ,CAUGHT_POKEMON, cau_poke , logger , loggerCatedra);

							if (enviado != ERROR) log_info(loggerCatedra,"Le envio a la cola CAUGHT_POKEMON -> ID_MENSAJE: %d , ESTADO: %d",cau_poke->id_mensaje,cau_poke->atrapo_pokemon);

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

							cola_CATCH_POKEMON * cat_poke = (cola_CATCH_POKEMON * ) malloc(sizeof(cola_CATCH_POKEMON));

							cat_poke->id_mensaje = 0 ;
							cat_poke->nombre_pokemon = strdup(argv[3]);
							cat_poke->tamanio_nombre = string_length(cat_poke->nombre_pokemon ) ;
							cat_poke->posicion_x = atoi(argv[4]) ;
							cat_poke->posicion_y = atoi(argv[5]) ;

							int enviado = conectar_y_enviar("BROKER", configGB->ipBroker , configGB->puertoBroker,"Team" , "Broker" ,CATCH_POKEMON, cat_poke , logger , loggerCatedra);

							if (enviado != ERROR) log_info(loggerCatedra,"Le envio a la cola CATCH_POKEMON -> POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",cat_poke->nombre_pokemon,cat_poke->posicion_x,cat_poke->posicion_y);

							free(comando);
							free(cat_poke);
							return EXIT_SUCCESS;
						}

						//./gameboy BROKER APPEARED_POKEMON [POKEMON] [POSX] [POSY] [ID_MENSAJE]

						if (strcasecmp("APPEARED_POKEMON",comando) == 0 ) {

							if ( argc < 7 ){
									printf("No se ingreso la cantidad de parametros necesarios\n");
									free(comando);
									return EXIT_FAILURE;
							}

							cola_APPEARED_POKEMON * app_poke = (cola_APPEARED_POKEMON * ) malloc(sizeof(cola_APPEARED_POKEMON));

							app_poke->id_mensaje = atoi(argv[6]) ;
							app_poke->nombre_pokemon = strdup(argv[3]);
							app_poke->tamanio_nombre = string_length(app_poke->nombre_pokemon ) ;
							app_poke->posicion_x = atoi(argv[4]) ;
							app_poke->posicion_y = atoi(argv[5]) ;

							int enviado = conectar_y_enviar("BROKER", configGB->ipBroker , configGB->puertoBroker,"Team" , "Broker" ,APPEARED_POKEMON, app_poke , logger , loggerCatedra);

							if (enviado != ERROR) log_info(loggerCatedra,"Le envio a la cola APPEARED_POKEMON -> POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",app_poke->nombre_pokemon,app_poke->posicion_x,app_poke->posicion_y);

							free(comando);
							free(app_poke);
							return EXIT_SUCCESS;
						}

						//./gameboy BROKER NEW_POKEMON [POKEMON] [POSX] [POSY] [CANTIDAD]

						if (strcasecmp("NEW_POKEMON",comando) == 0 ) {

							if ( argc < 7 ){
									printf("No se ingreso la cantidad de parametros necesarios\n");
									free(comando);
									return EXIT_FAILURE;
							}

							cola_NEW_POKEMON * new_poke = (cola_NEW_POKEMON * ) malloc(sizeof(cola_NEW_POKEMON));

							new_poke->id_mensaje = 0 ;
							new_poke->nombre_pokemon = strdup(argv[3]);
							new_poke->tamanio_nombre = string_length(new_poke->nombre_pokemon ) ;
							new_poke->posicion_x = atoi(argv[4]) ;
							new_poke->posicion_y = atoi(argv[5]) ;
							new_poke->cantidad = atoi(argv[6]) ;

							int enviado = conectar_y_enviar("BROKER", configGB->ipBroker , configGB->puertoBroker,"Team" , "Broker" ,NEW_POKEMON, new_poke , logger , loggerCatedra);

							if (enviado != ERROR) log_info(loggerCatedra,"Le envio a la cola NEW_POKEMON -> POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d , CANTIDAD: %d ",new_poke->nombre_pokemon,new_poke->posicion_x,new_poke->posicion_y,new_poke->cantidad);

							free(comando);
							free(new_poke);
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

				suscriptor * laSuscripcion ;

				laSuscripcion->modulo = GAMEBOY ;
				laSuscripcion->token = token();
				laSuscripcion->cola_a_suscribir = list_create();
				list_add(laSuscripcion->cola_a_suscribir,devolverTipoMsj(comando));

				int tiempoSuscripcion = atoi(argv[3]);

				int enviado = conectar_y_enviar("GAMEBOY",configGB->ipBroker,configGB->puertoBroker,"TEAM","BROKER",SUSCRIPCION,laSuscripcion,logger, loggerCatedra);

				if (enviado != ERROR) log_info(loggerCatedra,"Me estoy suscribiendo a la cola -> %s durante %d segundos ",comando,tiempoSuscripcion);

				log_info(logger,"Aqui loguearia los mensajes de los x segundos");

				//aplicar_protocolo_recibir(fdBroker)

				free(comando);
				return EXIT_SUCCESS;
		}
}


