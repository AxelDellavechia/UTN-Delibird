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

		comando = strdup( argv[1] ) ;

		//char * comando = strdup( "TEAM" ) ;

		/*
			Logs obligatorios

			Conexión a cualquier proceso.
			Suscripción a una cola de mensajes.
			Llegada de un nuevo mensaje afdReceptor una cola de mensajes.
			Envío de un mensaje a un suscriptor específico.
		 */

		log_info(logger, "Verificando los comandos ingresados");

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

			//comando = strdup(argv[2]);

			comando = strdup("LOCALIZED_POKEMON");

			//./gameboy TEAM ACK ESTADO ID_MSJ

			if (strcasecmp("ACK",comando) == 0 ) {

				respuesta_ACK * ack =  malloc( sizeof(respuesta_ACK) );

				ack->ack = atoi(argv[3]);
				ack->id_msj = atoi(argv[4]);

				int enviado = conectar_y_enviar("TEAM", configGB->ipTeam , configGB->puertoTeam,"BROKER" , "TEAM" ,APPEARED_POKEMON, ack , logger , loggerCatedra);


			}

			//./gameboy TEAM APPEARED_POKEMON [POKEMON] [POSX] [POSY]



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

			//./gameboy TEAM LOCALIZED_POKEMON Pikachu 3 "1,2,3,4,5,6" 45

			else if (strcasecmp("LOCALIZED_POKEMON",comando) == 0 ) {


				cola_LOCALIZED_POKEMON * loc_poke = malloc( sizeof(cola_LOCALIZED_POKEMON));


						loc_poke->lista_posiciones = list_create(); // 16

						loc_poke->id_mensaje = atoi(argv[6])  ; //4
						loc_poke->cantidad = atoi(argv[4]) ; // 4
						loc_poke->nombre_pokemon = strdup(argv[3]);//  "raichu" 6

						char * posiciones = strdup(argv[5]);

						char ** listapokemon = string_split(posiciones,",");

						//char ** listapokemon = string_split("1,2,3,4",",");

						/*
						loc_poke->id_mensaje = 22  ; //4
						loc_poke->cantidad = 2 ; // 4
						loc_poke->nombre_pokemon = strdup("Pikachu");; //  "raichu" 6
						*/
						loc_poke->tamanio_nombre = string_length(loc_poke->nombre_pokemon); // 4
						/*
						posicion * laPosicion1 = malloc(sizeof(posicion));
						posicion * laPosicion2 = malloc(sizeof(posicion));

						laPosicion1->posicion_x = 1 ;
						laPosicion1->posicion_y = 2 ;
						laPosicion2->posicion_x = 3 ;
						laPosicion2->posicion_y = 4 ;

						list_add(loc_poke->lista_posiciones,laPosicion1);
						list_add(loc_poke->lista_posiciones,laPosicion2);
						*/

						int unaposicion = 0 ;
						while (listapokemon[unaposicion] != NULL){
							posicion * laPosicion = malloc( sizeof(posicion));
							laPosicion->posicion_x = atoi(listapokemon[unaposicion]);
							unaposicion++;
							laPosicion->posicion_y = atoi(listapokemon[unaposicion]);
							list_add(loc_poke->lista_posiciones,laPosicion);
							unaposicion++;
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
			return EXIT_FAILURE;
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

				tiempoSuscripcion = 0 ;

				tiempoSuscripcion = atoi(argv[3]);

				log_info(logger,"Ingresaron el comando -> %s durante %d segundos ",comando,tiempoSuscripcion);

				suscriptor * laSuscripcion = (suscriptor * ) malloc(sizeof(suscriptor));

				laSuscripcion->modulo = GAMEBOY ;
				laSuscripcion->token = token();
				laSuscripcion->cola_a_suscribir = list_create();
				list_add(laSuscripcion->cola_a_suscribir, (int) devolverTipoMsj(comando));

				//crearHilos(laSuscripcion);

			    time_t endwait;

			    endwait = time (NULL) + tiempoSuscripcion ;

			    int fdCliente ; int head ; int bufferTam ;

			    conectarCon(fdCliente, configGB->ipBroker, configGB->puertoBroker, logger);

			    handshake_cliente(fdCliente,"Team","Broker",logger);

			    aplicar_protocolo_enviar(fdCliente,SUSCRIPCION,laSuscripcion);

			    while (time (NULL) < endwait)
			    {

				    recibirProtocolo(head, bufferTam,fdCliente);

				    void * mensaje = malloc(bufferTam);

				    recibirMensaje(fdCliente,bufferTam,mensaje);

				    printf("Se mostrarán en los Logs todos los msjd de la cola %s durante %d segundos",comando,tiempoSuscripcion);

			    	switch( head ){

			    								setlocale(LC_ALL,"");

			    											case NEW_POKEMON :{
			    												cola_NEW_POKEMON  new_poke ;
			    												deserealizar_NEW_POKEMON ( head, mensaje, bufferTam, & new_poke);
			    												log_info(logger,"Recibí en la cola NEW_POKEMON . POKEMON: %s  , CANTIDAD: %d  , CORDENADA X: %d , CORDENADA Y: %d ",new_poke.nombre_pokemon,new_poke.cantidad,new_poke.posicion_x,new_poke.posicion_y);
			    												//reenviarMsjCola_NEW_POKEMON(mensaje);
			    												break;
			    											}
			    											case CATCH_POKEMON :{
			    												cola_CATCH_POKEMON cath_poke;
			    												deserealizar_CATCH_POKEMON( head, mensaje, bufferTam, & cath_poke);
			    												log_info(logger,"Recibí en la cola CATCH_POKEMON . POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",cath_poke.nombre_pokemon,cath_poke.posicion_x,cath_poke.posicion_y);
			    												break;
			    											}
			    											case GET_POKEMON :{
			    												cola_GET_POKEMON get_poke ;
			    												deserealizar_GET_POKEMON ( head, mensaje, bufferTam, & get_poke);
			    												log_info(logger,"Recibí en la cola GET_POKEMON . POKEMON: %s",get_poke.nombre_pokemon);
			    												break;
			    											}

			    											case APPEARED_POKEMON :{
			    												cola_APPEARED_POKEMON app_poke;
			    												deserealizar_APPEARED_POKEMON ( head, mensaje, bufferTam, & app_poke);

			    												//responder por localized_pokemon
			    												log_info(logger,"Recibí en la cola APPEARED_POKEMON . POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",app_poke.nombre_pokemon,app_poke.posicion_x,app_poke.posicion_y);
			    												free(app_poke.nombre_pokemon);
			    												break;
			    											}

			    											case CAUGHT_POKEMON :{
			    												cola_CAUGHT_POKEMON caug_poke ;

			    												//responde por caught_pokemon
			    												deserealizar_CAUGHT_POKEMON ( head, mensaje, bufferTam, & caug_poke);
			    												log_info(logger,"Recibí en la cola CAUGHT_POKEMON . MENSAJE ID: %d  , ATRAPO: %d",caug_poke.id_mensaje,caug_poke.atrapo_pokemon);
			    												break;
			    											}

			    											case LOCALIZED_POKEMON :{
			    												cola_LOCALIZED_POKEMON loc_poke ;
			    												deserealizar_LOCALIZED_POKEMON ( head, mensaje, bufferTam, & loc_poke);
			    												for (int i = 0 ; i < list_size(loc_poke.lista_posiciones); i++){
			    												log_info(logger,"Recibí en la cola LOCALIZED_POKEMON . POKEMON: %s  , CANTIDAD: %d , POSICIÓN X: %d , POSICIÓN Y: %d",loc_poke.nombre_pokemon,loc_poke.cantidad,list_get(loc_poke.lista_posiciones,i),list_get(loc_poke.lista_posiciones,i + 1));
			    												i++;
			    												}
			    												free(loc_poke.nombre_pokemon);
			    												list_destroy(loc_poke.lista_posiciones);
			    												break;
			    											}
			    											case ACK :{
			    												respuesta_ACK ack;
			    												deserealizar_ACK( head, mensaje, bufferTam, & ack);
			    												log_info(logger,"Recibí un ACK con los siguientes datos ESTADO: %d ID_MSJ: %d ",ack.ack,ack.id_msj);
			    												break;
			    											}

			    											case SUSCRIPCION :{
			    												suscriptor laSus;
			    												deserealizar_suscriptor( head, mensaje, bufferTam, & laSus);
			    												for ( int i = 0 ; i < list_size(laSus.cola_a_suscribir) ; i++){
			    													log_info(logger,"Recibí del modulo %s una suscribición a la cola %s con el token %d", devolverModulo(laSus.modulo),tipoMsjIntoToChar(list_get(laSus.cola_a_suscribir,i)),laSus.token);
			    												}
			    												break;
			    											}
			    											default:
			    												log_info(logger, "Instrucción no reconocida");
			    												break;
			    										}

			    }
		}
}


