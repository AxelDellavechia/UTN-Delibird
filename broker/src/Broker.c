/*
 * Broker.c
 *
 *  Created on: 11 abr. 2020
 *      Author: utnso
 */

#include "Broker.h"

void imprimirCache(Particion * laParti){

	switch( laParti->colaAsignada){
		case NEW_POKEMON :{

			cola_NEW_POKEMON  * ptro_new_poke = malloc(sizeof(cola_NEW_POKEMON)) ;

			deserealizar_mem_NEW_POKEMON(laParti ,ptro_new_poke);

			log_info(logger,"Saque de Cache NEW_POKEMON . POKEMON: %s  , CANTIDAD: %d  , CORDENADA X: %d , CORDENADA Y: %d ",ptro_new_poke->nombre_pokemon,ptro_new_poke->cantidad,ptro_new_poke->posicion_x,ptro_new_poke->posicion_y);

			free(ptro_new_poke->nombre_pokemon);
			free(ptro_new_poke);
			break;
		}
		case APPEARED_POKEMON :{

			cola_APPEARED_POKEMON  * app_poke = malloc(sizeof(cola_NEW_POKEMON)) ;

			deserealizar_mem_APPEARED_POKEMON(laParti,app_poke);

			log_info(logger,"Saque de Cache APPEARED_POKEMON . POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",app_poke->nombre_pokemon,app_poke->posicion_x,app_poke->posicion_y,app_poke);

			free(app_poke->nombre_pokemon);
			free(app_poke);
			break;
		}
		case CATCH_POKEMON :{

			cola_CATCH_POKEMON  * cath_poke = malloc(sizeof(cola_CATCH_POKEMON)) ;

			deserealizar_mem_CATCH_POKEMON (laParti,cath_poke);

			log_info(logger,"Saque de Cache CATCH_POKEMON . POKEMON: %s  , CORDENADA X: %d , CORDENADA Y: %d ",cath_poke->nombre_pokemon,cath_poke->posicion_x,cath_poke->posicion_y);

			free(cath_poke->nombre_pokemon);
			free(cath_poke);
			break;
		}
		case CAUGHT_POKEMON :{

			cola_CAUGHT_POKEMON  * cau_poke  = malloc(sizeof(cola_CAUGHT_POKEMON)) ;

			deserealizar_mem_CAUGHT_POKEMON(laParti,cau_poke);

			log_info(logger,"Saque de Cache CAUGHT_POKEMON . ID_MSJ: %d  , ATRAPO: %d",cau_poke->id_mensaje,cau_poke->atrapo_pokemon);

			free(cau_poke);
			break;
		}
		case GET_POKEMON :{

			cola_GET_POKEMON * get_poke  = malloc(sizeof(cola_GET_POKEMON)) ;

			deserealizar_mem_GET_POKEMON(laParti,get_poke);

			log_info(logger,"Saque de Cache GET_POKEMON . POKEMON: %s  , ID_MSJ: %d",get_poke->nombre_pokemon,get_poke->id_mensaje);

			free(get_poke->nombre_pokemon);
			free(get_poke);
			break;
		}
		case LOCALIZED_POKEMON :{

			cola_LOCALIZED_POKEMON  * loc_poke = malloc(sizeof(cola_LOCALIZED_POKEMON)) ;

			deserealizar_mem_LOCALIZED_POKEMON(laParti,laParti->tamano,loc_poke);

			for (int i = 0 ; i < list_size(loc_poke->lista_posiciones); i++) {
				posicion * pos = list_get(loc_poke->lista_posiciones,i) ;
				log_info(logger,"Saque de Cache LOCALIZED_POKEMON . POKEMON: %s  , CANTIDAD: %d , POSICIÓN X: %d , POSICIÓN Y: %d",loc_poke->nombre_pokemon,loc_poke->cantidad,pos->posicion_x,pos->posicion_y);
			}

			list_destroy(loc_poke->lista_posiciones);
			free(loc_poke->nombre_pokemon);
			free(loc_poke);
			break;
	}
	}
}

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

	if(tamano < config_File->TAMANO_MEMORIA && tamano >= config_File->TAMANO_MINIMO_PARTICION)
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
			aux_particion->tiempoLRU = obtener_timestamp();

			nueva_particion->punteroInicial = aux_particion->punteroFinal + 1 ;


			nueva_particion->tamano = desperdicio;
			nueva_particion->libre = true;
			nueva_particion->colaAsignada = 0;
			nueva_particion->tiempoLRU = LRU_MAX;
			nueva_particion->id_msj = 0;

			list_remove(lista_particiones, index);
			list_add(lista_particiones, nueva_particion);
			list_add(lista_particiones, aux_particion);

			void * buffer = serealizar(head, msj, tamano + sizeof(uint32_t));

			memcpy(&aux_particion->id_msj,buffer, sizeof(uint32_t));

			pthread_mutex_unlock(&mutex_lista_particiones);

			pthread_mutex_lock(&mutex_memoria_cache);

			memcpy(memoria_cache+aux_particion->punteroInicial, buffer + sizeof(uint32_t) , tamano);

			//imprimirCache(head , aux_particion);

			free(buffer);


		    //memset(memoria_cache+nueva_particion->punteroInicial, '\0', nueva_particion->tamano);

			log_info(loggerCatedra,"Se guardo el Mensaje con ID_MSJ:%d  Puntero Inicial:%d  Puntero Final:%d Tamaño: %d",aux_particion->id_msj, aux_particion->punteroInicial, aux_particion->punteroFinal, aux_particion->tamano);

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
			aux_particion->tiempoLRU = obtener_timestamp(); //obtener Timestamp

			void * buffer = serealizar(head, msj, tamano + sizeof(uint32_t));

			memcpy(&aux_particion->id_msj,buffer, sizeof(uint32_t));

			pthread_mutex_lock(&mutex_memoria_cache);

			memcpy(memoria_cache+aux_particion->punteroInicial, buffer + sizeof(uint32_t) , tamano);

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
			nuevaPartLibre->tiempoLRU = LRU_MAX; //VER SI ESTÁ BIEN ESTE CERO, NO LO USA EN PRIMER AJUSTE
			list_add(lista_particiones, nuevaPartLibre);

		}

		particionBestFit->tamano = tamano;
		particionBestFit->libre = false;
		particionBestFit->punteroFinal = particionBestFit->punteroInicial + tamano - 1;
		particionBestFit->colaAsignada = head;
		particionBestFit->tiempoLRU = obtener_timestamp();

		void * buffer = serealizar(head, msj, tamano + sizeof(uint32_t));

		memcpy(&particionBestFit->id_msj,buffer, sizeof(uint32_t));


		pthread_mutex_lock(&mutex_memoria_cache);

		memcpy(memoria_cache+particionBestFit->punteroInicial, buffer + sizeof(uint32_t) , tamano);


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
	if(p->libre){
	//Si este bloque está libre, quiere decir que la partición que sigue
	//la puedo desplazar el tamaño de esta partición reservada.
	desplazamiento = desplazamiento + p->punteroFinal - p->punteroInicial +1;
	}else{
	if(desplazamiento > 0){
	//Voy copiando la memoria y desplazandola a su nueva ubicación

	pthread_mutex_lock(&mutex_memoria_cache);

	//memcpy(memoria_cache+(p->punteroInicial - desplazamiento),
	//memoria_cache+p->punteroInicial,p->punteroFinal - p->punteroInicial+1);

	void * aux = malloc(p->tamano);

	memcpy(aux,memoria_cache+p->punteroInicial,p->tamano );

	memcpy(memoria_cache+(p->punteroInicial - desplazamiento),aux,p->tamano);
	free(aux);
	p->punteroInicial= p->punteroInicial - desplazamiento;
	p->punteroFinal= p->punteroFinal - desplazamiento;

	imprimirCache(p);

	pthread_mutex_unlock(&mutex_memoria_cache);
	}
	}
	}

	//3- Elimino los bloques que tenía libre antes
	int j = 0;
	//int cantidadElementos = 0;
	while(j< list_size(lista_particiones)){
	Particion* a = list_get(lista_particiones,j);
		if(a->libre == TRUE){
		list_remove(lista_particiones,j);
		//cantidadElementos++;
		//if(a->id_msj == 0) cantidad_particiones_liberadas++;
		}else{
		tamOcupado = tamOcupado + (a->punteroFinal - a->punteroInicial ) + 1;
		j++;
		}
	}

	//int cantidadLista = list_size(lista_particiones ) - 1 ;
	//if ( cantidadElementos >= cantidadLista ) puntero_reemplazo = 0 ;

	//Armo el bloque que queda libre al final de la memoria
	//CAMBIAR EL 256 POR EL TAMAÑO DE LA CACHÉ QUE ESTÁ EN EL CONFIG
	if(tamOcupado < config_File->TAMANO_MEMORIA - 1){
	Particion * pFinalLibre = malloc(sizeof(Particion));
	pFinalLibre->libre = true;
	pFinalLibre->id_msj = 0;
	pFinalLibre->tiempoLRU = LRU_MAX;
	pFinalLibre->colaAsignada = 0;
	pFinalLibre->punteroInicial = tamOcupado;
	pFinalLibre->punteroFinal = config_File->TAMANO_MEMORIA - 1;
	pFinalLibre->tamano = pFinalLibre->punteroFinal  - 	pFinalLibre->punteroInicial  + 1;
	list_add(lista_particiones, pFinalLibre);
	pthread_mutex_lock(&mutex_memoria_cache);
	memset(memoria_cache + pFinalLibre->punteroInicial , '\0', pFinalLibre->tamano);
	pthread_mutex_unlock(&mutex_memoria_cache);
	}

	//compacte=true;


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
	_Bool busquedaFiFo( Particion * laParti , Particion  * particion2){return laParti->tiempoLRU < particion2->tiempoLRU;}

	list_sort(lista_particiones, (void *)busquedaFiFo );

	Particion * particion_victima = list_get(lista_particiones,0);

	log_info(loggerCatedra,"Se elimino la particion asociada al ID_MSJ: %d Puntero Inicial:%d  Puntero Final:%d",particion_victima->id_msj,particion_victima->punteroInicial, particion_victima->punteroFinal);

	particion_victima->id_msj = 0;
	particion_victima->libre = true;
	particion_victima->colaAsignada = 0;
	particion_victima->tiempoLRU = LRU_MAX;

	//memset(memoria_cache+particion_victima->punteroInicial, '\0', particion_victima->punteroFinal);

	consolidar(particion_victima);

	//if(puntero_reemplazo == ( list_size(lista_particiones) - 1 )) puntero_reemplazo=0;
	//else puntero_reemplazo++;
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
	Particion * particion_victima = list_get(lista_ordenada, 0);

	log_info(loggerCatedra,"Se elimino la particion asociada al ID_MSJ: %d Puntero Inicial:%d  Puntero Final:%d",particion_victima->id_msj,particion_victima->punteroInicial, particion_victima->punteroFinal);

	particion_victima->id_msj = 0;
	particion_victima->libre = true;
	particion_victima->colaAsignada = 0;
	particion_victima->tiempoLRU = LRU_MAX;

	//memset(memoria_cache+particion_victima->punteroInicial, '\0', particion_victima->tamano);

	consolidar(particion_victima);

	pthread_mutex_unlock(&mutex_memoria_cache);
	pthread_mutex_unlock(&mutex_lista_particiones);
}


void consolidar(Particion * particion_liberada) {


	if(particion_liberada->punteroInicial > 0){

		_Bool busquedaFiFo( Particion * laParti ){return laParti->punteroFinal == particion_liberada->punteroInicial - 1;}

		Particion * particionAnterior = list_find(lista_particiones, (void *)busquedaFiFo );

		_Bool eliminar( Particion * laParti ){return laParti->punteroInicial == particionAnterior->punteroInicial && laParti->punteroFinal == particionAnterior->punteroFinal ;}

		if(particionAnterior->libre){

			particion_liberada->punteroInicial = particionAnterior->punteroInicial ;
			particion_liberada->tamano = particion_liberada->punteroFinal - particion_liberada->punteroInicial + 1;
			list_remove_by_condition(lista_particiones,(void*) eliminar);

		}

	}

	if( particion_liberada->punteroFinal < ( config_File->TAMANO_MEMORIA - 1 ) ){

		_Bool busquedaFiFo( Particion * laParti ){return laParti->punteroInicial == particion_liberada->punteroFinal + 1;}

		Particion * particionSiguiente = list_find(lista_particiones, (void *)busquedaFiFo );

		_Bool eliminar( Particion * laParti ){return laParti->punteroInicial == particionSiguiente->punteroInicial && laParti->punteroFinal == particionSiguiente->punteroFinal ;}


		if(particionSiguiente->libre){
			particion_liberada->punteroFinal = particionSiguiente->punteroFinal;
			particion_liberada->tamano = particion_liberada->punteroFinal - particion_liberada->punteroInicial + 1;
			list_remove_by_condition(lista_particiones,(void*) eliminar);
		}

	}

/*
	if(strcmp(config_File->ALGORITMO_REEMPLAZO, "FIFO") == 0)
	{
		particion_liberada = list_get(lista_particiones, puntero_reemplazo);
	}

	Particion * particion_consolidada = malloc(sizeof(Particion));
;

	int victima ;
	if(puntero_reemplazo == 0){
		Particion * particion_siguiente = malloc(sizeof(Particion));
		victima = puntero_reemplazo + 1 ;
		particion_siguiente = list_get(lista_particiones,victima );


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

	if(puntero_reemplazo != 0 && puntero_reemplazo == (list_size(lista_particiones) - 1)){
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
*/

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

		pthread_mutex_lock(&mutex_lista_particiones);
		pthread_mutex_lock(&mutex_memoria_cache);

		for ( i = 0 ; i < list_size(lista_particiones) ; i++) {
			Particion * actual = list_get(lista_particiones,i) ;
			if ( actual->libre ) {
				fprintf(dump,"Partición %d: %d - %d		[L]		Size: %d bytes  \n",i,actual->punteroInicial,actual->punteroFinal,actual->tamano);
			} else {
				fprintf(dump,"Partición %d: %d - %d		[X]		Size: %d bytes		LRU: %llu		Cola: %s	ID_MSJ: %d\n",
				i,actual->punteroInicial, actual->punteroFinal,
				actual->tamano,actual->tiempoLRU,tipoMsjIntoToChar(actual->colaAsignada),actual->id_msj);
			}
			//free(actual);
		}

		pthread_mutex_unlock(&mutex_memoria_cache);
		pthread_mutex_unlock(&mutex_lista_particiones);

		fprintf(dump,"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
	} else {
		printf("No se puede abrir el archivo\n");
		return -1 ;
	}
	fclose(dump);
	log_info(loggerCatedra,"Se realizo un Dump");
	return 0 ;
}
