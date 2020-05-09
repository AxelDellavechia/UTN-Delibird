#ifndef SRC_PROTOCOLOS_COMUNICACION_H_
#define SRC_PROTOCOLOS_COMUNICACION_H_

#include "sockets.h"
#include <commons/log.h>
#include "mensajeria.h"
#include <commons/string.h>
#include <locale.h>

#define INT (sizeof(int))

typedef enum {
	NEW_POKEMON = 1,
	APPEARED_POKEMON,
	CATCH_POKEMON,
	CAUGHT_POKEMON,
	GET_POKEMON,
	LOCALIZED_POKEMON,
	FIN_DEL_PROTOCOLO
} protocolo;

typedef enum {
	GAMECARD = 1,
	BROKER,
	TEAM
} modulos;

int validar_recive(int status, int modo);
int validar_servidor(char *id , char* mensajeEsperado,t_log* logger);
int validar_conexion(int ret, int modo,t_log* logger);
int handshake_servidor (int sockClienteDe, char *mensajeEnviado , char *mensajeEsperado,t_log* logger);
int handshake_cliente (int sockClienteDe, char *mensajeEnviado , char *mensajeEsperado,t_log* logger) ;
int conectarCon(int fdServer , char * ipServer , int portServer,t_log* logger);
void * serealizar(int head, void * mensaje, int tamanio);
void * deserealizar(int head, void * buffer, int tamanio);
int calcularTamanioMensaje(int head, void* mensaje);
void* aplicar_protocolo_recibir(int fdEmisor, int* head);
int aplicar_protocolo_enviar(int fdReceptor, int head, void *mensaje);

#endif /* SRC_PROTOCOLOS_COMUNICACION_H_ */
