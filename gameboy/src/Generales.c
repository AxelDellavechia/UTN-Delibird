#include "Generales.h"


void inicializar_semaforos(){

	pthread_mutex_init(&semaforo, NULL);
	pthread_mutex_init(&semaforo2, NULL);
	pthread_mutex_init(&mxHilos, NULL);

}

void crearLogger( char * nombre , char * otroLog ) {

	char * archivoLog = strdup(nombre);
	char * archivoLogCatedra = strdup(otroLog);

	logger = log_create(LOG_PATH_INTERNO, archivoLog, FALSE, LOG_LEVEL_INFO);
	loggerCatedra = log_create(LOG_PATH, archivoLogCatedra, FALSE, LOG_LEVEL_INFO);

	free(archivoLog);
	archivoLog = NULL;

	free(archivoLogCatedra);
	archivoLogCatedra = NULL;
}

void leerArchivoDeConfiguracion(char *ruta,t_log * logger) {

	t_config *config;
	config = config_create(ruta);
	if (config != NULL) {
		log_info(logger, "Leyendo Archivo de Configuracion..");

		if (config_has_property(config, "IP_BROKER")) {
			configGB->ipBroker = strdup(
											config_get_string_value(config, "IP_BROKER"));
									log_info(logger,
											"Se encontró y cargó el contenido de IP_BROKER a la estructura. Valor: %s",configGB->ipBroker);

		} else {
									log_error(logger,
											"El archivo de configuracion no contiene el IP_BROKER");
		}

		if (config_has_property(config, "PUERTO_BROKER")) {
			configGB->puertoBroker = config_get_int_value(config,
								"PUERTO_BROKER");
						log_info(logger,
								"Se encontró y cargó el contido del PUERTO_BROKER. Valor: %d",configGB->puertoBroker);
					} else {

						log_error(logger,
								"El archivo de configuracion no contiene el PUERTO_BROKER");

		}

		if (config_has_property(config, "IP_TEAM")) {
					configGB->ipTeam = strdup(
													config_get_string_value(config, "IP_TEAM"));
											log_info(logger,
													"Se encontró y cargó el contenido de IP_BROKER a la estructura. Valor: %s",configGB->ipTeam);

				} else {
											log_error(logger,
													"El archivo de configuracion no contiene el IP_TEAM");
				}

				if (config_has_property(config, "PUERTO_TEAM")) {
					configGB->puertoTeam = config_get_int_value(config,
										"PUERTO_TEAM");
								log_info(logger,
										"Se encontró y cargó el contido del PUERTO_TEAM. Valor: %d",configGB->puertoTeam);
							} else {

								log_error(logger,
										"El archivo de configuracion no contiene el PUERTO_TEAM");

		}

		if (config_has_property(config, "IP_GAMECARD")) {
			configGB->ipGameCard = strdup(
							config_get_string_value(config, "IP_GAMECARD"));
													log_info(logger,
															"Se encontró y cargó el contenido de IP_GAMECARD a la estructura. Valor: %s",configGB->ipGameCard);

						} else {
													log_error(logger,
															"El archivo de configuracion no contiene el IP_GAMECARD");
						}

						if (config_has_property(config, "PUERTO_GAMECARD")) {
							configGB->puertoGameCard = config_get_int_value(config,
												"PUERTO_GAMECARD");
										log_info(logger,
												"Se encontró y cargó el contido del PUERTO_GAMECARD. Valor: %d",configGB->puertoGameCard);
									} else {

										log_error(logger,
												"El archivo de configuracion no contiene el PUERTO_GAMECARD");

		}





	}
	config_destroy(config);
}

void* reservarMemoria(int size) {

		void *puntero = malloc(size);
		if(puntero == NULL) {
			fprintf(stderr, "Error al reservar %d bytes de memoria", size);
			exit(ERROR);
		}
		return puntero;
}

int conectaryLoguear(char * modulo , int fdServer , char * ipServer , int portServer,t_log* logger,t_log * loggerCatedra) {

	conexion = conectarCon(fdServer,ipServer,portServer,logger) ;

		if (conexion == 1 ) {
					log_info(loggerCatedra,"Me conecte correctamente con el %s IP: %s y Puerto: %d",modulo,ipServer,portServer);
					return 1 ;
				} else {
					log_info(loggerCatedra,"No se pudo realizar correctamente la conexión con el %s IP: %s y Puerto: %d",modulo,ipServer,portServer);
					return 0;
	}

}


void crearHilos() {

	hilo_servidor = 0;
	hilo_consola = 0 ;
	//hilo_finalizar = 0 ;

	pthread_create(&hilo_servidor, NULL, (void*) servidor, NULL);
	pthread_create(&hilo_consola, NULL, (void*) consola, NULL);
	//pthread_create(&hilo_consola, NULL, (void*) finalizar, NULL);

	pthread_mutex_lock(&semaforo);
	//pthread_mutex_lock(&semaforo2);

	pthread_join(hilo_servidor, NULL);
	pthread_join(hilo_consola, NULL);

}
/*
int finalizar() {

	pthread_mutex_lock(&semaforo2);

	pthread_detach(hilo_consola);
	pthread_cancel(hilo_consola);

	free(mensaje);

	free(comando);



	liberarRecursosComunes();

	pthread_mutex_lock(&mxHilos);
	pthread_detach( pthread_self() );
	pthread_mutex_unlock(&mxHilos);

	pthread_mutex_unlock(&semaforo2);
}
*/
void servidor() {

	pthread_mutex_lock(&semaforo);

	time_t start = time (NULL);

	time_t endwait = time (NULL) + tiempoSuscripcion ;

	while (start  < endwait) {
		start = time (NULL);
	}

	//printf("Transcurrio la cantidad %d segundos se finaliza el programa\n",tiempoSuscripcion);


	pthread_mutex_unlock(&semaforo);
	//pthread_mutex_unlock(&semaforo2);

	pthread_mutex_lock(&mxHilos);

	pthread_cancel(hilo_consola);
	pthread_detach(hilo_consola);

	pthread_detach( pthread_self() );

	pthread_mutex_unlock(&mxHilos);
}

void responderACK(int id_msj){
	respuesta_ACK  * ack = malloc (sizeof(respuesta_ACK)) ;
	ack->ack = TRUE ;
	ack->id_msj = id_msj ;
	aplicar_protocolo_enviar(fdCliente,ACK,ack);
	free(ack);
}

void consola() {

fdCliente = nuevoSocket() ; int head = 0 ; int bufferTam = 0 ;

int conexion = conectarCon(fdCliente, configGB->ipBroker, configGB->puertoBroker, logger);

	if (conexion == TRUE ) {

		log_info(loggerCatedra,"Me conecte correctamente con el %s IP: %s y Puerto: %d","BROKER",configGB->ipBroker,configGB->puertoBroker);

		handshake_cliente(fdCliente,"Team","Broker",logger);

		aplicar_protocolo_enviar(fdCliente,SUSCRIPCION,laSuscripcion);

		list_destroy(laSuscripcion->cola_a_suscribir);

		free(laSuscripcion);

		//printf("Se mostrarán en los Logs todos los msjd de la cola %s durante %d segundos\n",comando,tiempoSuscripcion);

		pthread_mutex_unlock(&semaforo);

		while (true) {

			recibirProtocolo(&head,&bufferTam,fdCliente); // recibo head y tamaño de msj

			mensaje = malloc(bufferTam);

			recibirMensaje(fdCliente , bufferTam , mensaje );

			if (head < 1 ){ // DESCONEXIÓN
				pthread_mutex_lock(&mxHilos);
				pthread_detach( pthread_self() );
				pthread_mutex_unlock(&mxHilos);
			}else{

			switch( head ){

					setlocale(LC_ALL,"");

					case NEW_POKEMON :{
						cola_NEW_POKEMON  new_poke ;
						deserealizar_NEW_POKEMON ( head, mensaje, bufferTam, & new_poke);
						free(mensaje);
						log_info(loggerCatedra,"Recibí de la suscripción -> NEW_POKEMON : POKEMON: %s  , CANTIDAD: %d  , CORDENADA X: %d , CORDENADA Y: %d ",new_poke.nombre_pokemon,new_poke.cantidad,new_poke.posicion_x,new_poke.posicion_y);
						responderACK(new_poke.id_mensaje);
						free(new_poke.nombre_pokemon);
						break;
					}
					case CATCH_POKEMON :{
						cola_CATCH_POKEMON cath_poke;
						deserealizar_CATCH_POKEMON( head, mensaje, bufferTam, & cath_poke);
						free(mensaje);
						log_info(loggerCatedra,"Recibí de la suscripción -> CATCH_POKEMON : POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",cath_poke.nombre_pokemon,cath_poke.posicion_x,cath_poke.posicion_y);
						responderACK(cath_poke.id_mensaje);
						free(cath_poke.nombre_pokemon);
						break;
					}
					case GET_POKEMON :{
						cola_GET_POKEMON get_poke ;
						deserealizar_GET_POKEMON ( head, mensaje, bufferTam, & get_poke);
						free(mensaje);
						log_info(loggerCatedra,"Recibí de la suscripción -> GET_POKEMON :  POKEMON: %s",get_poke.nombre_pokemon);
						responderACK(get_poke.id_mensaje);
						free(get_poke.nombre_pokemon);
						break;
					}

					case APPEARED_POKEMON :{
						cola_APPEARED_POKEMON app_poke;
						deserealizar_APPEARED_POKEMON ( head, mensaje, bufferTam, & app_poke);
						free(mensaje);
						log_info(loggerCatedra,"Recibí de la suscripción -> APPEARED_POKEMON : POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",app_poke.nombre_pokemon,app_poke.posicion_x,app_poke.posicion_y);
						responderACK(app_poke.id_mensaje);
						free(app_poke.nombre_pokemon);
						break;
					}

					case CAUGHT_POKEMON :{
						cola_CAUGHT_POKEMON caug_poke ;
						deserealizar_CAUGHT_POKEMON ( head, mensaje, bufferTam, & caug_poke);
						free(mensaje);
						log_info(loggerCatedra,"Recibí de la suscripción -> CAUGHT_POKEMON : MENSAJE ID: %d  , ATRAPO: %d",caug_poke.id_mensaje,caug_poke.atrapo_pokemon);
						responderACK(caug_poke.id_mensaje);
						break;
					}

					case LOCALIZED_POKEMON :{
						cola_LOCALIZED_POKEMON loc_poke ;
						deserealizar_LOCALIZED_POKEMON ( head, mensaje, bufferTam, & loc_poke);
						free(mensaje);
						for (int i = 0 ; i < list_size(loc_poke.lista_posiciones); i++){
						log_info(loggerCatedra,"Recibí de la suscripción -> LOCALIZED_POKEMON : POKEMON: %s  , CANTIDAD: %d , POSICIÓN X: %d , POSICIÓN Y: %d",loc_poke.nombre_pokemon,loc_poke.cantidad,list_get(loc_poke.lista_posiciones,i),list_get(loc_poke.lista_posiciones,i + 1));
						i++;
						}
						responderACK(loc_poke.id_mensaje);
						free(loc_poke.nombre_pokemon);
						list_destroy(loc_poke.lista_posiciones);
						break;
					}
					/*
					case ACK :{
						respuesta_ACK ack;
						deserealizar_ACK( head, mensaje, bufferTam, & ack);
						free(mensaje);
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
					}*/
					default:
						log_info(logger, "Instrucción no reconocida");
						break;			    					    				    				    										}
				}
			}
		}
}


