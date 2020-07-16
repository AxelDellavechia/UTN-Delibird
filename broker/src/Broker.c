/*
 * Broker.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#include "Broker.h"

void dummyDump(){

	int a = 1 ;
	int b = 2;

	void * ptInicial = &a ;
	void * ptFinal = &b ;

		Particion * el_ejemplo;

		el_ejemplo = reservarMemoria(sizeof(Particion));

		el_ejemplo->libre = true ;
		el_ejemplo->tamano = 345 ;
		el_ejemplo->punteroInicial = ptInicial ;
		el_ejemplo->punteroFinal = ptFinal ;

		Particion * el_ejemplo2;

		el_ejemplo2 = reservarMemoria(sizeof(Particion));


		el_ejemplo2->colaAsignada = NEW_POKEMON ;
		el_ejemplo2->libre =false ;
		el_ejemplo2->punteroInicial = 0 ;
		el_ejemplo2->punteroFinal = 35 ;
		el_ejemplo2->tamano =456 ;
		el_ejemplo2->tiempoLRU = obtener_timestamp() ;

	 	lista_particiones = list_create() ;

		list_add(lista_particiones,el_ejemplo);

		list_add(lista_particiones,el_ejemplo2);
}

int main(){//int argc, char **argv) {

	//Inicio el Log
	iniciar_log();

	//Leo el Archivo de Configuracion

	config_File = reservarMemoria(sizeof(ConfigFile));

	leerArchivoDeConfiguracion(CONFIG_BROKER,logger);

	//Iniciar Estructuras
	iniciar_estructuras();

	//Capturar señal SIGUSR1 para el Dump de Memoria
	dummyDump();
	signal(SIGUSR1,dumpMemoria);

	//Multi-Hilos por conexion
	crearHilosBroker();

	//return EXIT_SUCCESS;
}
/*
void guardar_msj(int head, void * msj){
	int tamanio = calcularTamanioMensaje(head, msj);




}
*/
void guardar_msj(int head, int tamano, void * msj){
	if(tamano > config_File->TAMANO_MEMORIA && tamano < config_File->TAMANO_MINIMO_PARTICION)
	{
		//Particion * particion_select;
		if(strcmp(config_File->ALGORITMO_MEMORIA, "PARTICIONES") == 0)
		{
			if(config_File->ALGORITMO_PARTICION_LIBRE == "FF")
			{
				buscar_victima(head, tamano, First_Fit, msj);
			}
			else if(config_File->ALGORITMO_PARTICION_LIBRE == "BF")
			{
				buscar_victima(head, tamano, Best_Fit, msj);

			}
		}
		else if(strcmp(config_File->ALGORITMO_MEMORIA, "BS") == 0)
		{
			reservar_particion_bs(head, tamano, msj);
			//if(particion_select->tamano > tamano){/*LA PARTICION ES MAS GRANDE QUE LA NECESARIO SE PARTICIONA EN 2 (AGREGAR A LA LISTA DE PARTICIONES)*/}
			//if else(particion_select->tamano == tamano){/*SOLO SE MODIFICA LA PARTICION AL SER DEL MISMO TAMANIO*/}
			//else{/*ERROR, NO PUEDE SER EL TAMANIO MAYOR AL TAMANIO DE LA PARTICION*/}
		}
		else{/*ERROR CONFIG*/}
	}
	else{log_info(logger,"No se puede guardar el mensaje con el tamanio %d", tamano);}

}
/*
void reservar_particion_dinamica(int tamano, Mensaje mensaje){
	if(config_File->ALGORITMO_PARTICION_LIBRE == "FF")
	{
		buscar_victima(tamano, First_Fit);
	}
	else if(config_File->ALGORITMO_PARTICION_LIBRE == "BF")
	{
		buscar_victima(tamano, Best_Fit);

	}
}
*/
void reservar_particion_bs(int head, int tamano, void * mensaje){

}

void buscar_victima(int head, int tamano, Algoritmos Algoritmo){
	Particion* particion_libre;
	//PRIMERA VUELTA
	int contador_busquedas_fallidas = 0;
	_Bool encontro_particion;
	while(cantidad_fallidas <= contador_busquedas_fallidas && !encontro_particion)
	{
		switch (Algoritmo){
			case First_Fit: {
				encontro_particion = algoritmo_primer_ajuste(tamano);
				break;
			}
			case Best_Fit: {
				encontro_particion = algoritmo_mejor_ajuste(tamano);
				break;
			}
		}

		if(!encontro_particion){contador_busquedas_fallidas++;}
	}
	if(cantidad_fallidas == contador_busquedas_fallidas && !encontro_particion)
	{
		compactacion();
		//SEGUNDA VUELTA
		contador_busquedas_fallidas = 0;
		while(cantidad_fallidas <= contador_busquedas_fallidas && !encontro_particion)
		{
			switch (Algoritmo){
				case First_Fit: {
					algoritmo_primer_ajuste(tamano);
					break;
				}
				case Best_Fit: {
					algoritmo_mejor_ajuste(tamano);
					break;
				}
			}

			if(particion_libre == NULL){contador_busquedas_fallidas++;}
		}
		if(particion_libre == NULL && cantidad_fallidas == contador_busquedas_fallidas)
		{
			eliminar_particion();
		}
	}
}

/*
Particion* algoritmo_primer_ajuste(int tamano){
	int cantidad_particiones = list_size(lista_particiones);
	for(int i = 1; i<= cantidad_particiones; i++){
		Particion* particion = list_get(lista_particiones, i);
		if(particion != NULL && particion->tamano >= tamano){return particion;}
	}
	return NULL;
}

Particion* algoritmo_primer_ajuste(int tamano){
	_Bool particion_libre(Particion* particion){return particion->libre && particion->tamano >= tamano;}
	return list_find(lista_particiones, (void*)particion_libre);
}
*/
_Bool algoritmo_primer_ajuste(int tamano){
	Particion * aux_particion;
	int index = 0;
	_Bool encontro_particion = false;
	_Bool particion_libre(Particion* particion){return particion->libre && particion->tamano >= tamano;}
	//pthread_rwlock_rdlock(mutex_lista_particiones);
	_Bool hay_particion_libre = list_any_satisfy(lista_particiones, (void*)particion_libre);
	int cantidad_particiones = list_size(lista_particiones);
	while(hay_particion_libre && !encontro_particion && cantidad_particiones > index)
	{
		aux_particion = list_get(lista_particiones, index);
		if(aux_particion->libre && aux_particion->tamano >= tamano){encontro_particion = true;}
		else{index++;}
	}

	//pthread_rwlock_unlock(mutex_lista_particiones);
	if(encontro_particion)
	{
		if(aux_particion->tamano > tamano)
		{
			//pthread_rwlock_wrlock(mutex_lista_particiones);

			//pthread_rwlock_unlock(mutex_lista_particiones);
			/*SI LA PARTICION ES MAS GRANDE QUE LA NECESARIO SE PARTICIONA EN 2 (AGREGAR A LA LISTA DE PARTICIONES)*/
		}
		else if(aux_particion->tamano == tamano)
		{
			//pthread_rwlock_wrlock(mutex_lista_particiones);
			//REEMPLAZO LA PARTCION LIBRE UBICADA EN INDEX CON LA NUEVA
			//pthread_rwlock_unlock(mutex_lista_particiones);
		}
		else{/*ERROR, NO PUEDE SER EL TAMANIO MAYOR AL TAMANIO DE LA PARTICION*/}
	}
	else{return false;}

}

/*
void algoritmo_mejor_ajuste(int tamano){
	int cantidad_particiones = list_size(lista_particiones);
	t_list* lista_particiones_ordenadas

}
*/

_Bool algoritmo_mejor_ajuste(int tamano){
	_Bool ordenar(Particion* particion1, Particion* particion2){return particion1->tamano < particion2->tamano;}
	//pthread_rwlock_rdlock(mutex_lista_particiones);
	t_list* lista_ordenada = lista_particiones;
	//pthread_rwlock_unlock(mutex_lista_particiones);
	list_sort(lista_ordenada, (void*)ordenar);
	_Bool particion_libre(Particion* particion){return particion->libre && particion->tamano >= tamano;}
	return list_find(lista_ordenada, (void*)particion_libre);
	free(lista_ordenada);
}

void compactacion(){}

void eliminar_particion(){
	if(strcmp(config_File->ALGORITMO_REEMPLAZO, "FIFO") == 0)
	{
		algoritmo_fifo();
	}
	else if(strcmp(config_File->ALGORITMO_REEMPLAZO, "LRU") == 0)
	{
		algoritmo_lru();
	}
	else{log_info(logger,"El algoritmo de reemplazo %d, no es valido",config_File->FRECUENCIA_COMPACTACION);}
}

void algoritmo_fifo()
{

}

void algoritmo_lru(){}

int dumpMemoria (int senial) {

	setlocale(LC_ALL,"");

	FILE * dump ;

	time_t raw ;

	time(&raw);

	struct tm * tiempoActual ;

	tiempoActual = localtime(&raw);

	char filename[PATH_MAX];

	strftime(filename,PATH_MAX,"dump_%d%m%Y_%X.txt",tiempoActual);

	dump = fopen(filename,"a");

	if (dump != NULL) {
		fprintf(dump,"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
		fprintf(dump,"Dump: %s \n",ctime(&raw));
		int i ;
		for ( i = 0 ; i < list_size(lista_particiones) ; i++) {
			Particion * actual = list_get(lista_particiones,i) ;
			if ( actual->libre ) {
				fprintf(dump,"Partición %d: %p - %p		[L]		Size: %db  \n",i,actual->punteroInicial,actual->punteroFinal,actual->tamano);
			} else {
				char * colaNombre ;
				colaNombre = strdup( tipoMsjIntoToChar(actual->colaAsignada) ) ;
				fprintf(dump,"Partición %d: %d - %d		[X]		Size: %db		LRU: %d		Cola: %s		ID: %d\n",i,actual->punteroInicial,actual->punteroFinal,actual->tamano,actual->tiempoLRU,colaNombre,actual->colaAsignada);
				free(colaNombre);
			}

			free(actual);
		}
		fprintf(dump,"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
	} else {
		printf("No se puede abrir el archivo\n");
		return -1 ;
	}
	fclose(dump);
	return 0 ;
}
