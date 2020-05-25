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

int handshake_servidor (int sockClienteDe, char *mensajeEnviado , char *mensajeEsperado,t_log* logger) {

	enviarPorSocket(sockClienteDe, mensajeEnviado, string_length(mensajeEnviado));

	char *buff = (char*) malloc(string_length(mensajeEsperado));

	int status = recibirPorSocket(sockClienteDe, buff, string_length(mensajeEsperado));

	if( validar_recive(status, 0) ) { // El cliente envió un mensaje

		buff[string_length(mensajeEsperado)] = '\0';
		if (validar_servidor(buff , mensajeEsperado,logger)) {
			log_info(logger,"Hice el handshake y me respondieron: %s\n", buff);
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


void handshake_cliente (int sockClienteDe, char *mensajeEnviado , char *mensajeEsperado,t_log* logger) {

	char *buff = (char*) malloc(string_length(mensajeEsperado));

	int status = recibirPorSocket(sockClienteDe, buff, string_length(mensajeEsperado));

		validar_recive(status, 1); // Es terminante ya que la conexión es con el servidor

		buff[string_length(mensajeEsperado)] = '\0';
		if( ! validar_servidor(buff , mensajeEsperado,logger) ) {
			free(buff);
			exit(ERROR);
		} else {
			log_info(logger,"Handshake recibido: %s\n", buff);
			free(buff);
			enviarPorSocket(sockClienteDe, mensajeEnviado, string_length(mensajeEnviado));
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

	switch(head) {
	// CASE 1: El mensaje es un texto (char*)
	case NEW_POKEMON: case APPEARED_POKEMON: case CATCH_POKEMON: case CAUGHT_POKEMON: case GET_POKEMON:
	case LOCALIZED_POKEMON: {
		buffer = malloc(tamanio);
		memcpy(buffer, mensaje, tamanio);
		break;
	}

  } // fin switch head
	return buffer;
}

void * deserealizar(int head, void * buffer, int tamanio){

	void * mensaje = NULL;

	switch(head){
	// CASE 1: El mensaje es un texto (char*)
	case NEW_POKEMON: case APPEARED_POKEMON: case CATCH_POKEMON: case CAUGHT_POKEMON: case GET_POKEMON:
	case LOCALIZED_POKEMON: {
		mensaje = malloc(tamanio);
		memcpy(mensaje, buffer, tamanio);
		break;
	}

 }
	return mensaje;
} // Se debe castear lo retornado (indicar el tipo de dato que debe matchear con el void*)


int calcularTamanioMensaje(int head, void* mensaje){

	int tamanio;

	switch(head){
		// CASE 1: El mensaje es un texto (char*)
		case NEW_POKEMON: case APPEARED_POKEMON: case CATCH_POKEMON: case CAUGHT_POKEMON: case GET_POKEMON:
		case LOCALIZED_POKEMON:{
				tamanio = strlen((char*) mensaje) + 1;
				break;
			}

	} // fin switch head

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
