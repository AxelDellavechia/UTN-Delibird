/*
 * generales.c
 *
 *  Created on: 29 abr. 2020
 *      Author: utnso
 */
#include "generales.h"
#include "Broker.h"

void iniciar_estructuras(){
	//Se reserva la Memoria total del Broker
	memoria_cache = malloc(config_File->TAMANO_MEMORIA);
	memset(memoria_cache, '\0', config_File->TAMANO_MEMORIA);
	//Creo la particion inicial que contenga toda la memoria
	Particion * particion_memoria = reservarMemoria(sizeof(Particion));
	particion_memoria->punteroInicial = 0;
	particion_memoria->punteroFinal = config_File->TAMANO_MEMORIA;
	particion_memoria->tamano = config_File->TAMANO_MEMORIA;
	particion_memoria->libre = true;
	//Setea cantidad fallidas
	cantidad_fallidas = config_File->FRECUENCIA_COMPACTACION;
	id_msj = 0;
	contador_msjs_en_cola = 0;
	puntero_reemplazo = memoria_cache;

	//SE CREAN TODAS LAS LISTAS
	lista_msjs = list_create();
	lista_particiones = list_create();
	list_add(lista_particiones, particion_memoria);
	free(particion_memoria);

	lista_ack = list_create();

	suscriptores_new_pokemon = list_create();
	suscriptores_localized_pokemon = list_create();
	suscriptores_get_pokemon = list_create();
	suscriptores_appeared_pokemon = list_create();
	suscriptores_catch_pokemon = list_create();
	suscriptores_caught_pokemon = list_create();

	cola_new_pokemon = list_create();
	cola_localized_pokemon = list_create();
	cola_get_pokemon = list_create();
	cola_appeared_pokemon = list_create();
	cola_catch_pokemon = list_create();
	cola_caught_pokemon = list_create();


	//SE DEFINE MUTEX PARA DUMP DE MEMORIA CACHE
	pthread_mutex_init(&mutex_memoria_cache, NULL);
	pthread_mutex_init(&mutex_puntero_reemplazo, NULL);
	//SE DEFINE MUTEX PARA LA LISTA DE PARTICIONES
	pthread_mutex_init(&mutex_lista_particiones, NULL);
	//SE DEFINE MUTEX PARA VARIABLE DEL TIPO PRODUCTOR-CONSUMIDOR
	pthread_mutex_init(&mutex_id_msj, NULL);
	//SE DEFINE MUTEX PARA LA LISTA DE SUSCRIPTORES
	pthread_mutex_init(&mutex_suscriptores_new_pokemon, NULL);
	pthread_mutex_init(&mutex_suscriptores_localized_pokemon, NULL);
	pthread_mutex_init(&mutex_suscriptores_get_pokemon, NULL);
	pthread_mutex_init(&mutex_suscriptores_appeared_pokemon, NULL);
	pthread_mutex_init(&mutex_suscriptores_catch_pokemon, NULL);
	pthread_mutex_init(&mutex_suscriptores_caught_pokemon, NULL);
	//SE DEFINE MUTEX PARA LAS COLAS DE MSJS
	pthread_mutex_init(&mutex_contador_msjs_en_cola, NULL);
	pthread_mutex_init(&mutex_cola_new_pokemon, NULL);
	pthread_mutex_init(&mutex_cola_localized_pokemon, NULL);
	pthread_mutex_init(&mutex_cola_get_pokemon, NULL);
	pthread_mutex_init(&mutex_cola_appeared_pokemon, NULL);
	pthread_mutex_init(&mutex_cola_catch_pokemon, NULL);
	pthread_mutex_init(&mutex_cola_caught_pokemon, NULL);
	//SE DEFINE MUTEX PARA LOS HILOS
	pthread_mutex_init(&mxHilos, NULL);
	pthread_mutex_init(&mxSocketsFD, NULL);

	desplazamientoCache = 0 ;
}

void crearHilosBroker() {


	hilo_servidor= 0;
	hilo_consola= 0;
	hilo_Publisher = 0;

	pthread_create(&hilo_servidor, NULL, (void*) servidor, NULL);
	pthread_create(&hilo_consola, NULL, (void*) consola, NULL);
	//pthread_create(&hilo_Publisher, NULL, (void*) publisher, NULL);

	pthread_join(hilo_servidor, NULL);
	pthread_join(hilo_consola, NULL);
	//pthread_join(hilo_Publisher, NULL);

}

void consola() {

	printf("Hola! Ingresá \"salir\" para finalizar módulo\n");
	size_t buffer_size = 100; //por el momento restringido a 100 caracteres
	char* comando = (char *) calloc(1, buffer_size);

	while (!string_equals_ignore_case(comando, "salir\n")) {
		printf(">");
		int bytes_read = getline(&comando, &buffer_size, stdin);
		if (bytes_read == -1) {
			log_error(logger,"Error en getline");
		}
		if (bytes_read == 1) {
			continue;
		}
	}

	log_destroy(logger);
	log_destroy(loggerCatedra);
	free(comando);

	pthread_detach(hilo_servidor);
	pthread_cancel(hilo_servidor);

	pthread_detach(hilo_Publisher);
	pthread_cancel(hilo_Publisher);

	pthread_detach( pthread_self() );
}

void servidor() {

	fdBroker = nuevoSocket();

	asociarSocket(fdBroker, config_File->PUERTO_BROKER);
	escucharSocket(fdBroker, CONEXIONES_PERMITIDAS);

	log_info(logger," Escuchando conexiones");

	while(TRUE) {

		int conexionNueva = 0;

		while(conexionNueva == 0) {

			comandoNuevo = aceptarConexionSocket(fdBroker);

			conexionNueva = handshake_servidor ( comandoNuevo,"Broker" , "Team",logger);

			if( ! validar_conexion(conexionNueva, 0,logger) ) {
					pthread_mutex_lock(&mxSocketsFD); //desbloquea el semaforo
					cerrarSocket(fdBroker);
					pthread_mutex_unlock(&mxSocketsFD);
			}
		}
		pthread_t hilo;
		pthread_mutex_lock(&mxHilos);
		pthread_create(&hilo, NULL, (void*) thread_Broker,comandoNuevo);
		pthread_mutex_unlock(&mxHilos);

	}
}

int thread_Broker(int fdCliente) {

	while(TRUE){
	int head , bufferTam  ;

	int recibido = recibirProtocolo(&head,&bufferTam,fdCliente); // recibo head y tamaño de msj

	if (head < 1 || recibido <= 0){ // DESCONEXIÓN
		pthread_mutex_lock(&mxHilos);
		pthread_detach( pthread_self() );
		pthread_mutex_unlock(&mxHilos);
		return FALSE;
	}else{

						void * mensaje = malloc(bufferTam);

						recibirMensaje(fdCliente , bufferTam , mensaje ); // recibo msj serializado para el tratamiento deseado

						log_info(logger,"aplicar_protocolo_recibir -> recibió el HEAD #%d",head);

						log_info(logger,"aplicar_protocolo_recibir -> recibió un tamaño de -> %d",bufferTam);

						log_info(logger,"aplicar_protocolo_recibir -> comienza a deserealizar");

							switch( head ){

							setlocale(LC_ALL,"");

										case NEW_POKEMON :{

											//cola_NEW_POKEMON  new_poke ;
											cola_NEW_POKEMON  * ptro_new_poke =  malloc(sizeof(cola_NEW_POKEMON));
											deserealizar_NEW_POKEMON ( head, mensaje, bufferTam, ptro_new_poke);
											//log_info(logger,"Recibí en la cola NEW_POKEMON . POKEMON: %s  , CANTIDAD: %d  , CORDENADA X: %d , CORDENADA Y: %d ",new_poke.nombre_pokemon,new_poke.cantidad,new_poke.posicion_x,new_poke.posicion_y);
											log_info(logger,"Recibí en la cola NEW_POKEMON . POKEMON: %s  , CANTIDAD: %d  , CORDENADA X: %d , CORDENADA Y: %d ",ptro_new_poke->nombre_pokemon,ptro_new_poke->cantidad,ptro_new_poke->posicion_x,ptro_new_poke->posicion_y);
											pthread_mutex_lock(&mutex_cola_new_pokemon);
											ptro_new_poke->id_mensaje = obtener_idMsj();
											guardar_msj(head, bufferTam, ptro_new_poke);
											list_add(cola_new_pokemon, ptro_new_poke);
											pthread_mutex_unlock(&mutex_cola_new_pokemon);
											agregar_contador_msj();
											//pthread_mutex_lock();
											contador_msjs_en_cola++;
											//pthread_mutex_unlock();
											break;
										}
										case CATCH_POKEMON :{
											cola_CATCH_POKEMON * cath_poke = malloc (sizeof(cola_CATCH_POKEMON));
											deserealizar_CATCH_POKEMON( head, mensaje, bufferTam, cath_poke);
											log_info(logger,"Recibí en la cola CATCH_POKEMON . POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",cath_poke->nombre_pokemon,cath_poke->posicion_x,cath_poke->posicion_y);
											//GUARDAR O CACHEAR MSJ
											//cath_poke->id_mensaje = obtener_idMsj();
											pthread_mutex_lock(&mutex_cola_catch_pokemon);
											list_add(cola_catch_pokemon, cath_poke);
											pthread_mutex_unlock(&mutex_cola_catch_pokemon);
											agregar_contador_msj();
											break;
										}
										case GET_POKEMON :{
											cola_GET_POKEMON * get_poke = malloc (sizeof(cola_GET_POKEMON));
											deserealizar_GET_POKEMON ( head, mensaje, bufferTam, get_poke);
											log_info(logger,"Recibí en la cola GET_POKEMON . POKEMON: %s",get_poke->nombre_pokemon);
											get_poke->id_mensaje = obtener_idMsj();
											pthread_mutex_lock(&mutex_cola_get_pokemon);
											list_add(cola_get_pokemon, get_poke);
											pthread_mutex_unlock(&mutex_cola_get_pokemon);
											agregar_contador_msj();
											break;
										}

										case APPEARED_POKEMON :{
											cola_APPEARED_POKEMON * app_poke = malloc (sizeof(cola_APPEARED_POKEMON));
											deserealizar_APPEARED_POKEMON ( head, mensaje, bufferTam, app_poke);
											log_info(logger,"Recibí en la cola APPEARED_POKEMON . POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",app_poke->nombre_pokemon,app_poke->posicion_x,app_poke->posicion_y);
											app_poke->id_mensaje = obtener_idMsj();
											pthread_mutex_lock(&mutex_cola_appeared_pokemon);
											list_add(cola_appeared_pokemon, app_poke);
											pthread_mutex_unlock(&mutex_cola_appeared_pokemon);
											agregar_contador_msj();
											//free(app_poke->nombre_pokemon);
											break;
										}

										case CAUGHT_POKEMON :{
											cola_CAUGHT_POKEMON * caug_poke = malloc (sizeof(cola_CAUGHT_POKEMON));
											deserealizar_CAUGHT_POKEMON ( head, mensaje, bufferTam, caug_poke);
											log_info(logger,"Recibí en la cola CAUGHT_POKEMON . MENSAJE ID: %d  , ATRAPO: %d",caug_poke->id_mensaje,caug_poke->atrapo_pokemon);
											//caug_poke->id_mensaje = obtener_idMsj();
											pthread_mutex_lock(&mutex_cola_caught_pokemon);
											list_add(cola_caught_pokemon, caug_poke);
											pthread_mutex_unlock(&mutex_cola_caught_pokemon);
											agregar_contador_msj();
											break;
										}

										case LOCALIZED_POKEMON :{
											cola_LOCALIZED_POKEMON * loc_poke = malloc (sizeof(cola_LOCALIZED_POKEMON));
											deserealizar_LOCALIZED_POKEMON ( head, mensaje, bufferTam, loc_poke);
											for (int i = 0 ; i < list_size(loc_poke->lista_posiciones); i++){
											posicion * pos = list_get(loc_poke->lista_posiciones,i) ;
											log_info(logger,"Recibí en la cola LOCALIZED_POKEMON . POKEMON: %s  , CANTIDAD: %d , POSICIÓN X: %d , POSICIÓN Y: %d",loc_poke->nombre_pokemon,loc_poke->cantidad,pos->posicion_x,pos->posicion_y);
											}
											loc_poke->id_mensaje = obtener_idMsj();
											pthread_mutex_lock(&mutex_cola_localized_pokemon);
											list_add(cola_localized_pokemon, loc_poke);
											pthread_mutex_unlock(&mutex_cola_localized_pokemon);
											agregar_contador_msj();
											//free(loc_poke->nombre_pokemon);
											//list_destroy(loc_poke->lista_posiciones);
											break;
										}

										case ACK :{
											respuesta_ACK * ack = malloc(sizeof(respuesta_ACK));
											deserealizar_ACK( head, mensaje, bufferTam, ack);
											log_info(logger,"Recibí un ACK del token %d con los siguientes datos ESTADO: %d ID_MSJ: %d ",ack->token,ack->ack,ack->id_msj);
											list_add(lista_ack,ack);
											break;
										}

										case SUSCRIPCION :{


											losSuscriptores * suscripcionC = malloc(sizeof(losSuscriptores));
											suscripcionC->laSus = malloc(sizeof(suscriptor));
											suscripcionC->suSocket = comandoNuevo;

											deserealizar_suscriptor( head, mensaje, bufferTam, suscripcionC->laSus);
											for ( int i = 0 ; i < list_size(suscripcionC->laSus->cola_a_suscribir) ; i++){
												log_info(logger,"Recibí del modulo %s una suscribición a la cola %s con el token %d", devolverModulo(suscripcionC->laSus->modulo),tipoMsjIntoToChar(list_get(suscripcionC->laSus->cola_a_suscribir,i)),suscripcionC->laSus->token);
											}


											suscribirse(suscripcionC);

											respuesta_ACK * ack = malloc (sizeof(respuesta_ACK));
											ack->ack = TRUE;
											ack->id_msj = obtener_idMsj();

											/*struct sockaddr_in peer;
											int peer_len = sizeof(peer);
											if (getpeername(fdCliente, &peer, &peer_len) != -1) {
											log_info(logger,"Cliente IP address is: %s", inet_ntoa(peer.sin_addr));
											log_info(logger,"Cliente port is: %d", (int) ntohs(peer.sin_port));
											 }*/
											aplicar_protocolo_enviar(fdCliente,ACK,ack);

											reenviarMsjCache(suscripcionC);
											break;
										}
										default:
											log_info(logger, "Instrucción no reconocida");
											break;
									}
					}

	}
}

void* reservarMemoria(int size) {
		void *puntero = malloc(size);
		if(puntero == NULL) {
			fprintf(stderr, "Error al reservar %d bytes de memoria", size);
			exit(ERROR);
		}
		return puntero;
}

void leerArchivoDeConfiguracion(char *ruta,t_log * logger) {

	t_config *config;
	config = reservarMemoria (sizeof(t_config));
	config = config_create(ruta);

	if (config != NULL) {
		log_info(logger, "Leyendo Archivo de Configuracion..");

		if (config_has_property(config, "TAMANO_MEMORIA")) {
			config_File->TAMANO_MEMORIA = config_get_int_value(config,
								"TAMANO_MEMORIA");
						log_info(logger,
								"Se encontró y cargó el contido del TAMANO_MEMORIA. Valor: %d",config_File->TAMANO_MEMORIA);
					} else {

						log_error(logger,
								"El archivo de configuracion no contiene el TAMANO_MEMORIA");

		}

		if (config_has_property(config, "TAMANO_MINIMO_PARTICION")) {
			config_File->TAMANO_MINIMO_PARTICION = config_get_int_value(config,
								"TAMANO_MINIMO_PARTICION");
						log_info(logger,
								"Se encontró y cargó el contido del TAMANO_MINIMO_PARTICION. Valor: %d",config_File->TAMANO_MINIMO_PARTICION);
					} else {

						log_error(logger,
								"El archivo de configuracion no contiene el TAMANO_MINIMO_PARTICION");

		}

		if (config_has_property(config, "ALGORITMO_MEMORIA")) {
					config_File->ALGORITMO_MEMORIA = strdup(
													config_get_string_value(config, "ALGORITMO_MEMORIA"));
											log_info(logger,
													"Se encontró y cargó el contenido de ALGORITMO_MEMORIA a la estructura. Valor: %s",config_File->ALGORITMO_MEMORIA);

				} else {
											log_error(logger,
													"El archivo de configuracion no contiene el ALGORITMO_MEMORIA");
		}

		if (config_has_property(config, "ALGORITMO_REEMPLAZO")) {
					config_File->ALGORITMO_REEMPLAZO = strdup(
													config_get_string_value(config, "ALGORITMO_REEMPLAZO"));
											log_info(logger,
													"Se encontró y cargó el contenido de ALGORITMO_REEMPLAZO a la estructura. Valor: %s",config_File->ALGORITMO_REEMPLAZO);

				} else {
											log_error(logger,
													"El archivo de configuracion no contiene el ALGORITMO_REEMPLAZO");
		}

		if (config_has_property(config, "ALGORITMO_PARTICION_LIBRE")) {
					config_File->ALGORITMO_PARTICION_LIBRE = strdup(
													config_get_string_value(config, "ALGORITMO_PARTICION_LIBRE"));
											log_info(logger,
													"Se encontró y cargó el contenido de ALGORITMO_PARTICION_LIBRE a la estructura. Valor: %s",config_File->ALGORITMO_PARTICION_LIBRE);

				} else {
											log_error(logger,
													"El archivo de configuracion no contiene el ALGORITMO_PARTICION_LIBRE");
		}

		if (config_has_property(config, "IP_BROKER")) {
					config_File->IP_BROKER = strdup(
													config_get_string_value(config, "IP_BROKER"));
											log_info(logger,
													"Se encontró y cargó el contenido de IP_BROKER a la estructura. Valor: %s",config_File->IP_BROKER);

				} else {
											log_error(logger,
													"El archivo de configuracion no contiene el IP_BROKER");
		}



		if (config_has_property(config, "PUERTO_BROKER")) {
			config_File->PUERTO_BROKER = config_get_int_value(config,
										"PUERTO_BROKER");
								log_info(logger,
										"Se encontró y cargó el contido del PUERTO_BROKER. Valor: %d",config_File->PUERTO_BROKER);
							} else {

								log_error(logger,
										"El archivo de configuracion no contiene el PUERTO_BROKER");

		}

		if (config_has_property(config, "FRECUENCIA_COMPACTACION")) {
			config_File->FRECUENCIA_COMPACTACION = config_get_int_value(config,
										"FRECUENCIA_COMPACTACION");
								log_info(logger,
										"Se encontró y cargó el contido del FRECUENCIA_COMPACTACION. Valor: %d",config_File->FRECUENCIA_COMPACTACION);
							} else {

								log_error(logger,
										"El archivo de configuracion no contiene el FRECUENCIA_COMPACTACION");

		}
	}
	config_destroy(config);
}

void iniciar_log(){

	char * archivoLog = strdup("broker.log");
	char * archivoLogCatedra = strdup("brokerCatedra.log");

	logger = log_create(LOG_PATH_INTERNO, archivoLog, FALSE, LOG_LEVEL_INFO);
	loggerCatedra = log_create(LOG_PATH, archivoLogCatedra, FALSE, LOG_LEVEL_INFO);

	free(archivoLog);
	archivoLog = NULL;

	free(archivoLogCatedra);
	archivoLogCatedra = NULL;
}

void publisher(){

	while(TRUE){

		while(contador_msjs_en_cola != 0){

			if(list_size(cola_new_pokemon) != 0){
				reenviarMsjs_Cola(NEW_POKEMON, cola_new_pokemon, suscriptores_new_pokemon);
			}
			else if(list_size(cola_localized_pokemon) != 0){
				reenviarMsjs_Cola(LOCALIZED_POKEMON, cola_localized_pokemon, suscriptores_localized_pokemon);
			}
			else if(list_size(cola_get_pokemon) != 0){
				reenviarMsjs_Cola(GET_POKEMON, cola_get_pokemon, suscriptores_get_pokemon);
			}
			else if(list_size(cola_appeared_pokemon) != 0){
				reenviarMsjs_Cola(APPEARED_POKEMON, cola_appeared_pokemon, suscriptores_appeared_pokemon);
			}
			else if(list_size(cola_catch_pokemon) != 0){
				reenviarMsjs_Cola(CATCH_POKEMON, cola_catch_pokemon, suscriptores_catch_pokemon);
			}
			else if(list_size(cola_caught_pokemon) != 0){
				reenviarMsjs_Cola(CAUGHT_POKEMON, cola_caught_pokemon, suscriptores_caught_pokemon);
			}
			//mutex escritura
			contador_msjs_en_cola--;
		}

	}

}

void reenviarMsjs_Cola(int head, t_list * lista_Msjs_Cola, t_list * lista_de_suscriptores){

	while(!list_is_empty(lista_Msjs_Cola)){

		void * mensaje = list_get(lista_Msjs_Cola,0);

		t_list * aux_lista_de_suscriptores = list_create();

		list_add_all(aux_lista_de_suscriptores,lista_de_suscriptores);

		while(!list_is_empty(aux_lista_de_suscriptores)){

			//SE ENVIA POR SOCKET EL MENSAJE

			losSuscriptores * suscriptor = list_get(aux_lista_de_suscriptores,0);

			int enviados ;

			switch( head ){
					case NEW_POKEMON :{
						cola_NEW_POKEMON  * new_poke = mensaje ;
						enviados =  aplicar_protocolo_enviar(suscriptor->suSocket, head, new_poke);
						break;
					}
					case APPEARED_POKEMON :{
						cola_APPEARED_POKEMON  * app_poke = mensaje ;
						enviados =  aplicar_protocolo_enviar(suscriptor->suSocket, head, app_poke);
						break;
					}
					case CATCH_POKEMON :{
						cola_CATCH_POKEMON  * cath_poke = mensaje ;
						enviados =  aplicar_protocolo_enviar(suscriptor->suSocket, head, cath_poke);
						break;
					}
					case CAUGHT_POKEMON :{
						cola_CAUGHT_POKEMON  * cau_poke = mensaje ;
						enviados =  aplicar_protocolo_enviar(suscriptor->suSocket, head, cau_poke);
						break;
					}
					case GET_POKEMON :{
						cola_GET_POKEMON * get_poke = mensaje ;
						enviados =  aplicar_protocolo_enviar(suscriptor->suSocket, head, get_poke);
						break;
					}
					case LOCALIZED_POKEMON :{
						cola_LOCALIZED_POKEMON  * loc_poke = mensaje ;
						enviados = aplicar_protocolo_enviar(suscriptor->suSocket, head, loc_poke);
						break;
					}
				}

			if (enviados == ERROR ) log_info(logger,"No se puedo enviar correctamente el msj de la cola al suscriptor");

			log_info(logger,"Se puedo enviar correctamente el msj de la cola al suscriptor");

			list_remove(aux_lista_de_suscriptores, 0);
		}
		list_remove(lista_Msjs_Cola, 0);
	}
}

void reenviarMsjCache(losSuscriptores * laSus) {

	for ( int i = 0 ; i < list_size(lista_particiones) ; i++ ) {
	 Particion * laParti = list_get(lista_particiones,i);
	 if ( !laParti->libre) {
		_Bool estaPresente(respuesta_ACK * elAck){ return elAck->token == laSus->laSus->token && elAck->id_msj == laParti->id_msj;}
		_Bool fueRespondido = list_any_satisfy(lista_ack, (void*)estaPresente);

		if(!fueRespondido) {

			 desplazamientoCache = laParti->punteroInicial ;

						switch( laParti->colaAsignada ){
							case NEW_POKEMON :{
								cola_NEW_POKEMON  * new_poke = malloc(sizeof(cola_NEW_POKEMON)) ;
								deserealizar_NEW_POKEMON(laParti->colaAsignada,memoria_cache,laParti->tamano,new_poke);
								aplicar_protocolo_enviar(laSus->suSocket, laParti->colaAsignada , new_poke);
								break;
							}
							case APPEARED_POKEMON :{
								cola_APPEARED_POKEMON  * app_poke = malloc(sizeof(cola_NEW_POKEMON)) ;
								deserealizar_APPEARED_POKEMON(laParti->colaAsignada,memoria_cache,laParti->tamano,app_poke);
								aplicar_protocolo_enviar(laSus->suSocket, laParti->colaAsignada , app_poke);
								break;
							}
							case CATCH_POKEMON :{
								cola_CATCH_POKEMON  * cath_poke = malloc(sizeof(cola_CATCH_POKEMON)) ;
								deserealizar_CATCH_POKEMON (laParti->colaAsignada,memoria_cache,laParti->tamano,cath_poke);
								aplicar_protocolo_enviar(laSus->suSocket, laParti->colaAsignada , cath_poke);
								break;
							}
							case CAUGHT_POKEMON :{
								cola_CAUGHT_POKEMON  * cau_poke  = malloc(sizeof(cola_CAUGHT_POKEMON)) ;
								deserealizar_CAUGHT_POKEMON(laParti->colaAsignada,memoria_cache,laParti->tamano,cau_poke);
								aplicar_protocolo_enviar(laSus->suSocket, laParti->colaAsignada , cau_poke);
								break;
							}
							case GET_POKEMON :{
								cola_GET_POKEMON * get_poke  = malloc(sizeof(cola_GET_POKEMON)) ;
								deserealizar_GET_POKEMON(laParti->colaAsignada,memoria_cache,laParti->tamano,get_poke);
								aplicar_protocolo_enviar(laSus->suSocket, laParti->colaAsignada , get_poke);
								break;
							}
							case LOCALIZED_POKEMON :{
								cola_LOCALIZED_POKEMON  * loc_poke = malloc(sizeof(cola_LOCALIZED_POKEMON)) ;
								deserealizar_LOCALIZED_POKEMON(laParti->colaAsignada,memoria_cache,laParti->tamano,loc_poke);
								aplicar_protocolo_enviar(laSus->suSocket, laParti->colaAsignada , loc_poke);
								break;
							}
						}
		}
	 }
	}
}

int buscarEnLista( t_list * lista , suscriptor * buscado ) {
	_Bool buscar_token(suscriptor* suscriptor_lista){return suscriptor_lista->token == buscado->token;}
	return list_count_satisfying(lista, (void*)buscar_token);
}

void suscribirse(losSuscriptores * suscp){

	int i = 0 , cantColas;

	cantColas = list_size(suscp->laSus->cola_a_suscribir);

	while(i != cantColas){
		int cola;
		cola = list_get(suscp->laSus->cola_a_suscribir,i);

		switch (cola){

		case NEW_POKEMON :{
			//VERIFICO SI YA ESTABA INSCRIPTO, DE SER AFIRMATIVO LE REENVIO LOS MSJ
			if ( buscarEnLista(suscriptores_new_pokemon,&suscp->laSus) != 1) {
				pthread_mutex_lock(&mutex_suscriptores_new_pokemon);
				list_add(suscriptores_new_pokemon, suscp);
				pthread_mutex_unlock(&mutex_suscriptores_new_pokemon);
				break;
			}else{
				//VERIFICAR SI SE DESCONECTO
					//SI SE DESCONECTO EDITAR SOCKET CON EL NUEVO PERTENECIENTE
				reenviarMsjs_Cola(NEW_POKEMON,cola_new_pokemon,suscriptores_new_pokemon);
				break;
			}
		}
		case APPEARED_POKEMON :{

			if ( buscarEnLista(suscriptores_appeared_pokemon,&suscp->laSus) != 1) {
				pthread_mutex_lock(&mutex_suscriptores_appeared_pokemon);
				list_add(suscriptores_appeared_pokemon, suscp);
				pthread_mutex_unlock(&mutex_suscriptores_appeared_pokemon);
				break;
			}else{
				//VERIFICAR SI SE DESCONECTO
					//SI SE DESCONECTO EDITAR SOCKET CON EL NUEVO PERTENECIENTE
				reenviarMsjs_Cola(APPEARED_POKEMON,cola_appeared_pokemon,suscriptores_appeared_pokemon);
				break;
			}
		}
		case CATCH_POKEMON :{

			if ( buscarEnLista(suscriptores_catch_pokemon,&suscp->laSus) != 1) {
				pthread_mutex_lock(&mutex_suscriptores_catch_pokemon);
				list_add(suscriptores_catch_pokemon, suscp);
				pthread_mutex_unlock(&mutex_suscriptores_catch_pokemon);
				break;
			}else{
				//VERIFICAR SI SE DESCONECTO
					//SI SE DESCONECTO EDITAR SOCKET CON EL NUEVO PERTENECIENTE
				reenviarMsjs_Cola(CATCH_POKEMON,cola_catch_pokemon,suscriptores_catch_pokemon);
				break;
			}
		}
		case GET_POKEMON :{

			if ( buscarEnLista(suscriptores_get_pokemon,&suscp->laSus) != 1) {
				pthread_mutex_lock(&mutex_suscriptores_get_pokemon);
				list_add(suscriptores_get_pokemon, suscp);
				pthread_mutex_unlock(&mutex_suscriptores_get_pokemon);
				break;
			}else{
				//VERIFICAR SI SE DESCONECTO
					//SI SE DESCONECTO EDITAR SOCKET CON EL NUEVO PERTENECIENTE
				reenviarMsjs_Cola(GET_POKEMON,cola_get_pokemon,suscriptores_get_pokemon);
				break;
			}
		}
		case LOCALIZED_POKEMON :{

			if ( buscarEnLista(suscriptores_localized_pokemon,&suscp->laSus) != 1) {
				pthread_mutex_lock(&mutex_suscriptores_localized_pokemon);
				list_add(suscriptores_localized_pokemon, suscp);
				pthread_mutex_unlock(&mutex_suscriptores_localized_pokemon);
				break;
			}else{
				//VERIFICAR SI SE DESCONECTO
					//SI SE DESCONECTO EDITAR SOCKET CON EL NUEVO PERTENECIENTE
				reenviarMsjs_Cola(LOCALIZED_POKEMON,cola_localized_pokemon,suscriptores_localized_pokemon);
				break;
			}
		}
		case CAUGHT_POKEMON :{

			if ( buscarEnLista(suscriptores_caught_pokemon,&suscp->laSus) != 1) {
				pthread_mutex_lock(&mutex_suscriptores_localized_pokemon);
				list_add(suscriptores_caught_pokemon, suscp);
				pthread_mutex_unlock(&mutex_suscriptores_localized_pokemon);
				break;
			}else{
				//VERIFICAR SI SE DESCONECTO
					//SI SE DESCONECTO EDITAR SOCKET CON EL NUEVO PERTENECIENTE
				reenviarMsjs_Cola(CAUGHT_POKEMON,cola_caught_pokemon,suscriptores_caught_pokemon);
				break;
			}
		}
		default:
			log_info(logger, "Instrucción para suscribirse alguna cola no reconocida");
			break;
		}
		i++;
	}

}
/*
void obtener_msj(int id_msj , Mensaje * msj)
{
	_Bool particion_libre(Mensaje* elMensajeBuscado){return elMensajeBuscado-> elack.id_msj == id_msj;}
	msj = list_find(lista_msjs, (void*)particion_libre);
}
*/

int32_t obtener_idMsj(){
	int32_t id_msj_aux;
	pthread_mutex_lock(&mutex_id_msj);
	id_msj++;
	id_msj_aux = id_msj;
	pthread_mutex_unlock(&mutex_id_msj);
	return id_msj_aux;
}

void agregar_contador_msj(){
	pthread_mutex_lock(&mutex_contador_msjs_cola);
	contador_msjs_en_cola++;
	pthread_mutex_unlock(&mutex_contador_msjs_cola);
}


