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

		el_ejemplo = malloc(sizeof(Particion));

		el_ejemplo->libre = true ;
		el_ejemplo->tamano = 345 ;
		el_ejemplo->punteroInicial = ptInicial ;
		el_ejemplo->punteroFinal = ptFinal ;

		Particion * el_ejemplo2;

		el_ejemplo2 = malloc(sizeof(Particion));


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

	config_File = malloc(sizeof(ConfigFile));

	leerArchivoDeConfiguracion(CONFIG_BROKER,logger);

	//Iniciar Estructuras
	iniciar_estructuras();
	iniciar_semaforos();

	//Capturar señal SIGUSR1 para el Dump de Memoria
	//dummyDump();
	signal(SIGUSR1,dumpMemoria);

	//Multi-Hilos por conexion
	crearHilosBroker();

	//return EXIT_SUCCESS;
}
void guardar_msj(int head, int tamano, void * msj){
	if(tamano < config_File->TAMANO_MEMORIA && tamano > config_File->TAMANO_MINIMO_PARTICION)
	{
		//Particion * particion_select;
		if(strcmp(config_File->ALGORITMO_MEMORIA, "PARTICIONES") == 0)
		{
			if(strcmp(config_File->ALGORITMO_PARTICION_LIBRE, "FF") == 0)
			{
				buscar_victima(head, tamano, First_Fit, msj);
			}
			else if(strcmp(config_File->ALGORITMO_PARTICION_LIBRE, "BF") == 0)
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

void reservar_particion_bs(int head, int tamano, void * msj)
{
	pthread_mutex_lock(&mutex_lista_particiones);
	pthread_mutex_lock(&mutex_memoria_cache);
	Particion * aux_particion = malloc(sizeof(Particion));
	int index = 0;
	_Bool encontro_particion = false;
	_Bool particion_libre(Particion* particion){return particion->libre && particion->tamano >= tamano;}
	_Bool hay_particion_libre = list_any_satisfy(lista_particiones, (void*)particion_libre);
	int cantidad_particiones = list_size(lista_particiones);
	while(hay_particion_libre && !encontro_particion && cantidad_particiones > index)
	{
		aux_particion = list_get(lista_particiones, index);
		if(aux_particion->libre && aux_particion->tamano >= tamano){encontro_particion = true;}
		else{index++;}
	}
	int ptro_inicial_particion = aux_particion->punteroInicial;
	int ptro_final_particion = aux_particion->punteroFinal;

	if(aux_particion->tamano == tamano)
	{
		aux_particion->tamano = tamano;
		aux_particion->libre = false;
		aux_particion->colaAsignada = head;
		aux_particion->tiempoLRU = (int)obtener_timestamp();
		void * buffer = serealizar(head, msj, tamano);
		memcpy(memoria_cache+aux_particion->punteroInicial, buffer, tamano);
		memcpy(&aux_particion->id_msj, memoria_cache+aux_particion->punteroInicial, sizeof(uint32_t));
		free(buffer);
	}
	else
	{
		int cont;
		int tamano_particion = aux_particion->tamano;
		int tamano_buddy = aux_particion->tamano;
		int aux_ptro_inicial, aux_ptro_final;
		for(cont=0; (tamano_buddy/2) > tamano; cont++)
		aux_particion->tamano = tamano_buddy;
		aux_particion->libre = false;
		aux_particion->colaAsignada = head;
		aux_particion->punteroFinal = aux_particion->punteroInicial + tamano_buddy;
		aux_particion->tiempoLRU = (int)obtener_timestamp();
		void * buffer = serealizar(head, msj, tamano);
		memcpy(memoria_cache+aux_particion->punteroInicial, buffer, tamano_buddy);
		memcpy(&aux_particion->id_msj, memoria_cache+aux_particion->punteroInicial, sizeof(uint32_t));
		aux_ptro_inicial = aux_particion->punteroFinal+1;

		for(int i; i < cont; i++)
		{
			Particion * nueva_particion = malloc(sizeof(Particion));
			nueva_particion->id_msj = 0;
			nueva_particion->colaAsignada = 0;
			nueva_particion->libre = true;
			nueva_particion->tamano = tamano_buddy;
			nueva_particion->punteroFinal = aux_ptro_inicial + tamano_buddy;
			nueva_particion->punteroInicial = aux_ptro_inicial;
			nueva_particion->tiempoLRU = 0;
			aux_ptro_inicial = nueva_particion->punteroFinal +1;
			tamano_buddy = tamano_buddy * 2;
			list_add_in_index(lista_particiones, index+1, nueva_particion);
		}
	}
	//memset(ptro_inicial_particion, '\0', ptro_final_particion - ptro_inicial_particion);
	pthread_mutex_unlock(&mutex_memoria_cache);
	pthread_mutex_unlock(&mutex_lista_particiones);
}


void buscar_victima(int head, int tamano, Algoritmos Algoritmo, void * msj){
	//PRIMERA VUELTA
	_Bool encontro_particion = false;

	while(!encontro_particion)
	{
		int cont_particiones_eliminadas = 0;
		while(cont_particiones_eliminadas < frecuencia_compactacion && !encontro_particion)
		{
			switch (Algoritmo){
				case First_Fit: {
					encontro_particion = algoritmo_primer_ajuste(head, tamano, msj);
					break;
				}
				case Best_Fit: {
					encontro_particion = algoritmo_mejor_ajuste(head, tamano, msj);
					break;
				}
				default:
					log_info(logger, "Algoritmo de Seleccion no reconocido");
					break;
			}
			if(!encontro_particion)
			{
				eliminar_particion();
				cont_particiones_eliminadas++;
			}
		}
		if(!encontro_particion)
		{
			compactacion();
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

_Bool algoritmo_primer_ajuste(int head, int tamano, void *msj){

	Particion * aux_particion = malloc(sizeof(Particion));

	int index = 0;
	_Bool encontro_particion = false;
	_Bool particion_libre(Particion* particion){return ( particion->libre && tamano <= particion->tamano ) ;}

	pthread_mutex_lock(&mutex_lista_particiones);
	_Bool hay_particion_libre = list_any_satisfy(lista_particiones, (void*)particion_libre);
	pthread_mutex_unlock(&mutex_lista_particiones);

	int cantidad_particiones = list_size(lista_particiones);
	while(hay_particion_libre && !encontro_particion && cantidad_particiones > index)
	{
		aux_particion = list_get(lista_particiones, index);
		if(aux_particion->libre && aux_particion->tamano >= tamano){encontro_particion = true;}
		else{index++;}
	}
	if(encontro_particion)
	{
		int desperdicio = aux_particion->tamano - tamano;
		if(desperdicio > 0)
		{
			//SE AGREGA UNA NUEVA PARTICION CON EL TAMANIO RESTANTE DE LA PARTICION ORIGINAL

			pthread_mutex_lock(&mutex_lista_particiones);

			Particion * nueva_particion = malloc(sizeof(Particion));

			aux_particion->tamano = tamano;
			aux_particion->libre = false;

			nueva_particion->punteroFinal = aux_particion->punteroFinal ;


			aux_particion->punteroFinal = aux_particion->punteroInicial + tamano - 1;
			aux_particion->colaAsignada = head;
			aux_particion->tiempoLRU = (int)obtener_timestamp();

			nueva_particion->punteroInicial = aux_particion->punteroFinal + 1 ;


			nueva_particion->tamano = desperdicio;
			nueva_particion->libre = true;
			nueva_particion->colaAsignada = 0;
			nueva_particion->tiempoLRU = 0;
			nueva_particion->id_msj = -1;

			list_remove(lista_particiones, index);
			list_add(lista_particiones, nueva_particion);
			list_add(lista_particiones, aux_particion);
			pthread_mutex_unlock(&mutex_lista_particiones);

			pthread_mutex_lock(&mutex_memoria_cache);

			void * buffer = serealizar(head, msj, tamano);

			memcpy(memoria_cache+aux_particion->punteroInicial, buffer, tamano);

			free(buffer);

			memcpy(&aux_particion->id_msj, memoria_cache+aux_particion->punteroInicial, sizeof(uint32_t));

		    //memset(memoria_cache+nueva_particion->punteroInicial, '\0', nueva_particion->tamano);
			pthread_mutex_unlock(&mutex_memoria_cache);

			//free(nueva_particion);
		}
		else
		{
			//REEMPLAZO LA PARTCION LIBRE UBICADA EN INDEX CON LA NUEVA

			//pthread_mutex_lock(&mutex_lista_particiones);

			aux_particion->tamano = tamano;
			aux_particion->libre = false;
			aux_particion->colaAsignada = head;
			aux_particion->tiempoLRU = (int)obtener_timestamp(); //obtener Timestamp

			void * buffer = serealizar(head, msj, tamano);

			pthread_mutex_lock(&mutex_memoria_cache);

			memcpy(memoria_cache+aux_particion->punteroInicial, buffer, tamano);
			memcpy(&aux_particion->id_msj, memoria_cache+aux_particion->punteroInicial, sizeof(uint32_t));

			pthread_mutex_unlock(&mutex_memoria_cache);

			free(buffer);

			pthread_mutex_lock(&mutex_lista_particiones);

			list_remove(lista_particiones, index);
			list_add_in_index(lista_particiones, index, aux_particion);
			log_info(loggerCatedra,"Se guardo el Mensaje con ID_MSJ:%d  Puntero Inicial:%d  Puntero Final:%d",aux_particion->id_msj, aux_particion->punteroInicial, aux_particion->punteroFinal);

			pthread_mutex_unlock(&mutex_lista_particiones);
			//pthread_mutex_unlock(&mutex_lista_particiones);
		}

		//free(aux_particion);

		return true;
	}
	else
	{
		return false;
	}

}

_Bool algoritmo_mejor_ajuste(int head, int tamano, void *msj){

	pthread_mutex_lock(&mutex_lista_particiones);
	_Bool particion_libre(Particion* particion){return particion->libre && particion->tamano >= tamano;}
	_Bool hay_particion_libre = list_any_satisfy(lista_particiones, (void*)particion_libre);
	if(hay_particion_libre){
		int bestPartIndex = 0;
		int desperdicio = config_File->TAMANO_MEMORIA;
		_Bool encontro_particion = false;

		for(int i = 0; i < list_size(lista_particiones) && encontro_particion == false;i++){
			Particion * particion = list_get(lista_particiones,i);
			if(particion->libre == true ){
				if(particion->tamano >= tamano){
					if(particion->tamano == tamano){
						desperdicio = 0;
						bestPartIndex = i;
						encontro_particion = true;
					}else{
						if((particion->tamano - tamano) < desperdicio ){
							bestPartIndex = i;
							desperdicio = (particion->tamano - tamano);
						}
					}
				}
			}
		}

		Particion* particionBestFit = list_get(lista_particiones,bestPartIndex);


		if(desperdicio > 0){
			Particion* nuevaPartLibre = malloc(sizeof(Particion));
			nuevaPartLibre->libre = true;
			nuevaPartLibre->tamano = (particionBestFit->tamano - tamano);
			nuevaPartLibre->punteroFinal = particionBestFit->punteroFinal;
			nuevaPartLibre->punteroInicial = particionBestFit->punteroInicial + tamano;
			nuevaPartLibre->tiempoLRU = 0; //VER SI ESTÁ BIEN ESTE CERO, NO LO USA EN PRIMER AJUSTE
			list_add(lista_particiones, nuevaPartLibre);

		}

		particionBestFit->tamano = tamano;
		particionBestFit->libre = false;
		particionBestFit->punteroFinal = particionBestFit->punteroInicial + tamano - 1;
		particionBestFit->colaAsignada = head;
		particionBestFit->tiempoLRU = (int)obtener_timestamp();

		void * buffer = serealizar(head, msj, tamano);

		pthread_mutex_lock(&mutex_memoria_cache);

		memcpy(memoria_cache+particionBestFit->punteroInicial, buffer, tamano);
		memcpy(&particionBestFit->id_msj, memoria_cache+particionBestFit->punteroInicial, sizeof(uint32_t));

		pthread_mutex_unlock(&mutex_memoria_cache);

		free(buffer);

		pthread_mutex_unlock(&mutex_lista_particiones);

		log_info(loggerCatedra,"Se guardo el Mensaje con ID_MSJ:%d  Puntero Inicial:%d  Puntero Final:%d",particionBestFit->id_msj, particionBestFit->punteroInicial, particionBestFit->punteroFinal);

		return true;


	}else{
		pthread_mutex_unlock(&mutex_lista_particiones);
		return false;
	}


	}


void compactacion(){


	//1- Ordeno la lista de particiones según ubicación en la memoria caché
	_Bool ordenar(Particion* a, Particion* b){return a->punteroInicial < b->punteroInicial;}
	list_sort(lista_particiones, (void*)ordenar);

	//2- Voy recorriendo la lista de particiones y desplazando el contenido de la memoria
	//y actualizando las nuevas ubicaciones.
	int desplazamiento = 0;
	int tamOcupado = 0;
	for(int i = 0; i<list_size(lista_particiones);i++){
	Particion* p = list_get(lista_particiones,i);
	if(p->libre == TRUE){
	//Si este bloque está libre, quiere decir que la partición que sigue
	//la puedo desplazar el tamaño de esta partición reservada.
	desplazamiento = desplazamiento + p->punteroFinal - p->punteroInicial +1;
	}else{
	if(desplazamiento > 0){
	//Voy copiando la memoria y desplazandola a su nueva ubicación
	pthread_mutex_lock(&mutex_memoria_cache);
	void * aux = malloc(p->tamano);
	memcpy(aux,memoria_cache+p->punteroInicial,p->tamano);
	memcpy(memoria_cache+(p->punteroInicial - desplazamiento),aux,p->tamano);
	free(aux);
	p->punteroInicial= p->punteroInicial - desplazamiento;
	p->punteroFinal= p->punteroFinal - desplazamiento;
	pthread_mutex_unlock(&mutex_memoria_cache);
	}
	}
	}

	//3- Elimino los bloques que tenía libre antes
	int j = 0;
	while(j< list_size(lista_particiones)){
	Particion* a = list_get(lista_particiones,j);
	if(a->libre == TRUE){
	list_remove(lista_particiones,j);
	//if(a->id_msj == 0) cantidad_particiones_liberadas++;
	}else{
	tamOcupado = tamOcupado + (a->punteroFinal - a->punteroInicial ) + 1;
	j++;
	}

	}

	//Armo el bloque que queda libre al final de la memoria
	//CAMBIAR EL 256 POR EL TAMAÑO DE LA CACHÉ QUE ESTÁ EN EL CONFIG
	if(tamOcupado < config_File->TAMANO_MEMORIA - 1){
	Particion * pFinalLibre = malloc(sizeof(Particion));
	pFinalLibre->libre = true;
	pFinalLibre->id_msj = 0;
	pFinalLibre->tiempoLRU = 0;
	pFinalLibre->colaAsignada = 0;
	pFinalLibre->punteroInicial = tamOcupado;
	pFinalLibre->punteroFinal = config_File->TAMANO_MEMORIA - 1;
	list_add(lista_particiones, pFinalLibre);
	pthread_mutex_lock(&mutex_memoria_cache);
	memset(memoria_cache + pFinalLibre->punteroInicial , '\0', pFinalLibre->punteroFinal - pFinalLibre->punteroInicial + 1);
	pthread_mutex_unlock(&mutex_memoria_cache);
	}

	compacte=true;


}

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
	pthread_mutex_lock(&mutex_lista_particiones);
	pthread_mutex_lock(&mutex_memoria_cache);
	pthread_mutex_lock(&mutex_puntero_reemplazo);
/*
	if(compacte) {
		puntero_reemplazo -= cantidad_particiones_liberadas;
		cantidad_particiones_liberadas = 0 ;
	}
*/
	Particion * particion_victima = list_get(lista_particiones, puntero_reemplazo);
	particion_victima->id_msj = 0;
	particion_victima->libre = true;
	particion_victima->colaAsignada = 0;
	particion_victima->tiempoLRU = 0;
	//memset(memoria_cache+particion_victima->punteroInicial, '\0', particion_victima->punteroFinal);
	consolidar(particion_victima);
	log_info(loggerCatedra,"Se elimino la particion asociada a Puntero Inicial:%d  Puntero Final:%d",particion_victima->punteroInicial, particion_victima->punteroFinal);
	if(puntero_reemplazo == ( list_size(lista_particiones) - 1 )) puntero_reemplazo=0;
	else puntero_reemplazo++;
	pthread_mutex_unlock(&mutex_puntero_reemplazo);
	pthread_mutex_unlock(&mutex_memoria_cache);
	pthread_mutex_unlock(&mutex_lista_particiones);
}

void algoritmo_lru()
{
	pthread_mutex_lock(&mutex_lista_particiones);
	pthread_mutex_lock(&mutex_memoria_cache);
	t_list * lista_ordenada = list_duplicate(lista_particiones);
	_Bool ordenarLRU(Particion* a, Particion* b){return a->tiempoLRU < b->tiempoLRU;}
	list_sort(lista_ordenada, (void*)ordenarLRU);
	Particion * particion_victima = list_get(lista_ordenada, 1);
	particion_victima->id_msj = 0;
	particion_victima->libre = TRUE;
	particion_victima->colaAsignada = 0;
	particion_victima->tiempoLRU = 0;
	//memset(memoria_cache+particion_victima->punteroInicial, '\0', particion_victima->tamano);
	consolidar(particion_victima);
	log_info(loggerCatedra,"Se elimino la particion asociada a Puntero Inicial:%d  Puntero Final:%d",particion_victima->punteroInicial, particion_victima->punteroFinal);
	pthread_mutex_unlock(&mutex_memoria_cache);
	pthread_mutex_unlock(&mutex_lista_particiones);
}


void consolidar(Particion * particion_liberada) {
	if(strcmp(config_File->ALGORITMO_REEMPLAZO, "FIFO") == 0)
	{
		particion_liberada = list_get(lista_particiones, puntero_reemplazo);
	}

	Particion * particion_consolidada = malloc(sizeof(Particion));
	particion_consolidada->colaAsignada = 0;
	particion_consolidada->id_msj = 0;
	particion_consolidada->libre = true;
	particion_consolidada->tiempoLRU = 0;

	int victima ;
	if(puntero_reemplazo == 0){
		Particion * particion_siguiente = malloc(sizeof(Particion));
		victima = puntero_reemplazo + 1 ;
		particion_siguiente = list_get(lista_particiones,victima );

		if(particion_siguiente->libre){
			particion_consolidada->punteroInicial = particion_liberada->punteroInicial;
			particion_consolidada->punteroFinal = particion_siguiente->punteroFinal;
			particion_consolidada->tamano = particion_siguiente->punteroFinal - particion_liberada->punteroInicial +1;
			list_remove(lista_particiones, victima ) ;
			list_remove(lista_particiones, puntero_reemplazo) ;
			list_add_in_index(lista_particiones, puntero_reemplazo , particion_consolidada);
			//pthread_mutex_lock(&mutex_memoria_cache);
			//memset(memoria_cache+particion_consolidada->punteroInicial, '\0', particion_consolidada->tamano);
			//pthread_mutex_unlock(&mutex_memoria_cache);
		}
	}

	int victima_sig = puntero_reemplazo + 1 ;

	int victima_ant = puntero_reemplazo-1 ;

	Particion * particion_anterior = malloc(sizeof(Particion));
	particion_anterior = list_get(lista_particiones, victima_ant);
	Particion * particion_siguiente = malloc(sizeof(Particion));
	particion_siguiente = list_get(lista_particiones, victima);

	if(puntero_reemplazo > 0  && puntero_reemplazo < ( list_size(lista_particiones) - 1 ) ){

		if(particion_anterior->libre && !particion_siguiente->libre) {

				particion_consolidada->punteroInicial = particion_anterior->punteroInicial;
				particion_consolidada->punteroFinal = particion_liberada->punteroFinal;
				particion_consolidada->tamano = particion_liberada->punteroFinal - particion_anterior->punteroInicial +1;

				list_remove(lista_particiones, victima_ant);
				list_add_in_index(lista_particiones, victima_ant, particion_consolidada);
				//pthread_mutex_lock(&mutex_memoria_cache);
				//memset(memoria_cache+particion_consolidada->punteroInicial, '\0', particion_consolidada->tamano);
				//pthread_mutex_unlock(&mutex_memoria_cache);


		} else if(!particion_anterior->libre && particion_siguiente->libre) {

			particion_consolidada->punteroInicial = particion_liberada->punteroInicial;
			particion_consolidada->punteroFinal = particion_siguiente->punteroFinal;
			particion_consolidada->tamano = particion_siguiente->punteroFinal - particion_liberada->punteroInicial + 1;

			list_remove(lista_particiones, victima_sig);
			list_add_in_index(lista_particiones, puntero_reemplazo, particion_consolidada);
			//pthread_mutex_lock(&mutex_memoria_cache);
			memset(memoria_cache+particion_consolidada->punteroInicial, '\0', particion_consolidada->tamano);
			//pthread_mutex_unlock(&mutex_memoria_cache);

		}
		else if(particion_anterior->libre && particion_siguiente->libre){
			particion_consolidada->punteroInicial = particion_anterior->punteroInicial;
			particion_consolidada->punteroFinal = particion_siguiente->punteroFinal;
			particion_consolidada->tamano = particion_siguiente->punteroFinal - particion_liberada->punteroInicial + 1;

			list_remove(lista_particiones, victima_ant);
			list_remove(lista_particiones, victima_sig);
			list_add_in_index(lista_particiones, victima_ant, particion_consolidada);
			//pthread_mutex_lock(&mutex_memoria_cache);
			memset(memoria_cache+particion_consolidada->punteroInicial, '\0', particion_consolidada->tamano);
			//pthread_mutex_unlock(&mutex_memoria_cache);
		}

	}

	if(puntero_reemplazo == (list_size(lista_particiones) - 1)){
		if(particion_anterior->libre) {

			particion_consolidada->punteroInicial = particion_anterior->punteroInicial;
			particion_consolidada->punteroFinal = particion_liberada->punteroFinal;
			particion_consolidada->tamano = particion_liberada->punteroFinal - particion_anterior->punteroInicial +1;

			list_remove(lista_particiones, victima_ant);
			list_add_in_index(lista_particiones, victima_ant, particion_consolidada);
			//pthread_mutex_lock(&mutex_memoria_cache);
			//memset(memoria_cache+particion_consolidada->punteroInicial, '\0', particion_consolidada->tamano);
			//pthread_mutex_unlock(&mutex_memoria_cache);


		}
	}


}

int dumpMemoria (int senial) {

	setlocale(LC_ALL,"");

	FILE * dump ;

	time_t raw ;

	time(&raw);

	struct tm * tiempoActual ;

	tiempoActual = localtime(&raw);

	char filename[PATH_MAX];

	strftime(filename,PATH_MAX,"../dump_%d%m%Y_%X.txt",tiempoActual);

	dump = fopen(filename,"a");

	if (dump != NULL) {
		fprintf(dump,"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
		fprintf(dump,"Dump: %s \n",ctime(&raw));
		int i ;
		for ( i = 0 ; i < list_size(lista_particiones) ; i++) {
			Particion * actual = list_get(lista_particiones,i) ;
			if ( actual->libre ) {
				fprintf(dump,"Partición %d: %d - %d		[L]		Size: %db  \n",i,actual->punteroInicial,actual->punteroFinal,actual->tamano);
			} else {
				char * colaNombre ;
				colaNombre = strdup( tipoMsjIntoToChar(actual->colaAsignada) ) ;
				fprintf(dump,"Partición %d: %d - %d		[X]		Size: %db		LRU: %d		Cola: %s	ID: %d\n",i,actual->punteroInicial,actual->punteroFinal,actual->tamano,actual->tiempoLRU,colaNombre,actual->colaAsignada);
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
	log_info(loggerCatedra,"Se realizo un Dump");
	return 0 ;
}
