<<<<<<< HEAD
/*
=======
>>>>>>> origin/master
#include "protocolos_comunicacion.h"

int validar_recive(int status, int modo) {

	if( (status == ERROR) || (status == 0) ) {

		if(modo == 1) { // Modo terminante
			exit(ERROR);
		} else { // Modo no terminante
			return FALSE;
		}

	} else {
		return TRUE;
	}
}

int validar_servidor_o_cliente(char *id , char* mensajeEsperado,t_log* logger) {
	if(strcasecmp(id, mensajeEsperado) == 0 ) {
<<<<<<< HEAD
		log_info(logger,"Servidor o Cliente aceptado.");
		return TRUE;
	} else {
		log_info(logger,"Servidor o Cliente rechazado.");
=======

		return TRUE;
	} else {

>>>>>>> origin/master
		return FALSE;
	}
}

int validar_conexion(int ret, int modo,t_log* logger) {

	if(ret == ERROR) {

		if(modo == 1) { // Modo terminante
			exit(ERROR);
		} else { // Sino no es terminante
			return FALSE;
		}
	} else { // No hubo error
<<<<<<< HEAD
		log_info(logger, "Alguien se conectó");

=======

		log_info(logger, "Alguien se conectó");


>>>>>>> origin/master
		return TRUE;
	}
}

int handshake_servidor (int sockClienteDe, char *mensajeEnviado , char *mensajeEsperado,t_log* logger) {

	enviarPorSocket(sockClienteDe, mensajeEnviado, string_length(mensajeEnviado));

	char *buff = (char*) malloc( string_length(mensajeEsperado) + 1 ) ;

	int status = recibirPorSocket(sockClienteDe, buff, string_length(mensajeEsperado));

	//log_info(logger,"metodo handshake_servidor recibió: %d", status);

	if( validar_recive(status, 0) ) { // El cliente envió un mensaje

		buff[string_length(mensajeEsperado)] = '\0';
		if (validar_servidor_o_cliente(buff , mensajeEsperado,logger)) {
<<<<<<< HEAD
			log_info(logger,"Hice el handshake y me respondieron: %s", buff);
=======

			log_info(logger,"Hice el handshake y me respondieron: %s", buff);

>>>>>>> origin/master
			free(buff);
			return TRUE;
		} else {
			free(buff);
			return FALSE;
		}

	} else { // Hubo algún error o se desconectó el cliente

		free(buff);
		return FALSE;
	}
	return FALSE; // No debería llegar acá pero lo pongo por el warning
}


int handshake_cliente (int sockClienteDe, char *mensajeEnviado , char *mensajeEsperado,t_log* logger) {

	char *buff = (char*) malloc( string_length(mensajeEsperado) + 1 ) ;

	int status = recibirPorSocket(sockClienteDe, buff, string_length(mensajeEsperado));

	log_info(logger,"metodo handshake_cliente recibió: %d", status);

		validar_recive(status, 0); // Es terminante ya que la conexión es con el servidor

		buff[string_length(mensajeEsperado)] = '\0';
		if( ! validar_servidor_o_cliente(buff , mensajeEsperado,logger) ) {
			free(buff);
			return FALSE;
		} else {
			log_info(logger,"Handshake recibido: %s", buff);
			free(buff);
			enviarPorSocket(sockClienteDe, mensajeEnviado, string_length(mensajeEnviado));
			return TRUE;
		}
}


int conectarCon(int fdServer , char * ipServer , int portServer,t_log* logger) {

<<<<<<< HEAD

		//log_info(logger,"metodo conectarCon");
		//log_info(logger,"fdServer:%d", fdServer);
		//log_info(logger,"ipServer:%s", ipServer);
		//log_info(logger,"portServer:%d", portServer);

		int conexion = conectarSocket(fdServer, ipServer, portServer);

		if(conexion == ERROR){
			log_info(logger,"No se pudo realizar la conexión entre el socket[%d] y el servidor ip: %s / puerto: %d " , fdServer , ipServer,portServer);
			return FALSE;
		}
		else{
			log_info(logger,"Se pudo realizar la conexión entre el socket[%d] y el servidor ip: %s / puerto: %d " , fdServer , ipServer,portServer);
=======
		/*
		log_info(logger,"metodo conectarCon");
		log_info(logger,"fdServer:%d", fdServer);
		log_info(logger,"ipServer:%s", ipServer);
		log_info(logger,"portServer:%d", portServer);
		 */
		int conexion = conectarSocket(fdServer, ipServer, portServer);

		if(conexion == ERROR){

			log_info(logger,"No se pudo realizar la conexión entre el socket[%d] y el servidor ip: %s / puerto: %d " , fdServer , ipServer,portServer);

			return FALSE;
		}
		else{

			log_info(logger,"Se pudo realizar la conexión entre el socket[%d] y el servidor ip: %s / puerto: %d " , fdServer , ipServer,portServer);

>>>>>>> origin/master
			return TRUE;
		}

}

void * serealizar(int head, void * mensaje ,  int tamanio){

	int desplazamiento = 0;

	buffer = malloc ( tamanio ) ;

	cola_NEW_POKEMON * new_pokemon = (cola_NEW_POKEMON *) mensaje;
	cola_APPEARED_POKEMON * appeared_pokemon = (cola_APPEARED_POKEMON *) mensaje;
	cola_CATCH_POKEMON * catch_pokemon = (cola_CATCH_POKEMON *) mensaje;
	cola_CAUGHT_POKEMON * caug_poke = (cola_CAUGHT_POKEMON * ) mensaje;
	cola_GET_POKEMON * get_pokemon = (cola_GET_POKEMON *) mensaje;
	cola_LOCALIZED_POKEMON * localized_pokemon = (cola_LOCALIZED_POKEMON *) mensaje;
<<<<<<< HEAD


			//typedef struct{
				//uint32_t id_mensaje;
				//char* nombre_pokemon;
				//uint32_t  tamanio_nombre;
				//uint32_t  posicion_x;
				//uint32_t posicion_y;
				//uint32_t cantidad;
			//}cola_NEW_POKEMON;

=======
	suscriptor *  sucriptor = (suscriptor *) mensaje;
	respuesta_ACK * ack = (respuesta_ACK *) mensaje;


>>>>>>> origin/master
	switch(head) {

	case NEW_POKEMON: {

<<<<<<< HEAD


=======
>>>>>>> origin/master
				memcpy(buffer+desplazamiento,&new_pokemon->id_mensaje,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				memcpy(buffer+desplazamiento,&new_pokemon->tamanio_nombre,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				memcpy(buffer+desplazamiento,new_pokemon->nombre_pokemon,new_pokemon->tamanio_nombre);
				desplazamiento += new_pokemon->tamanio_nombre;
				memcpy(buffer+desplazamiento,&new_pokemon->posicion_x,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				memcpy(buffer+desplazamiento,&new_pokemon->posicion_y,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				memcpy(buffer+desplazamiento,&new_pokemon->cantidad,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);

				break;

<<<<<<< HEAD
	}

		//typedef struct{
			//uint32_t id_mensaje;
			//char* nombre_pokemon;
			//uint32_t  tamanio_nombre;
			//uint32_t posicion_x;
			//uint32_t posicion_y;
		//}cola_APPEARED_POKEMON;


	case APPEARED_POKEMON: {

		memcpy(buffer+desplazamiento,&appeared_pokemon->id_mensaje,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer+desplazamiento,&appeared_pokemon->tamanio_nombre,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer+desplazamiento,appeared_pokemon->nombre_pokemon,string_length(appeared_pokemon->nombre_pokemon));
		desplazamiento += appeared_pokemon->tamanio_nombre;
		memcpy(buffer+desplazamiento,&appeared_pokemon->posicion_x,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer+desplazamiento,&appeared_pokemon->posicion_y,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);

		break;
	}

		//typedef struct{
			//uint32_t id_mensaje;
			//char* nombre_pokemon;
			//uint32_t  tamanio_nombre;
			//uint32_t posicion_x;
			//uint32_t posicion_y;
		//}cola_CATCH_POKEMON;

	case CATCH_POKEMON: {

		memcpy(buffer+desplazamiento,&catch_pokemon->id_mensaje,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer+desplazamiento,&catch_pokemon->tamanio_nombre,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer+desplazamiento,catch_pokemon->nombre_pokemon,string_length(catch_pokemon->nombre_pokemon));
		desplazamiento += catch_pokemon->tamanio_nombre;
		memcpy(buffer+desplazamiento,&catch_pokemon->posicion_x,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer+desplazamiento,&catch_pokemon->posicion_y,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);

		break;
	}

		//typedef struct{
			//uint32_t id_mensaje;
			//uint32_t atrapo_pokemon;
		//}cola_CAUGHT_POKEMON;

	case CAUGHT_POKEMON: {



=======
				}

	case APPEARED_POKEMON: {

				memcpy(buffer+desplazamiento,&appeared_pokemon->id_mensaje,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				memcpy(buffer+desplazamiento,&appeared_pokemon->tamanio_nombre,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				memcpy(buffer+desplazamiento,appeared_pokemon->nombre_pokemon,string_length(appeared_pokemon->nombre_pokemon));
				desplazamiento += appeared_pokemon->tamanio_nombre;
				memcpy(buffer+desplazamiento,&appeared_pokemon->posicion_x,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				memcpy(buffer+desplazamiento,&appeared_pokemon->posicion_y,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);

				break;
				}


	case CATCH_POKEMON: {

				memcpy(buffer+desplazamiento,&catch_pokemon->id_mensaje,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				memcpy(buffer+desplazamiento,&catch_pokemon->tamanio_nombre,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				memcpy(buffer+desplazamiento,catch_pokemon->nombre_pokemon,string_length(catch_pokemon->nombre_pokemon));
				desplazamiento += catch_pokemon->tamanio_nombre;
				memcpy(buffer+desplazamiento,&catch_pokemon->posicion_x,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				memcpy(buffer+desplazamiento,&catch_pokemon->posicion_y,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);

				break;
			}

	case CAUGHT_POKEMON: {

>>>>>>> origin/master
				memcpy(buffer+desplazamiento,&caug_poke->id_mensaje,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				memcpy(buffer+desplazamiento,&caug_poke->atrapo_pokemon,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);

				break;
<<<<<<< HEAD
	}

		//typedef struct{
			//uint32_t id_mensaje;
			//char* nombre_pokemon;
			//uint32_t  tamanio_nombre;
		//}cola_GET_POKEMON;

	case GET_POKEMON: {



		memcpy(buffer+desplazamiento,&get_pokemon->id_mensaje,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer+desplazamiento,&get_pokemon->tamanio_nombre,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer+desplazamiento,get_pokemon->nombre_pokemon,string_length(get_pokemon->nombre_pokemon));
		desplazamiento += get_pokemon->tamanio_nombre;
		break;
	}

		//typedef struct{
			//uint32_t id_mensaje;
			//char* nombre_pokemon;
			//uint32_t  tamanio_nombre;
			//uint32_t cantidad;
			//t_list* lista_posiciones;
		//}cola_LOCALIZED_POKEMON;

	case LOCALIZED_POKEMON: {

		int desplazamiento = 0;
		memcpy(buffer+desplazamiento,&localized_pokemon->id_mensaje,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer+desplazamiento,&localized_pokemon->tamanio_nombre,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer+desplazamiento,localized_pokemon->nombre_pokemon,string_length(localized_pokemon->nombre_pokemon));
		desplazamiento += localized_pokemon->tamanio_nombre;
		memcpy(buffer+desplazamiento,&localized_pokemon->cantidad,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		int tamLista = list_size(localized_pokemon->lista_posiciones);
		for (int i = 0 ; i < tamLista ; i++){
		int elemento = list_get(localized_pokemon->lista_posiciones,i) ;
		memcpy(buffer+desplazamiento,&elemento,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		}
		break;
	}
=======
			}

	case GET_POKEMON: {
				memcpy(buffer+desplazamiento,&get_pokemon->id_mensaje,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				memcpy(buffer+desplazamiento,&get_pokemon->tamanio_nombre,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				memcpy(buffer+desplazamiento,get_pokemon->nombre_pokemon,string_length(get_pokemon->nombre_pokemon));
				desplazamiento += get_pokemon->tamanio_nombre;
				break;
			}

	case LOCALIZED_POKEMON: {

				int desplazamiento = 0;
				memcpy(buffer+desplazamiento,&localized_pokemon->id_mensaje,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				memcpy(buffer+desplazamiento,&localized_pokemon->tamanio_nombre,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				memcpy(buffer+desplazamiento,localized_pokemon->nombre_pokemon,string_length(localized_pokemon->nombre_pokemon));
				desplazamiento += localized_pokemon->tamanio_nombre;
				memcpy(buffer+desplazamiento,&localized_pokemon->cantidad,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				int tamLista = list_size(localized_pokemon->lista_posiciones);
				for (int i = 0 ; i < tamLista ; i++){
				posicion* elemento = list_get(localized_pokemon->lista_posiciones,i) ;
				memcpy(buffer+desplazamiento,elemento,sizeof(uint32_t)*2);
				desplazamiento += (sizeof(uint32_t) * 2);
				}
				break;
			}

	case ACK: {

				memcpy(buffer+desplazamiento,&ack->ack,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				memcpy(buffer+desplazamiento,&ack->id_msj,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				memcpy(buffer+desplazamiento,&ack->token,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				break;
		}

	case SUSCRIPCION: {

			int desplazamiento = 0;
			memcpy(buffer+desplazamiento,&sucriptor->token,sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);

			for (int i = 0 ; i < list_size(sucriptor->cola_a_suscribir) ; i++){
			int elemento = list_get(sucriptor->cola_a_suscribir,i) ;
			memcpy(buffer+desplazamiento,&elemento,sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);
			}

			memcpy(buffer+desplazamiento,&sucriptor->modulo,sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);

			break;
			}
>>>>>>> origin/master

  } // fin switch head

	return buffer;
}

<<<<<<< HEAD
void * deserealizar_NEW_POKEMON (int head, void * buffer, int tamanio , cola_NEW_POKEMON * new_poke){

	int desplazamiento = 0;

							new_poke->nombre_pokemon = malloc(1);
							memcpy(&new_poke->id_mensaje,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);
							memcpy(&new_poke->tamanio_nombre,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);
							new_poke->nombre_pokemon = realloc(new_poke->nombre_pokemon,new_poke->tamanio_nombre);
							memcpy(&new_poke->nombre_pokemon,(buffer+desplazamiento),new_poke->tamanio_nombre);
							desplazamiento += new_poke->tamanio_nombre;
							memcpy(&new_poke->posicion_x,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);
							memcpy(&new_poke->posicion_y,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);
							memcpy(&new_poke->cantidad,(buffer+desplazamiento),sizeof(uint32_t));
							free(new_poke->nombre_pokemon);
							//return new_poke;
=======
void * recibirACK(int fdEmisor ){

	int head ; int bufferTam ;

	recibirProtocolo(&head,&bufferTam,fdEmisor);

	void * mensajeSeriealizado = malloc(bufferTam);

	void * mensaje = malloc(bufferTam);

	recibirMensaje(fdEmisor , bufferTam , mensajeSeriealizado );

	memcpy(mensaje, mensajeSeriealizado, bufferTam);

	return mensaje;
}

void * deserealizar_NEW_POKEMON (int head, void * bufferNewP, int tamanio , cola_NEW_POKEMON * new_poke){

	int desplazamiento = 0;


							memcpy(&new_poke->id_mensaje,(bufferNewP+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);

							memcpy(&new_poke->tamanio_nombre,(bufferNewP+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);

							new_poke->nombre_pokemon = malloc(1);

							new_poke->nombre_pokemon = realloc(new_poke->nombre_pokemon,new_poke->tamanio_nombre+1);
							memcpy(new_poke->nombre_pokemon,(bufferNewP+desplazamiento),new_poke->tamanio_nombre);
							desplazamiento += new_poke->tamanio_nombre;

							memcpy(&new_poke->posicion_x,(bufferNewP+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);

							memcpy(&new_poke->posicion_y,(bufferNewP+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);

							memcpy(&new_poke->cantidad,(bufferNewP+desplazamiento),sizeof(uint32_t));

							new_poke->nombre_pokemon[new_poke->tamanio_nombre] = '\0';
>>>>>>> origin/master

}

void * deserealizar_APPEARED_POKEMON (int head, void * buffer, int tamanio , cola_APPEARED_POKEMON * app_poke) {

	int desplazamiento = 0;

<<<<<<< HEAD
	setlocale(LC_ALL,"");

=======
>>>>>>> origin/master
			app_poke->nombre_pokemon = malloc(1);

			memcpy(&app_poke->id_mensaje,(buffer+desplazamiento),sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);

			memcpy(&app_poke->tamanio_nombre,(buffer+desplazamiento),sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);

			app_poke->nombre_pokemon = realloc(app_poke->nombre_pokemon,app_poke->tamanio_nombre);
			memcpy(app_poke->nombre_pokemon,(buffer+desplazamiento),app_poke->tamanio_nombre);
			desplazamiento += app_poke->tamanio_nombre;

			memcpy(&app_poke->posicion_x,(buffer+desplazamiento),sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);

			memcpy(&app_poke->posicion_y,(buffer+desplazamiento),sizeof(uint32_t));

			app_poke->nombre_pokemon[app_poke->tamanio_nombre] = '\0';

			//log_info(logger,"deserealizar CASE APPEARED_POKEMON -> ID:%d , POKEMON:%s , POSICIÓN X:%d , POSICIÓN Y: %d",app_poke->id_mensaje,app_poke->nombre_pokemon,app_poke->posicion_x,app_poke->posicion_y);

			//return app_poke;
}

void * deserealizar_CATCH_POKEMON (int head, void * buffer, int tamanio, cola_CATCH_POKEMON* cat_poke) {

	int desplazamiento = 0;

	cat_poke->nombre_pokemon = malloc(1);

			memcpy(&cat_poke->id_mensaje,(buffer+desplazamiento),sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);

			memcpy(&cat_poke->tamanio_nombre,(buffer+desplazamiento),sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);

<<<<<<< HEAD
			cat_poke->nombre_pokemon = realloc(cat_poke->nombre_pokemon,cat_poke->tamanio_nombre);
=======
			cat_poke->nombre_pokemon = realloc(cat_poke->nombre_pokemon,1 + cat_poke->tamanio_nombre);
>>>>>>> origin/master
			memcpy(cat_poke->nombre_pokemon,(buffer+desplazamiento),cat_poke->tamanio_nombre);
			desplazamiento += cat_poke->tamanio_nombre;

			memcpy(&cat_poke->posicion_x,(buffer+desplazamiento),sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);

			memcpy(&cat_poke->posicion_y,(buffer+desplazamiento),sizeof(uint32_t));

			cat_poke->nombre_pokemon[cat_poke->tamanio_nombre] = '\0';

			//return cat_poke;
}

void * deserealizar_CAUGHT_POKEMON (int head, void * buffer, int tamanio , cola_CAUGHT_POKEMON* cau_poke) {

	int desplazamiento = 0;
							memcpy(&cau_poke->id_mensaje,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);

							memcpy(&cau_poke->atrapo_pokemon,(buffer+desplazamiento),sizeof(uint32_t));

							return cau_poke;
}

void * deserealizar_GET_POKEMON (int head, void * buffer, int tamanio , cola_GET_POKEMON * get_poke) {

	int desplazamiento = 0;

				get_poke->nombre_pokemon = malloc(1);

							memcpy(&get_poke->id_mensaje,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);

							memcpy(&get_poke->tamanio_nombre,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);

<<<<<<< HEAD
							get_poke->nombre_pokemon = realloc(get_poke->nombre_pokemon,get_poke->tamanio_nombre);
=======
							get_poke->nombre_pokemon = realloc(get_poke->nombre_pokemon,1 + get_poke->tamanio_nombre);
>>>>>>> origin/master
							memcpy(get_poke->nombre_pokemon,(buffer+desplazamiento),get_poke->tamanio_nombre);

							get_poke->nombre_pokemon[get_poke->tamanio_nombre] = '\0';

							//return get_poke;
}

<<<<<<< HEAD
void * deserealizar_LOCALIZED_POKEMON (int head, void * buffer, int tamanio , cola_LOCALIZED_POKEMON * loc_poke_des) {

	int desplazamiento = 0;


	 	//int desplazamiento = 0;
		//memcpy(buffer+desplazamiento,&localized_pokemon->id_mensaje,sizeof(uint32_t));
		//desplazamiento += sizeof(uint32_t);
		//memcpy(buffer+desplazamiento,&localized_pokemon->tamanio_nombre,sizeof(uint32_t));
		//desplazamiento += sizeof(uint32_t);
		//memcpy(buffer+desplazamiento,&localized_pokemon->nombre_pokemon,string_length(localized_pokemon->nombre_pokemon));
		//desplazamiento += localized_pokemon->tamanio_nombre;
		//memcpy(buffer+desplazamiento,&localized_pokemon->cantidad,sizeof(uint32_t));
		//desplazamiento += sizeof(uint32_t);
		//memcpy(buffer+desplazamiento,&localized_pokemon->lista_posiciones,sizeof(uint32_t) * list_size(localized_pokemon->lista_posiciones));
		//desplazamiento += sizeof(uint32_t);


	loc_poke_des->nombre_pokemon = malloc(1);
	loc_poke_des->lista_posiciones = list_create();
=======


void deserealizar_ACK(int head, void * buffer, int tamanio , respuesta_ACK * ack ){

	int desplazamiento = 0;

							memcpy(&ack->ack,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);

							memcpy(&ack->id_msj,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);

							memcpy(&ack->token,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);
}


void * deserealizar_LOCALIZED_POKEMON (int head, void * buffer, int tamanio , cola_LOCALIZED_POKEMON * loc_poke_des) {

	int desplazamiento = 0;

	/*
		int desplazamiento = 0;
		memcpy(buffer+desplazamiento,&localized_pokemon->id_mensaje,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer+desplazamiento,&localized_pokemon->tamanio_nombre,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer+desplazamiento,localized_pokemon->nombre_pokemon,string_length(localized_pokemon->nombre_pokemon));
		desplazamiento += localized_pokemon->tamanio_nombre;
		memcpy(buffer+desplazamiento,&localized_pokemon->cantidad,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		int tamLista = list_size(localized_pokemon->lista_posiciones);
		for (int i = 0 ; i < tamLista ; i++){
		int elemento = list_get(localized_pokemon->lista_posiciones,i) ;
		memcpy(buffer+desplazamiento,&elemento,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
	*/
>>>>>>> origin/master

							memcpy(&loc_poke_des->id_mensaje,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);

							memcpy(&loc_poke_des->tamanio_nombre,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);

<<<<<<< HEAD
							 	//cat_poke->nombre_pokemon = realloc(cat_poke->nombre_pokemon,cat_poke->tamanio_nombre);
								//memcpy(cat_poke->nombre_pokemon,(buffer+desplazamiento),cat_poke->tamanio_nombre);
								//desplazamiento += cat_poke->tamanio_nombre;
=======
							loc_poke_des->nombre_pokemon = malloc(1);
>>>>>>> origin/master

							loc_poke_des->nombre_pokemon = realloc(loc_poke_des->nombre_pokemon,loc_poke_des->tamanio_nombre);
							memcpy(loc_poke_des->nombre_pokemon,(buffer+desplazamiento),loc_poke_des->tamanio_nombre);
							desplazamiento += loc_poke_des->tamanio_nombre;

							memcpy(&loc_poke_des->cantidad,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);

<<<<<<< HEAD
=======
							loc_poke_des->lista_posiciones = list_create();

>>>>>>> origin/master
							int cantidadElementos =  ( tamanio - sizeof(uint32_t) - sizeof(uint32_t) - sizeof(uint32_t) - loc_poke_des->tamanio_nombre ) / sizeof(uint32_t) ;

							for (int i = 0 ; i < cantidadElementos ; i++){
							int aux = 0;
<<<<<<< HEAD
							memcpy(&aux,buffer+desplazamiento,sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);
=======
							memcpy(&aux,buffer+desplazamiento,(3*sizeof(uint32_t)));
							desplazamiento += (3 * sizeof(uint32_t));
>>>>>>> origin/master
							list_add(loc_poke_des->lista_posiciones,aux);
							}

							loc_poke_des->nombre_pokemon[loc_poke_des->tamanio_nombre] = '\0';

							//return loc_poke;
}

int calcularTamanioMensaje(int head, void* mensaje){

	int tamanio = 0 ;

<<<<<<< HEAD
	if ( head == CAUGHT_POKEMON){
		tamanio = sizeof(uint32_t) + sizeof(uint32_t);
		return tamanio;

=======
	if ( head == CAUGHT_POKEMON) {
		tamanio = sizeof(uint32_t) + sizeof(uint32_t);
		return tamanio;
	}

	if ( head == ACK) {
		tamanio = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);
		return tamanio;
>>>>>>> origin/master
	}

	cola_NEW_POKEMON * new_poke = (cola_NEW_POKEMON *) mensaje;
	cola_LOCALIZED_POKEMON * loc_poke = (cola_LOCALIZED_POKEMON *) mensaje;
<<<<<<< HEAD
=======
	suscriptor * laSus = (suscriptor *) mensaje;
>>>>>>> origin/master

	tamanio = tamanio + strlen(new_poke->nombre_pokemon) ;

	switch(head){


		case NEW_POKEMON: {

			tamanio = tamanio + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) ;
			break;
		}

		case APPEARED_POKEMON:  case CATCH_POKEMON: {
			tamanio = tamanio + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);
			break;
		}
		case GET_POKEMON: {
			tamanio = tamanio + sizeof(uint32_t) + sizeof(uint32_t) ;
			break;
		}

<<<<<<< HEAD
		case LOCALIZED_POKEMON:{

			tamanio = tamanio + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + ( sizeof(uint32_t) * list_size(loc_poke->lista_posiciones) ) ;
=======
		case SUSCRIPCION: {
			tamanio = sizeof(uint32_t) + sizeof(uint32_t) + ( sizeof(uint32_t) * list_size(laSus->cola_a_suscribir));
			break;
		}

		case LOCALIZED_POKEMON:{

			/*
				typedef struct{
					char* nombre_pokemon;
					uint32_t id_mensaje;
					uint32_t  tamanio_nombre;
					uint32_t cantidad;
					t_list* lista_posiciones;
				}cola_LOCALIZED_POKEMON;
			 */

			tamanio = tamanio + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + ( ( sizeof(uint32_t) + sizeof(uint32_t) ) * loc_poke->cantidad ) ;
>>>>>>> origin/master
			break;
		}
	} // fin switch head

	return tamanio;
}

void* recibirProtocolo(int * head , int * bufferTam ,int fdEmisor ) {

<<<<<<< HEAD
	setlocale(LC_ALL,"");
=======
	//setlocale(LC_ALL,"");
>>>>>>> origin/master


	// Validar contra NULL al recibir en cada módulo.
	// Recibo primero el head:
	int recibido = recibirPorSocket(fdEmisor, head, sizeof(int));

	if (*head < 1 || *head > FIN_DEL_PROTOCOLO || recibido <= 0){ // DESCONEXIÓN
		//printf("Error al recibir mensaje.\n");
		return ERROR;
	}
	// Recibo ahora el tamaño del mensaje:

	recibido = recibirPorSocket(fdEmisor, bufferTam, sizeof(int));

	if (recibido <= 0) return ERROR;

	// Recibo por último el mensaje serealizado:
}

void * recibirMensaje(int fdEmisor , int bufferTam , void * mensaje ) {

	int recibido = recibirPorSocket(fdEmisor, mensaje, bufferTam);

}

int conectar_y_enviar(char * modulo , char * ipServer , int puertoServer, char *handShake , char * handShakeEsperado ,int head, void *mensaje , t_log * logger ,t_log * loggerCatedra ) {

	int fdServer = nuevoSocket();

	if ( conectarCon( fdServer ,ipServer,puertoServer,logger) )	{

		log_info(loggerCatedra,"Me conecte correctamente con el %s IP: %s y Puerto: %d",modulo,ipServer,puertoServer);

		handshake_cliente(fdServer,handShake,handShakeEsperado,logger);

		int desplazamiento = 0, tamanioMensaje, tamanioTotalAEnviar;

		if (head < 1 || head > FIN_DEL_PROTOCOLO) printf("Error al enviar mensaje.\n");
		// Calculo el tamaño del mensaje:

		tamanioMensaje = calcularTamanioMensaje(head, mensaje);
<<<<<<< HEAD

=======
		pthread_mutex_lock(&mxBuffer);
>>>>>>> origin/master
		// Serealizo el mensaje según el protocolo (me devuelve el mensaje empaquetado):
		void * mensajeSerealizado = serealizar(head, mensaje  , tamanioMensaje );

		// Lo que se envía es: head + tamaño del msj + el msj serializado:
		tamanioTotalAEnviar = sizeof(int) + sizeof(int) + tamanioMensaje;

		// Meto en el buffer las tres cosas:
		void *buffer = malloc(tamanioTotalAEnviar);
		memcpy(buffer + desplazamiento, &head, sizeof(int));
			desplazamiento += sizeof(int);
		memcpy(buffer + desplazamiento, &tamanioMensaje, sizeof(int));
			desplazamiento += sizeof(int);
		memcpy(buffer + desplazamiento, mensajeSerealizado, tamanioMensaje);

		// Envío la totalidad del paquete (lo contenido en el buffer):
		int enviados = enviarPorSocket(fdServer, buffer, tamanioTotalAEnviar);

		free(buffer);
<<<<<<< HEAD
=======
		free(mensajeSerealizado);
		pthread_mutex_unlock(&mxBuffer);
>>>>>>> origin/master

		//free(mensajeSerealizado);

		cerrarSocket(fdServer);

		return enviados;

	}else{
<<<<<<< HEAD
	log_info(loggerCatedra,"No se pudo realizar correctamente la conexión con el %s IP: %s y Puerto: %d",modulo,ipServer,puertoServer);
=======

	log_info(loggerCatedra,"No se pudo realizar correctamente la conexión con el %s IP: %s y Puerto: %d",modulo,ipServer,puertoServer);

>>>>>>> origin/master
	return ERROR;
	}
}

int aplicar_protocolo_enviar(int fdReceptor, int head, void *mensaje){

<<<<<<< HEAD
	int desplazamiento = 0, tamanioMensaje, tamanioTotalAEnviar;
=======
	int desplazamiento = 0, tamanioMensaje, tamanioTotalAEnviar = 0;
>>>>>>> origin/master

	if (head < 1 || head > FIN_DEL_PROTOCOLO) printf("Error al enviar mensaje.\n");
	// Calculo el tamaño del mensaje:


	tamanioMensaje = calcularTamanioMensaje(head, mensaje);
<<<<<<< HEAD

=======
	pthread_mutex_lock(&mxBuffer);
>>>>>>> origin/master
	// Serealizo el mensaje según el protocolo (me devuelve el mensaje empaquetado):
	void * mensajeSerealizado = serealizar(head, mensaje  , tamanioMensaje );

	// Lo que se envía es: head + tamaño del msj + el msj serializado:
	tamanioTotalAEnviar = sizeof(int) + sizeof(int) + tamanioMensaje;

	// Meto en el buffer las tres cosas:
<<<<<<< HEAD
	void *buffer = malloc(tamanioTotalAEnviar);
	memcpy(buffer + desplazamiento, &head, sizeof(int));
		desplazamiento += sizeof(int);
	memcpy(buffer + desplazamiento, &tamanioMensaje, sizeof(int));
		desplazamiento += sizeof(int);
	memcpy(buffer + desplazamiento, mensajeSerealizado, tamanioMensaje);

	// Envío la totalidad del paquete (lo contenido en el buffer):
	int enviados = enviarPorSocket(fdReceptor, buffer, tamanioTotalAEnviar);

	free(buffer);

	//free(mensajeSerealizado);

	return enviados;
}

*/
=======
	void* buffer2 = malloc(tamanioTotalAEnviar);
	memcpy(buffer2 + desplazamiento, &head, sizeof(int));
		desplazamiento += sizeof(int);
	memcpy(buffer2 + desplazamiento, &tamanioMensaje, sizeof(int));
		desplazamiento += sizeof(int);
	memcpy(buffer2 + desplazamiento, mensajeSerealizado, tamanioMensaje);

	// Envío la totalidad del paquete (lo contenido en el buffer):
	int enviados = enviarPorSocket(fdReceptor, buffer2, tamanioTotalAEnviar);

	free(buffer2);

	free(mensajeSerealizado);
	pthread_mutex_unlock(&mxBuffer);
	return enviados;
}
>>>>>>> origin/master
