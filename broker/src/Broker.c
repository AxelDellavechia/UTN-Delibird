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
		el_ejemplo2->punteroInicial = ptInicial ;
		el_ejemplo2->punteroFinal = ptFinal ;
		el_ejemplo2->tamano =456 ;
		el_ejemplo2->tiempoLRU = config_File->FRECUENCIA_COMPACTACION ;

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

void reservar_particion_bs(int head, int tamano, void * mensaje){

}

void buscar_victima(int head, int tamano, Algoritmos Algoritmo, void * msj){
	Particion* particion_libre;
	//PRIMERA VUELTA
	int contador_busquedas_fallidas = 0;
	_Bool encontro_particion;
	while(cantidad_fallidas <= contador_busquedas_fallidas && !encontro_particion)
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
					encontro_particion = algoritmo_primer_ajuste(head, tamano, msj);
					break;
				}
				case Best_Fit: {
					encontro_particion = algoritmo_mejor_ajuste(head, tamano, msj);
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
_Bool algoritmo_primer_ajuste(int head, int tamano, void *msj){
	Particion * aux_particion;
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
	if(encontro_particion)
	{
		int desperdicio = aux_particion->tamano - tamano;
		if(desperdicio > 0)
		{
			//SE AGREGA UNA NUEVA PARTICION CON EL TAMANIO RESTANTE DE LA PARTICION ORIGINAL
			Particion * nueva_particion;
			nueva_particion->punteroInicial = aux_particion->punteroFinal + 1;
			nueva_particion->punteroFinal = nueva_particion->punteroFinal + tamano;
			nueva_particion->tamano = desperdicio;
			nueva_particion->libre = TRUE;

			aux_particion->tamano = tamano;
			aux_particion->libre = false;
			aux_particion->colaAsignada = head;
			aux_particion->tiempoLRU = 0;

			pthread_mutex_lock(&mutex_lista_particiones);
			list_remove(lista_particiones, index);
			list_add_in_index(lista_particiones, index+1, nueva_particion);
			pthread_mutex_unlock(&mutex_lista_particiones);
			void * buffer = serealizar(head, msj, tamano);
			pthread_mutex_lock(&mutex_memoria_cache);
			memcpy(memoria_cache+aux_particion->punteroInicial, buffer, tamano);
			memset(memoria_cache+nueva_particion->punteroInicial, '\0', memoria_cache+nueva_particion->punteroFinal);
			pthread_mutex_unlock(&mutex_memoria_cache);
			return true;
		}
		else
		{
			//REEMPLAZO LA PARTCION LIBRE UBICADA EN INDEX CON LA NUEVA
			aux_particion->tamano = tamano;
			aux_particion->libre = false;
			aux_particion->colaAsignada = head;
			aux_particion->tiempoLRU = 0; //obtener Timestamp
			pthread_mutex_lock(&mutex_lista_particiones);
			list_remove(lista_particiones, index);
			list_add_in_index(lista_particiones, index, aux_particion);
			pthread_mutex_unlock(&mutex_lista_particiones);
			void * buffer = serealizar(head, msj, tamano);
			pthread_mutex_lock(&mutex_memoria_cache);
			memcpy(memoria_cache+aux_particion->punteroInicial, buffer, tamano);
			pthread_mutex_unlock(&mutex_memoria_cache);
			return true;
		}
	}
	else{return false;}

}
_Bool algoritmo_mejor_ajuste(int head, int tamano, void *msj){
	_Bool ordenar(Particion* particion1, Particion* particion2){return particion1->tamano < particion2->tamano;}
	t_list* lista_ordenada = list_duplicate(lista_particiones);
	list_sort(lista_ordenada, (void*)ordenar);

	Particion * aux_particion;
	int index = 0;
	_Bool encontro_particion = false;
	_Bool particion_libre(Particion* particion){return particion->libre && particion->tamano >= tamano;}
	_Bool hay_particion_libre = list_any_satisfy(lista_particiones, (void*)particion_libre);
	int cantidad_particiones = list_size(lista_particiones);
	while(hay_particion_libre && !encontro_particion && cantidad_particiones > index)
	{
		aux_particion = list_get(lista_ordenada, index);
		if(aux_particion->libre && aux_particion->tamano >= tamano){encontro_particion = true;}
		else{index++;}
	}
	if(encontro_particion)
		{
			int desperdicio = aux_particion->tamano - tamano;
			if(desperdicio > 0)
			{
				//SE AGREGA UNA NUEVA PARTICION CON EL TAMANIO RESTANTE DE LA PARTICION ORIGINAL
				Particion * nueva_particion;
				nueva_particion->punteroInicial = aux_particion->punteroFinal + 1;
				nueva_particion->punteroFinal = nueva_particion->punteroFinal + tamano;
				nueva_particion->tamano = desperdicio;
				nueva_particion->libre = TRUE;

				aux_particion->tamano = tamano;
				aux_particion->libre = false;
				aux_particion->colaAsignada = head;
				aux_particion->tiempoLRU = 0;

				pthread_mutex_lock(&mutex_lista_particiones);
				list_remove(lista_particiones, index);
				list_add_in_index(lista_particiones, index+1, nueva_particion);
				pthread_mutex_unlock(&mutex_lista_particiones);
				void * buffer = serealizar(head, msj, tamano);
				pthread_mutex_lock(&mutex_memoria_cache);
				memcpy(memoria_cache+aux_particion->punteroInicial, buffer, tamano);
				memset(memoria_cache+nueva_particion->punteroInicial, '\0', memoria_cache+nueva_particion->punteroFinal);
				pthread_mutex_unlock(&mutex_memoria_cache);
				return true;
			}
			else
			{
				//REEMPLAZO LA PARTCION LIBRE UBICADA EN INDEX CON LA NUEVA
				aux_particion->tamano = tamano;
				aux_particion->libre = false;
				aux_particion->colaAsignada = head;
				aux_particion->tiempoLRU = 0; //obtener Timestamp
				pthread_mutex_lock(&mutex_lista_particiones);
				list_remove(lista_particiones, index);
				list_add_in_index(lista_particiones, index, aux_particion);
				pthread_mutex_unlock(&mutex_lista_particiones);
				void * buffer = serealizar(head, msj, tamano);
				pthread_mutex_lock(&mutex_memoria_cache);
				memcpy(memoria_cache+aux_particion->punteroInicial, buffer, tamano);
				pthread_mutex_unlock(&mutex_memoria_cache);
			}
		}
		else{return false;}
}

void compactacion(){
	//1- Ordeno la lista de particiones según ubicación en la memoria caché
	t_list * lista_ordenada = list_duplicate(lista_particiones);
	_Bool ordenar(Particion* a, Particion* b){return a->punteroInicial < b->punteroInicial;}
	list_sort(lista_ordenada, (void*)ordenar);

	//2- Voy recorriendo la lista de particiones y desplazando el contenido de la memoria
	//y actualizando las nuevas ubicaciones.
	int desplazamiento = 0;
	int tamOcupado = 0;
	for(int i = 0; i<list_size(lista_particiones);i++){
		Particion* p = list_get(lista_particiones,i);
		if(p->libre == TRUE)
		{
			//Si este bloque está libre, quiere decir que la partición que sigue
			//la puedo desplazar el tamaño de esta partición reservada.
			desplazamiento = desplazamiento + p->punteroFinal - p->punteroInicial +1;
		}else
		{
			if(desplazamiento > 0){
				//Voy copiando la memoria y desplazandola a su nueva ubicación
				memcpy(memoria_cache+(p->punteroInicial - desplazamiento),memoria_cache+p->punteroInicial,p->punteroFinal - p->punteroInicial+1);
				p->punteroInicial= p->punteroInicial - desplazamiento;
				p->punteroFinal= p->punteroFinal - desplazamiento;

				}
			}
	}

	//3- Elimino los bloques que tenía libre antes
	int j = 0;
	while(j< list_size(lista_particiones)){
		Particion* a = list_get(lista_particiones,j);
		if(a->libre == TRUE){
			list_remove(lista_particiones,j);
		}
		else
		{
			tamOcupado = tamOcupado + (a->punteroFinal - a->punteroInicial ) + 1;
			j++;
		}

	}

	//Armo el bloque que queda libre al final de la memoria
	//CAMBIAR EL 256 POR EL TAMAÑO DE LA CACHÉ QUE ESTÁ EN EL CONFIG
	if(tamOcupado < config_File->TAMANO_MEMORIA - 1){
		Particion pFinalLibre;
		pFinalLibre.libre = TRUE;
		pFinalLibre.punteroInicial = tamOcupado;
		pFinalLibre.punteroFinal = config_File->TAMANO_MEMORIA - 1;
		list_add(lista_particiones, &pFinalLibre);
		pthread_mutex_lock(&mutex_memoria_cache);
		memset(memoria_cache + pFinalLibre.punteroInicial , '\0', pFinalLibre.punteroFinal - pFinalLibre.punteroInicial + 1);
		pthread_mutex_unlock(&mutex_memoria_cache);
	}


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
	pthread_mutex_lock(&mutex_puntero_reemplazo);
	Particion * particion_victima;
	particion_victima = list_get(lista_particiones, puntero_reemplazo);
	particion_victima->libre = TRUE;
	particion_victima->id_msj = 0;
	pthread_mutex_lock(&mutex_memoria_cache);
	memset(memoria_cache+particion_victima->punteroInicial, '\0', particion_victima->punteroFinal);
	pthread_mutex_unlock(&mutex_memoria_cache);
	puntero_reemplazo += particion_victima->punteroFinal;
	pthread_mutex_unlock(&mutex_puntero_reemplazo);
}

void algoritmo_lru()
{
	t_list * lista_ordenada = list_duplicate(lista_particiones);
	_Bool ordenarLRU(Particion* a, Particion* b){return a->tiempoLRU < b->tiempoLRU;}
	list_sort(lista_ordenada, (void*)ordenarLRU);
	Particion * particion_victima = list_get(lista_ordenada, 1);
	particion_victima->libre = TRUE;
	particion_victima->id_msj = 0;
	pthread_mutex_lock(&mutex_memoria_cache);
	memset(memoria_cache+particion_victima->punteroInicial, '\0', particion_victima->punteroFinal);
	pthread_mutex_unlock(&mutex_memoria_cache);
}

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
				switch (actual->colaAsignada) {
					case NEW_POKEMON :
						colaNombre = strdup("NEW_POKEMON");
					break;
					case APPEARED_POKEMON :
						colaNombre = strdup("APPEARED_POKEMON");
					break;
					case CATCH_POKEMON :
						colaNombre = strdup("CATCH_POKEMON");
					break;
					case CAUGHT_POKEMON :
						colaNombre = strdup("CAUGHT_POKEMON");
					break;
					case GET_POKEMON :
						colaNombre = strdup("GET_POKEMON");
					break;
					case LOCALIZED_POKEMON :
						colaNombre = strdup("LOCALIZED_POKEMON");
					break;
				}
				fprintf(dump,"Partición %d: %p - %p		[X]		Size: %db		LRU: %d		Cola: %s\n",i,actual->punteroInicial,actual->punteroFinal,actual->tamano,actual->tiempoLRU,colaNombre);
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
