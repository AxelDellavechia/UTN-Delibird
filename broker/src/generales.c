/*
 * generales.c
 *
 *  Created on: 29 abr. 2020
 *      Author: utnso
 */
#include "generales.h"

/*
 void consola() {

	setlocale(LC_ALL,"");

	printf("Hola! este es el modulo BROKER y mi función es administrar las colas de mensajes\n");
	printf("--------------------------------------------------------\n");
	printf ("Para finalizar módulo ingrese \"SALIR\"\n");

	size_t buffer_size = 100;

	char* comando = (char *) calloc(1, buffer_size);

	do{
		printf(">");
		int bytes_read = getline(&comando, &buffer_size, stdin);
		if (bytes_read == -1) {
			free(comando);
			exit(0);
		}
		if (bytes_read == 1) {
			continue;
		}
	}while (strcasecmp(comando, "salir") != 1) ;
}
*/

void crearHilosBroker() {


	hilo_servidor= 0;
	hilo_consola= 0;

	pthread_create(&hilo_servidor, NULL, (void*) servidor, NULL);
	pthread_create(&hilo_consola, NULL, (void*) consola, NULL);

	pthread_join(hilo_servidor, NULL);
	pthread_join(hilo_consola, NULL);

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

	pthread_detach( pthread_self() );

}

void inicializar_semaforos(){
	//inicializo semaforos de nodos
	pthread_mutex_init(&mxHilos, NULL);
	pthread_mutex_init(&mxSocketsFD, NULL);
}


void servidor() {

	fdBroker = nuevoSocket();

	asociarSocket(fdBroker, config_File->PUERTO_BROKER);
	escucharSocket(fdBroker, CONEXIONES_PERMITIDAS);

	log_info(logger," Escuchando conexiones");

	while(TRUE) {

		int conexionNueva = 0;
		int comandoNuevo;//= reservarMemoria(INT);

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

	int head , bufferTam  ;

	recibirProtocolo(&head,&bufferTam,fdCliente); // recibo head y tamaño de msj

						void * mensaje = malloc(bufferTam);

						recibirMensaje(fdCliente , bufferTam , mensaje ); // recibo msj serializado para el tratamiento deseado

						log_info(logger,"aplicar_protocolo_recibir -> recibió el HEAD #%d",head);

						log_info(logger,"aplicar_protocolo_recibir -> recibió un tamaño de -> %d",bufferTam);

						log_info(logger,"aplicar_protocolo_recibir -> comienza a deserealizar");

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
											respuesta_ACK * ack = malloc (sizeof(respuesta_ACK));
											ack->ack = TRUE;
											ack->id_msj = 0;
											aplicar_protocolo_enviar(fdCliente,ACK,ack);
											break;
										}
										default:
											log_info(logger, "Instrucción no reconocida");
											break;
									}

	pthread_mutex_lock(&mxHilos);
	pthread_detach( pthread_self() );
	pthread_mutex_unlock(&mxHilos);
	return FALSE;
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
/*
void reenviarMsjCola_NEW_POKEMON(void * mensaje){
	int tam_list, i = 0;
	suscriptor *suscriptor;
	tam_list = list_size(cola_new_pokemon);
	while(i != tam_list){
		suscriptor = list_get(cola_new_pokemon, i);
		int fdSuscriptor = nuevoSocket();
		int conexion = conectarSocket(fdSuscriptor, suscriptor->mi_ip, suscriptor->mi_puerto);
		if(conexion == ERROR){
			log_info(logger,"[SOCKETS] No se pudo realizar la conexión entre el socket y el suscriptor(Gamecard).");
		}
		aplicar_protocolo_enviar(fdSuscriptor, NEW_POKEMON, mensaje);
		//SE DEBE DE AGREGAR UNA ESPERA PENDIENTE DEL ACK LUEGO DE QUE LE LLEGUE EL MSJ
	}

}
*/
void Suscribirse(suscriptor * suscp){
	int i, cantColas;
	//t_list * colas_suscp = suscp->cola_a_suscribir;
	cantColas = list_size(suscp->cola_a_suscribir);
	while(i != cantColas){
		int cola;
		cola = list_get(suscp->cola_a_suscribir,i);
		switch (cola){
		case NEW_POKEMON :{
			pthread_mutex_lock(&mutex_suscriptores_new_pokemon);
			list_add(suscriptores_new_pokemon, &suscp);
			pthread_mutex_unlock(&mutex_suscriptores_new_pokemon);
			enviarConfirmacion(suscp);//LE ENVIO CONFIRMACION AL PROCESO (GAMECARD O TEAM)
			break;
			}
		case APPEARED_POKEMON :{
			pthread_mutex_lock(&mutex_suscriptores_appeared_pokemon);
			list_add(suscriptores_appeared_pokemon, &suscp);
			pthread_mutex_unlock(&mutex_suscriptores_appeared_pokemon);
			//LE ENVIO CONFIRMACION AL PROCESO (GAMECARD O TEAM)
			break;
			}
		case CATCH_POKEMON :{
			pthread_mutex_lock(&mutex_suscriptores_catch_pokemon);
			list_add(suscriptores_catch_pokemon, &suscp);
			pthread_mutex_unlock(&mutex_suscriptores_catch_pokemon);
			//LE ENVIO CONFIRMACION AL PROCESO (GAMECARD O TEAM)
			break;
			}
		case GET_POKEMON :{
			pthread_mutex_lock(&mutex_suscriptores_get_pokemon);
			list_add(suscriptores_get_pokemon, &suscp);
			pthread_mutex_unlock(&mutex_suscriptores_get_pokemon);
			//LE ENVIO CONFIRMACION AL PROCESO (GAMECARD O TEAM)
			break;
			}
		case LOCALIZED_POKEMON :{
			pthread_mutex_lock(&mutex_suscriptores_localized_pokemon);
			list_add(suscriptores_localized_pokemon, &suscp);
			pthread_mutex_unlock(&mutex_suscriptores_localized_pokemon);
			//LE ENVIO CONFIRMACION AL PROCESO (GAMECARD O TEAM)
			break;
			}
		default:
			log_info(logger, "Instrucción para suscribirse alguna cola no reconocida");
			break;
		}
		i++;
	}

}

void enviarConfirmacion(){

}
