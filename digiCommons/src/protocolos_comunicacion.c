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

int validar_servidor(char *id , char* mensajeEsperado,t_log* logger) {
	if(strcmp(id, mensajeEsperado) == 0 ) {
		log_info(logger,"Servidor aceptado.\n");
		return TRUE;
	} else {
		log_info(logger,"Servidor rechazado.\n");
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
		log_info(logger, "Alguien se conectó\n");

		return TRUE;
	}
}

void handshake(int sockClienteDe, char *mensajeEnviado , char *mensajeEsperado,t_log* logger) {

		int handshake_esperado = (int) malloc(strlen(mensajeEsperado) + 1);

		char *buff =  handshake_esperado;

		int status = enviarPorSocket(sockClienteDe, mensajeEnviado, strlen(mensajeEnviado) + 1 );

		validar_recive(status, 1); // Es terminante ya que la conexión es con el servidor

		recibirPorSocket(sockClienteDe, buff, handshake_esperado);

		buff[handshake_esperado-1] = '\0';

		if( validar_servidor(buff , mensajeEsperado,logger) == FALSE) {
			free(buff);
			exit(ERROR);
		} else {
			log_info(logger,"Handshake recibido: %s\n", buff);
			free(buff);

		}
}

int conectarCon(int fdServer , char * ipServer , int portServer,t_log* logger) {

		log_info(logger,"fdSacServer:%d", fdServer);
		log_info(logger,"ipServer:%s", ipServer);
		log_info(logger,"portServer:%d", portServer);

		int conexion = conectarSocket(fdServer, ipServer, portServer);

		if(conexion == ERROR){
			log_info(logger,"[SOCKETS] No se pudo realizar la conexión entre el socket y el servidor.");
			return FALSE;
		}
		else{
			log_info(logger,"[SOCKETS] Se pudo realizar la conexión entre el socket y el servidor.");
			return TRUE;
		}

}

void * serealizar(int head, void * mensaje, int tamanio){

	void * buffer = NULL;

	int desplazamiento = 0;

	cola_NEW_POKEMON * new_poke = (cola_NEW_POKEMON *) mensaje;
	cola_LOCALIZED_POKEMON * loc_poke = (cola_LOCALIZED_POKEMON *) mensaje;
	cola_CAUGHT_POKEMON * caug_poke = (cola_CAUGHT_POKEMON * ) mensaje;


	switch(head) {

	memcpy(buffer+desplazamiento,&new_poke->id_mensaje,sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(buffer+desplazamiento,new_poke->nombre_pokemon,string_length(new_poke->nombre_pokemon));
	desplazamiento += string_length(new_poke->nombre_pokemon);

	case NEW_POKEMON: {
		/*
			typedef struct{
				uint32_t id_mensaje;
				char* nombre_pokemon;
				uint32_t  posicion_x;
				uint32_t posicion_y;
				uint32_t cantidad;
			}cola_NEW_POKEMON;
		*/
				memcpy(buffer+desplazamiento,&new_poke->posicion_x,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				memcpy(buffer+desplazamiento,&new_poke->posicion_y,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				memcpy(buffer+desplazamiento,&new_poke->cantidad,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				free(new_poke);
				break;

	}
	case APPEARED_POKEMON: {
		/*
			typedef struct{
				uint32_t id_mensaje;
				char* nombre_pokemon;
				uint32_t posicion_x;
				uint32_t posicion_y;
			}cola_APPEARED_POKEMON;
		*/

				memcpy(buffer+desplazamiento,&new_poke->posicion_x,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				memcpy(buffer+desplazamiento,&new_poke->posicion_y,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				free(new_poke);
				break;
	}
	case CATCH_POKEMON: {
		/*
			typedef struct{
				uint32_t id_mensaje;
				char* nombre_pokemon;
				uint32_t posicion_x;
				uint32_t posicion_y;
			}cola_CATCH_POKEMON;
		 */

		memcpy(buffer+desplazamiento,&new_poke->posicion_x,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		memcpy(buffer+desplazamiento,&new_poke->posicion_y,sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
		free(new_poke);
		break;
	}
	case CAUGHT_POKEMON: {
		/*
		  	typedef struct{
				uint32_t atrapo_pokemon;
			}cola_CAUGHT_POKEMON;
		 */

		buffer = NULL ;
		desplazamiento = 0 ;
memcpy(buffer+desplazamiento,&new_poke->posicion_x,sizeof(uint32_t));
desplazamiento += sizeof(uint32_t);
				memcpy(buffer+desplazamiento,&caug_poke->atrapo_pokemon,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				free(caug_poke);
				break;
	}
	case GET_POKEMON: {
		/*
			typedef struct{
				uint32_t id_mensaje;
				char* nombre_pokemon;
			}cola_GET_POKEMON;
		 */
				free(new_poke);
				break;
	}
	case LOCALIZED_POKEMON: {
		/*
			typedef struct{
				uint32_t id_mensaje;
				char* nombre_pokemon;
				uint32_t cantidad;
				t_list* lista_posiciones;
			}cola_LOCALIZED_POKEMON;
		 */
				memcpy(buffer+desplazamiento,&loc_poke->cantidad,sizeof(uint32_t));
				desplazamiento += sizeof(uint32_t);
				memcpy(buffer+desplazamiento,&loc_poke->lista_posiciones,sizeof(uint32_t) * list_size(loc_poke->lista_posiciones));
				desplazamiento += sizeof(uint32_t);
				free(loc_poke);
				break;
	}
	default: {
		buffer = malloc(tamanio);
		memcpy(buffer, mensaje, tamanio);
		break;

	}

  } // fin switch head
	return buffer;
}

void * deserealizar(int head, void * buffer, int tamanio){

	void * mensaje = NULL;

	int desplazamiento = 0;

	switch(head) {
		// CASE 1: El mensaje es un texto (char*)
		case NEW_POKEMON: {
			/*
				typedef struct{
					uint32_t id_mensaje;
					char* nombre_pokemon;
					uint32_t  posicion_x;
					uint32_t posicion_y;
					uint32_t cantidad;
				}cola_NEW_POKEMON;
			*/



			cola_NEW_POKEMON * new_poke = malloc(sizeof(cola_NEW_POKEMON));

							memcpy(&new_poke->id_mensaje,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);
							memcpy(&new_poke->nombre_pokemon,(buffer+desplazamiento),string_length(new_poke->nombre_pokemon));
							desplazamiento += string_length(new_poke->nombre_pokemon);
							memcpy(&new_poke->posicion_x,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);
							memcpy(&new_poke->posicion_y,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);
							memcpy(&new_poke->cantidad,(buffer+desplazamiento),sizeof(uint32_t));
							return (new_poke);
							break;

		}
		case APPEARED_POKEMON: {
			/*
				typedef struct{
					uint32_t id_mensaje;
					char* nombre_pokemon;
					uint32_t posicion_x;
					uint32_t posicion_y;
				}cola_APPEARED_POKEMON;
			*/

			cola_APPEARED_POKEMON * app_poke = malloc(sizeof(cola_APPEARED_POKEMON));

			memcpy(&app_poke->id_mensaje,(buffer+desplazamiento),sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);
			memcpy(&app_poke->nombre_pokemon,(buffer+desplazamiento),string_length(app_poke->nombre_pokemon));
			desplazamiento += string_length(app_poke->nombre_pokemon);
			memcpy(&app_poke->posicion_x,(buffer+desplazamiento),sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);
			memcpy(&app_poke->posicion_y,(buffer+desplazamiento),sizeof(uint32_t));
			return (app_poke);
			break;
		}
		case CATCH_POKEMON: {
			/*
				typedef struct{
					uint32_t id_mensaje;
					char* nombre_pokemon;
					uint32_t posicion_x;
					uint32_t posicion_y;
				}cola_CATCH_POKEMON;
			 */

			cola_CATCH_POKEMON * cat_poke = malloc(sizeof(cola_CATCH_POKEMON));

			memcpy(&cat_poke->id_mensaje,(buffer+desplazamiento),sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);
			memcpy(&cat_poke->nombre_pokemon,(buffer+desplazamiento),string_length(cat_poke->nombre_pokemon));
			desplazamiento += string_length(cat_poke->nombre_pokemon);
			memcpy(&cat_poke->posicion_x,(buffer+desplazamiento),sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);
			memcpy(&cat_poke->posicion_y,(buffer+desplazamiento),sizeof(uint32_t));
			return (cat_poke);
			break;
		}
		case CAUGHT_POKEMON: {
			/*
			  	typedef struct{
			  	uint32_t id_mensaje;
					uint32_t atrapo_pokemon;
				}cola_CAUGHT_POKEMON;
			 */

			cola_CAUGHT_POKEMON* cau_poke = malloc(sizeof(cola_CAUGHT_POKEMON));
							memcpy(&cau_poke->id_mensaje,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);
							memcpy(&cau_poke->atrapo_pokemon,(buffer+desplazamiento),sizeof(uint32_t));
							return (cau_poke);
							break;
		}
		case GET_POKEMON: {
			/*
				typedef struct{
					uint32_t id_mensaje;
					char* nombre_pokemon;
				}cola_GET_POKEMON;
			 */

			cola_GET_POKEMON* get_poke = malloc(sizeof(cola_GET_POKEMON));

							memcpy(&get_poke->id_mensaje,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);
							memcpy( &get_poke->nombre_pokemon,(buffer+desplazamiento),string_length(get_poke->nombre_pokemon) );
							return (get_poke);
							break;
		}
		case LOCALIZED_POKEMON: {
			/*
				typedef struct{
					uint32_t id_mensaje;
					char* nombre_pokemon;
					uint32_t cantidad;
					t_list* lista_posiciones;
				}cola_LOCALIZED_POKEMON;
			 */

			cola_LOCALIZED_POKEMON* loc_poke = malloc(sizeof(cola_LOCALIZED_POKEMON));

							memcpy(&loc_poke->id_mensaje,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);
							memcpy(&loc_poke->nombre_pokemon,(buffer+desplazamiento),string_length(loc_poke->nombre_pokemon));
							desplazamiento += string_length(loc_poke->nombre_pokemon);
							memcpy(&loc_poke->cantidad,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);
							memcpy(buffer+desplazamiento,&loc_poke->lista_posiciones,sizeof(uint32_t) * list_size(loc_poke->lista_posiciones));
							return (loc_poke);
							break;
		}
		default: {
			buffer = malloc(tamanio);
			memcpy(buffer, mensaje, tamanio);
			break;

		}

	  } // fin switch head
	return mensaje;
} // Se debe castear lo retornado (indicar el tipo de dato que debe matchear con el void*)


int calcularTamanioMensaje(int head, void* mensaje){

	int tamanio;

	cola_NEW_POKEMON * new_poke = (cola_NEW_POKEMON *) mensaje;
	cola_LOCALIZED_POKEMON * loc_poke = (cola_LOCALIZED_POKEMON *) mensaje;

	tamanio = + string_length(new_poke->nombre_pokemon) ;

	switch(head){


		case NEW_POKEMON: {
			tamanio =+ sizeof(uint32_t) * 4  ;
			break;
		}

		case APPEARED_POKEMON: case CAUGHT_POKEMON: case CATCH_POKEMON: {
			tamanio =+ sizeof(uint32_t) * 3 ;
			break;
		}

		case GET_POKEMON: {
			tamanio = + sizeof(uint32_t) ;
			break;
		}

		case LOCALIZED_POKEMON:{
			tamanio = sizeof(uint32_t) * 2 + string_length(loc_poke->nombre_pokemon) + (sizeof(uint32_t) * list_size(loc_poke->lista_posiciones)) ;
			break;
		}

		default: {
			tamanio = strlen((char*) mensaje) + 1;
			break;
		}

	} // fin switch head

	free(new_poke);
	free(loc_poke);
	return tamanio;
}

void* aplicar_protocolo_recibir(int fdEmisor, int* head){

	// Validar contra NULL al recibir en cada módulo.
	// Recibo primero el head:
	int recibido = recibirPorSocket(fdEmisor, head, INT);

	if (*head < 1 || *head > FIN_DEL_PROTOCOLO || recibido <= 0){ // DESCONEXIÓN
		//printf("Error al recibir mensaje.\n");
		return NULL;
	}

	// Recibo ahora el tamaño del mensaje:
	int* tamanioMensaje = malloc(INT);
	recibido = recibirPorSocket(fdEmisor, tamanioMensaje, INT);
		if (recibido <= 0) return NULL;
	// Recibo por último el mensaje serealizado:
	void* mensaje = malloc(*tamanioMensaje);
	recibido = recibirPorSocket(fdEmisor, mensaje, *tamanioMensaje);
		if (recibido <= 0) return NULL;
	// Deserealizo el mensaje según el protocolo:
	void* buffer = deserealizar(*head, mensaje, *tamanioMensaje);

	free(tamanioMensaje); tamanioMensaje = NULL;
	free(mensaje); mensaje = NULL;

	return buffer;
}

int aplicar_protocolo_enviar(int fdReceptor, int head, void *mensaje){

	int desplazamiento = 0, tamanioMensaje, tamanioTotalAEnviar;

	if (head < 1 || head > FIN_DEL_PROTOCOLO) printf("Error al enviar mensaje.\n");
	// Calculo el tamaño del mensaje:
	tamanioMensaje = calcularTamanioMensaje(head, mensaje);

	// Serealizo el mensaje según el protocolo (me devuelve el mensaje empaquetado):
	void *mensajeSerealizado = serealizar(head, mensaje, tamanioMensaje);

	// Lo que se envía es: head + tamaño del msj + el msj serializado:
	tamanioTotalAEnviar = 2*INT + tamanioMensaje;

	// Meto en el buffer las tres cosas:
	void *buffer = malloc(tamanioTotalAEnviar);
	memcpy(buffer + desplazamiento, &head, INT);
		desplazamiento += INT;
	memcpy(buffer + desplazamiento, &tamanioMensaje, INT);
		desplazamiento += INT;
	memcpy(buffer + desplazamiento, mensajeSerealizado, tamanioMensaje);

	// Envío la totalidad del paquete (lo contenido en el buffer):
	int enviados = enviarPorSocket(fdReceptor, buffer, tamanioTotalAEnviar);

	free(mensajeSerealizado); mensajeSerealizado = NULL;
	free(buffer); buffer = NULL;

	return enviados;
}
