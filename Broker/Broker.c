/*
 * Broker.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */
#include "Broker.h"


int main(){//int argc, char **argv) {

	//Inicio el Log
	iniciar_log();

	//Leo el Archivo de Configuracion
	leer_configFile();

	//Iniciar Estructuras
	iniciar_estructuras();

	//Multi-Hilos por conexion
	iniciar_servicio_broker();

	printf("Hola Mundo");
	return EXIT_SUCCESS;
}

void leer_configFile(){
	t_config *config;
		config = config_create("/BROKER.config");
		if (config != NULL) {
			config_File->TAMANO_MEMORIA = config_get_int_value(config, "TAMANO_MEMORIA");
			config_File->TAMANO_MINIMO_PARTICION = config_get_int_value(config, "TAMANO_MINIMO_PARTICION");
			config_File->ALGORITMO_MEMORIA = config_get_string_value(config, "ALGORITMO_MEMORIA");
			config_File->ALGORITMO_REEMPLAZO = config_get_string_value(config,"ALGORITMO_REEMPLAZO");
			config_File->ALGORITMO_PARTICION_LIBRE = config_get_string_value(config,"ALGORITMO_PARTICION_LIBRE");
			config_File->ALGORITMO_REEMPLAZO = config_get_string_value(config,"ALGORITMO_REEMPLAZO");
			config_File->IP_BROKER = config_get_string_value(config,"IP_BROKER");
			config_File->PUERTO_BROKER = config_get_int_value(config,"PUERTO_BROKER");
			config_File->FRECUENCIA_COMPACTACION = config_get_int_value(config,"FRECUENCIA_COMPACTACION");
			config_File->LOG_FILE = config_get_string_value(config,"LOG_FILE");
			config_destroy(config);
		}
}

void iniciar_log(){
	char *archivoLog = string_duplicate("Broker.log");
	logger = log_create(config_File->LOG_FILE, archivoLog, false, LOG_LEVEL_INFO);
	free(archivoLog);
	archivoLog = NULL;
}

void iniciar_Servicio_Broker(){
	int socket = nuevoSocket();
	uint32_t servidor = asociarSocket(socket, config_File->PUERTO_BROKER);
	while(true){
		esperar_conexion(servidor);
	}


}

void esperar_conexion(uint32_t servidor){
	uint32_t socket = aceptarConexionSocket(servidor);
	pthread_t cliente;
	pthread_create(&cliente, NULL, (void*)atender,(void*)socket);
	pthread_detach(cliente);
}
void atender(){

}

void iniciar_estructuras(){
	memoria_cache = malloc(config_File->TAMANO_MEMORIA);
	memset(memoria_cache, '\0', config_File->TAMANO_MEMORIA);
	//SE DEFINE MUTEX PARA DUMP DE MEMORIA CACHE
	pthread_mutex_init(mutex_memoria_cache, NULL);

}

void algoritmo_primer_ajuste(t_list* lista_particiones){

}
void algoritmo_mejor_ajuste(t_list* lista_particiones){}
void algoritmo_fifo(t_list* lista_particiones){}
void algoritmo_lru(t_list* lista_particiones){}
