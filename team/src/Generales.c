#include "Generales.h"


void iniciar_logCatedra(){
	aux = string_split(configFile->logFile,"/") ;
	int i = 0 ;
	while (aux[i] != NULL) {
		if ( string_contains(aux[i],"log") ) {
			char * archivoLogCatedra = strdup(aux[i]);
			loggerCatedra = log_create(configFile->logFile, archivoLogCatedra, FALSE, LOG_LEVEL_INFO);
			free(archivoLogCatedra);
			archivoLogCatedra = NULL;
		}
		i++;
	}
	//free(aux);
}

void iniciar_log(){
	char * archivoLog = strdup("Team.log");
	logger = log_create(LOG_PATH_INTERNO, archivoLog, FALSE, LOG_LEVEL_INFO);
	free(archivoLog);
	archivoLog = NULL;
}

void leerArchivoDeConfiguracion(char *ruta, t_log *logger) {

	//setlocale(LC_ALL,"");

	int posicionLista = 0;

	configFile = reservarMemoria(sizeof(archivoConfigFile));

	t_config *config;

	log_info(logger, "Leyendo Archivo de Configuracion..");

	config = config_create(ruta);

	posicionEntrenadoresAux =  config_get_array_value(config, "POSICIONES_ENTRENADORES") ;
	pokemonEntrenadoresAux =  config_get_array_value(config, "POKEMON_ENTRENADORES") ;
	objetivosEntrenadoresAux =  config_get_array_value(config, "OBJETIVOS_ENTRENADORES") ;

	configFile->tiempoReconexion = config_get_int_value(config, "TIEMPO_RECONEXION");
	log_info(logger,"Se encontró y cargó el contido del TIEMPO_RECONEXION. Valor: %d",configFile->tiempoReconexion);

	configFile->retardoCicloCPU = config_get_int_value(config, "RETARDO_CICLO_CPU");
	log_info(logger,"Se encontró y cargó el contido del RETARDO_CICLO_CPU. Valor: %d",configFile->retardoCicloCPU);

	configFile->algoritmoPlanificacion = strdup ( config_get_string_value(config, "ALGORITMO_PLANIFICACION"));
	log_info(logger,"Se encontró y cargó el contido del ALGORITMO_PLANIFICACION. Valor: %s",configFile->algoritmoPlanificacion);

	configFile->quantum = config_get_int_value(config, "QUANTUM");
	log_info(logger,"Se encontró y cargó el contido del QUANTUM. Valor: %d",configFile->quantum);

	configFile->ipBroker = strdup ( config_get_string_value(config, "IP_BROKER"));
	log_info(logger,"Se encontró y cargó el contido del IP_BROKER. Valor: %s",configFile->ipBroker);

	configFile->estimacionInicial = config_get_int_value(config, "ESTIMACION_INICIAL");
	log_info(logger,"Se encontró y cargó el contido del ESTIMACION_INICIAL. Valor: %d",configFile->estimacionInicial);

	configFile->puertoBroker = config_get_int_value(config, "PUERTO_BROKER");
	log_info(logger,"Se encontró y cargó el contido del PUERTO_BROKER. Valor: %d",configFile->puertoBroker);

	configFile->puertoTeam = config_get_int_value(config, "PUERTO_TEAM");
	log_info(logger,"Se encontró y cargó el contido del PUERTO_TEAM. Valor: %d",configFile->puertoTeam);

	configFile->logFile = strdup ( config_get_string_value(config, "LOG_FILE"));
	log_info(logger,"Se encontró y cargó el contido del LOG_FILE. Valor: %s",configFile->logFile);

	configFile->posicionEntrenadores = list_create();
	configFile->pokemonEntrenadores = list_create();
	configFile->objetivosEntrenadores = list_create();
	while(posicionEntrenadoresAux[posicionLista] != NULL) {
		list_add(configFile->posicionEntrenadores, string_duplicate(posicionEntrenadoresAux[posicionLista]));
		log_info(logger,"Se encontró en la posición %d  de la lista POSICIONES_ENTRENADORES el valor -> %s",posicionLista,list_get(configFile->posicionEntrenadores,posicionLista));
		posicionLista++;
	}
	posicionLista = 0;
	while(pokemonEntrenadoresAux[posicionLista] != NULL) {
		list_add(configFile->pokemonEntrenadores, string_duplicate(pokemonEntrenadoresAux[posicionLista]));
		log_info(logger,"Se encontró en la posición %d  de la lista POKEMON_ENTRENADORES el valor -> %s",posicionLista,list_get(configFile->pokemonEntrenadores,posicionLista));
		posicionLista++;
	}

	posicionLista = 0;
	while(objetivosEntrenadoresAux[posicionLista] != NULL) {
		list_add(configFile->objetivosEntrenadores, string_duplicate(objetivosEntrenadoresAux[posicionLista]));
		log_info(logger,"Se encontró en la posición %d  de la lista OBJETIVOS_ENTRENADORES el valor -> %s",posicionLista,list_get(configFile->objetivosEntrenadores,posicionLista));
		posicionLista++;
	}

	config_destroy(config); // Libero la estructura archivoConfig

}

void* reservarMemoria(int size) {
		void *puntero = malloc(size);
		if(puntero == NULL) {
			fprintf(stderr, "Error al reservar %d bytes de memoria", size);
			exit(ERROR);
		}
		return puntero;
}

void crearEstructuras() {
	objetivoTeam = list_create();
	colaNew = list_create();
	colaReady = list_create();
	colaBlocked = list_create();
	colaExit = list_create();
	mapaPokemon = list_create();
	misHilos = list_create();
	entrenadoresEnDeadlock = list_create();
}

//este tiene los agoritmos de palinifación
void ejecutar() {
	char* algoritmo = configFile->algoritmoPlanificacion;
	int algoritmoInt = -1;
	char* accion;
	int quantum;
	char* accionAComparar;
	int posicionProximoAEjecutar;
	int rafagaCPUAccion;
	entrenadorPokemon* entrenador = list_get(colaReady, 0);
	accion = entrenador->proximaAccion;
	if (strcmp("FIFO", algoritmo)) {
		exec = entrenador;
		pthread_mutex_unlock(&exec->semaforMutex);
		list_remove(colaReady, 0);
		realizarAccion(accion, 0);
	} else if (strcmp("RR", algoritmo)) {
		quantum = configFile->quantum;
		exec = entrenador;

		list_remove(colaReady, 0);
		realizarAccion(accion, quantum);
	}else if (strcmp("SJF sin desalojo", algoritmo)) {
		rafagaCPUAccion = calcularRafagaCPU(accion);
		for (int posicionEntrenador = 1; posicionEntrenador < list_size(colaReady); posicionEntrenador++) {
			entrenadorPokemon* entrenador = list_get(colaReady, posicionEntrenador);
			accionAComparar = entrenador->proximaAccion;
			int rafagaCPUComparacion = calcularRafagaCPU(accionAComparar);
			if(rafagaCPUComparacion > rafagaCPUAccion) {
				rafagaCPUAccion = rafagaCPUComparacion;
				accion = accionAComparar;
				posicionProximoAEjecutar = posicionEntrenador;
			}
		}
		exec = entrenador;
		list_remove(colaReady, posicionProximoAEjecutar);
		realizarAccion(accion, 0);
	} else if (strcmp("SJF con desalojo", algoritmo)){
		rafagaCPUAccion = calcularRafagaCPU(accion);
		for (int posicionEntrenador = 1; posicionEntrenador < list_size(colaReady); posicionEntrenador++) {
			entrenadorPokemon* entrenador = list_get(colaReady, posicionEntrenador);
			accionAComparar = entrenador->proximaAccion;
			int rafagaCPUComparacion = calcularRafagaCPU(accionAComparar);
			if(rafagaCPUComparacion > rafagaCPUAccion) {
				rafagaCPUAccion = rafagaCPUComparacion;
				accion = accionAComparar;
				posicionProximoAEjecutar = posicionEntrenador;
			}
		}
		if (exec != NULL) {
			list_add(colaReady, exec);
		}
		exec = entrenador;
		list_remove(colaReady, posicionProximoAEjecutar);
		realizarAccion(accion, 0);
	} else {
		printf("Algoritmo incorrecto/n");
	}
}

int calcularRafagaCPU(accion) {
	return 0 ;
}

void realizarAccion(char* accion, int tiempo) {
	int accionInt = -1;
	if (strcmp("AtraparPokemon", accion)) {

	}
	switch(accionInt) {
	case 0:

	break;
	}
}


void inicializar_semaforos(){

	//inicializo semaforos de nodos
	pthread_mutex_init(&mxSocketsFD, NULL);
	pthread_mutex_init(&mxHilos, NULL);
	pthread_mutex_init(&h_reconectar,NULL);

}

void crearHilos() {

	hilo_servidor = 0;
	hilo_consola = 0;


	pthread_create(&hilo_servidor, NULL, (void*) planificador, NULL);
	pthread_create(&hilo_consola, NULL, (void*) consola, NULL);
	pthread_create(&hilo_conexion, NULL, (void*) reconectar, NULL);

	pthread_mutex_lock(&h_reconectar);

	pthread_join(hilo_servidor, NULL);
	pthread_join(hilo_consola, NULL);
	pthread_join(hilo_conexion, NULL);

	//tener un thread para manejar la reconexión cada x segundos , es un connect por cada msj
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

	pthread_detach(hilo_conexion);
	pthread_cancel(hilo_conexion);

	for ( int i = 0 ; i < list_size(configFile->posicionEntrenadores) ; i++){
		pthread_detach(hilo);
		pthread_cancel(hilo);
	}

	pthread_detach( pthread_self() );

}

// debería funcionar cmoo el planificador

void bloquearSemaforoEnt( t_list * lista , int indice ) {

	entrenadorPokemon * elEntrenador = list_get(lista,indice);
	//log_info(logger,"El thread_Entrenador %d tiene su semaforo en valor: %d antes del wait",elEntrenador->idEntrenador,elEntrenador->semafoContador)
	list_remove(lista,indice);
	pthread_mutex_lock(&elEntrenador->semaforMutex);
	//log_info(logger,"El thread_Entrenador %d tiene su semaforo en valor: %d despues del signal",elEntrenador->idEntrenador,elEntrenador->semafoContador);
	list_add_in_index(lista,indice,elEntrenador);

}

void desBloquearSemaforoEnt( t_list * lista , int indice ) {

	entrenadorPokemon * elEntrenador = list_get(lista,indice);
	list_remove(lista,indice);
	pthread_mutex_unlock(&elEntrenador->semaforMutex);
	list_add_in_index(lista,indice,elEntrenador);
}

void tiempoEspera(){



}

void * generarSus( suscriptor * suscriptor ){

		suscriptor->modulo = TEAM;
		suscriptor->token = token();
		suscriptor->cola_a_suscribir = list_create();
		list_add(suscriptor->cola_a_suscribir,GET_POKEMON);
		list_add(suscriptor->cola_a_suscribir,LOCALIZED_POKEMON);
		list_add(suscriptor->cola_a_suscribir,CATCH_POKEMON);
		list_add(suscriptor->cola_a_suscribir,CAUGHT_POKEMON);
		list_add(suscriptor->cola_a_suscribir,APPEARED_POKEMON);
}


void planificador() {


	log_info(logger," Obteniendo los Entrenados");

	obtenerEntrenadores(logger);

	fdTeam = nuevoSocket();

	fdBroker = nuevoSocket();

	asociarSocket(fdTeam, configFile->puertoTeam);

	escucharSocket(fdTeam, CONEXIONES_PERMITIDAS);

	conBroker = conectarCon(fdBroker,configFile->ipBroker,configFile->puertoBroker,logger);

	int head , bufferTam  ;

	generarSus(&laSus);

	for ( int i = 0 ; i < list_size(laSus.cola_a_suscribir) ; i++){
		log_info(logger,"El modulo %s se va a suscribir a la cola %s con el token %d", devolverModulo(laSus.modulo),tipoMsjIntoToChar(list_get(laSus.cola_a_suscribir,i)),laSus.token);
	}

	if ( conBroker == 1) {
		handshake_cliente(fdBroker,"Team","Broker",logger);
		aplicar_protocolo_enviar(fdBroker,SUSCRIPCION,&laSus);
		void * mensaje = malloc(bufferTam);
		recibirProtocolo(head,bufferTam,fdBroker);
		if ( head == ACK) {
			recibirMensaje(fdBroker,bufferTam,mensaje);
			respuesta_ACK elACK;
			deserealizar_ACK(head,mensaje,bufferTam,&elACK);
			if (elACK.ack == TRUE)	;//guardotoken con el metodo en el config by Javi
		}
	}

	else pthread_mutex_unlock(&h_reconectar);

	log_info(logger," Escuchando conexiones");



	for ( int i = 0 ; i < list_size(configFile->posicionEntrenadores) ; i++){
			entrenadorPokemon * entrenador = list_get(colaNew,i) ;
			//sem_wait(&entrenador->semafoContador);
			pthread_create(&hilo, NULL, (void*) thread_Entrenador,entrenador);
			//list_add(misHilos,process_get_thread_id());
			//log_info(logger,"Cree un hilo para el entrenador %d y tiene ID %d",i,hilo=pthread_self());
	}

	while(TRUE) {

		int conexionNueva = 0;

		int comandoNuevo;//= reservarMemoria(INT);

		while(conexionNueva == 0) {

			comandoNuevo = aceptarConexionSocket(fdTeam);

			conexionNueva = handshake_servidor ( comandoNuevo,"Team" , "Broker",logger);

			if(!validar_conexion(conexionNueva, 0,logger) ) {

				cerrarSocket(fdTeam);

			}

		}

		recibirProtocolo(&head,&bufferTam,comandoNuevo); // recibo head y tamaño de msj

					void * mensaje = malloc(bufferTam);

					recibirMensaje(comandoNuevo , bufferTam , mensaje ); // recibo msj serializado para el tratamiento deseado

					log_info(logger,"aplicar_protocolo_recibir -> recibió el HEAD #%d",head);

					log_info(logger,"aplicar_protocolo_recibir -> recibió un tamaño de -> %d",bufferTam);

					log_info(logger,"aplicar_protocolo_recibir -> comienza a deserealizar");

						switch( head ){
									/*
									case NEW_POKEMON :{
										cola_NEW_POKEMON  new_poke ;
										deserealizar_NEW_POKEMON ( head, mensaje, bufferTam, & new_poke);
										log_info(logger,"Recibí en la cola NEW_POKEMON . POKEMON: %s  , CANTIDAD: %d  , CORDENADA X: %d , CORDENADA Y: %d ",new_poke.nombre_pokemon,new_poke.cantidad,new_poke.posicion_x,new_poke.posicion_y);
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
									*/
									case ACK :{
										respuesta_ACK ack;
										deserealizar_ACK( head, mensaje, bufferTam, & ack);
										log_info(logger,"Recibí un ACK con los siguientes datos ESTADO: %d ID_MSJ: %d ",ack.ack,ack.id_msj);
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
										desBloquearSemaforoEnt(colaNew,1);
										//aplicar_proto falla devuelvo respuesta default y despierta el hilo reconectar
										free(loc_poke.nombre_pokemon);
										list_destroy(loc_poke.lista_posiciones);
										break;
									}
									default:
										log_info(logger, "Instrucción no reconocida");
										break;
								}
							//ejecutar

	}
}

void thread_Entrenador(entrenadorPokemon * elEntrenador) {

	pthread_mutex_lock(& elEntrenador->semaforMutex);
	log_info(logger,"Trabajando con el Entrenador Nro %d",elEntrenador->idEntrenador);
	//realizarAccion()
	//EN EL CATCH -> APLICAR_PROTOCOLO_ENVIAR -> CATCH
	// RECIBIR EL ID DE MSJ Y GUARDARLO EN UNA VARIABLE
	//CUANDO RECIBO EL CAUGHT VALIDO MIS ENTRENADORES Y DESPIERTO AL QUE ESPERA EL MSJ
	// VER SITUACIÓN DE EJECUTACIÓN.
	pthread_mutex_unlock(&elEntrenador->semaforMutex);

}

void reconectar(){

	pthread_mutex_lock(&h_reconectar);

	int head , bufferTam  ;

	while ( conBroker != 1 ) {

		usleep(configFile->tiempoReconexion * 1000000);

		conBroker = conectarCon(fdBroker,configFile->ipBroker,configFile->puertoBroker,logger);

	}

	handshake_cliente(fdBroker,"Team","Broker",logger);

	if (configFile->token == 0 ){
		generarSus(&laSus);
		aplicar_protocolo_enviar(fdBroker,SUSCRIPCION,&laSus);
		void * mensaje = malloc(bufferTam);
		recibirProtocolo(head,bufferTam,fdBroker);
		if ( head == ACK) {
			recibirMensaje(fdBroker,bufferTam,mensaje);
			respuesta_ACK elACK;
			deserealizar_ACK(head,mensaje,bufferTam,&elACK);
			if (elACK.ack == TRUE)	;//guardotoken con el metodo en el config by Javi
		}
	}

}

