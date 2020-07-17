/*
 * memoria.c
 *
 *  Created on: 16 jul. 2020
 *      Author: utnso
 */

#include "memoria.h"

void deserealizar_mem_NEW_POKEMON (int desplazamiento , cola_NEW_POKEMON * new_poke ) {

							memcpy(&new_poke->id_mensaje,memoria_cache+desplazamiento,sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);

							memcpy(&new_poke->tamanio_nombre,memoria_cache+desplazamiento,sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);

							new_poke->nombre_pokemon = malloc(1);

							new_poke->nombre_pokemon = realloc(new_poke->nombre_pokemon,new_poke->tamanio_nombre + 1);
							memcpy(new_poke->nombre_pokemon,memoria_cache+desplazamiento,new_poke->tamanio_nombre);
							desplazamiento += new_poke->tamanio_nombre;

							memcpy(&new_poke->posicion_x,memoria_cache+desplazamiento,sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);

							memcpy(&new_poke->posicion_y,memoria_cache+desplazamiento,sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);

							memcpy(&new_poke->cantidad,memoria_cache+desplazamiento,sizeof(uint32_t));

							new_poke->nombre_pokemon[new_poke->tamanio_nombre] = '\0';

}

void deserealizar_mem_APPEARED_POKEMON (int head, int desplazamiento, void * buffer, int tamanio , cola_APPEARED_POKEMON * app_poke) {

			app_poke->nombre_pokemon = malloc(1);

			memcpy(&app_poke->id_mensaje,(buffer+desplazamiento),sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);

			memcpy(&app_poke->tamanio_nombre,(buffer+desplazamiento),sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);

			app_poke->nombre_pokemon = realloc(app_poke->nombre_pokemon,app_poke->tamanio_nombre +1);
			memcpy(app_poke->nombre_pokemon,(buffer+desplazamiento),app_poke->tamanio_nombre);
			desplazamiento += app_poke->tamanio_nombre;

			memcpy(&app_poke->posicion_x,(buffer+desplazamiento),sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);

			memcpy(&app_poke->posicion_y,(buffer+desplazamiento),sizeof(uint32_t));

			app_poke->nombre_pokemon[app_poke->tamanio_nombre] = '\0';

			//log_info(logger,"deserealizar CASE APPEARED_POKEMON -> ID:%d , POKEMON:%s , POSICIÓN X:%d , POSICIÓN Y: %d",app_poke->id_mensaje,app_poke->nombre_pokemon,app_poke->posicion_x,app_poke->posicion_y);

			//return app_poke;
}

void deserealizar_mem_CATCH_POKEMON (int head, int desplazamiento, void * buffer, int tamanio, cola_CATCH_POKEMON* cat_poke) {

	cat_poke->nombre_pokemon = malloc(1);

			memcpy(&cat_poke->id_mensaje,(buffer+desplazamiento),sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);

			memcpy(&cat_poke->tamanio_nombre,(buffer+desplazamiento),sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);

			cat_poke->nombre_pokemon = realloc(cat_poke->nombre_pokemon,cat_poke->tamanio_nombre +1);
			memcpy(cat_poke->nombre_pokemon,(buffer+desplazamiento),cat_poke->tamanio_nombre);
			desplazamiento += cat_poke->tamanio_nombre;

			memcpy(&cat_poke->posicion_x,(buffer+desplazamiento),sizeof(uint32_t));
			desplazamiento += sizeof(uint32_t);

			memcpy(&cat_poke->posicion_y,(buffer+desplazamiento),sizeof(uint32_t));

			cat_poke->nombre_pokemon[cat_poke->tamanio_nombre] = '\0';

			//return cat_poke;
}

void deserealizar_mem_CAUGHT_POKEMON (int head, int desplazamiento, void * buffer, int tamanio , cola_CAUGHT_POKEMON* cau_poke) {

							memcpy(&cau_poke->id_mensaje,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);

							memcpy(&cau_poke->atrapo_pokemon,(buffer+desplazamiento),sizeof(uint32_t));

}

void deserealizar_mem_GET_POKEMON (int head, int desplazamiento, void * buffer, int tamanio , cola_GET_POKEMON * get_poke) {

				get_poke->nombre_pokemon = malloc(1);

							memcpy(&get_poke->id_mensaje,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);

							memcpy(&get_poke->tamanio_nombre,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);

							get_poke->nombre_pokemon = realloc(get_poke->nombre_pokemon,get_poke->tamanio_nombre +1);
							memcpy(get_poke->nombre_pokemon,(buffer+desplazamiento),get_poke->tamanio_nombre);

							get_poke->nombre_pokemon[get_poke->tamanio_nombre] = '\0';

							//return get_poke;
}

void deserealizar_mem_LOCALIZED_POKEMON (int head, int desplazamiento, void * buffer, int tamanio , cola_LOCALIZED_POKEMON * loc_poke_des) {

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

							memcpy(&loc_poke_des->id_mensaje,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);

							memcpy(&loc_poke_des->tamanio_nombre,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);

							loc_poke_des->nombre_pokemon = malloc(1);

							loc_poke_des->nombre_pokemon = realloc(loc_poke_des->nombre_pokemon,loc_poke_des->tamanio_nombre +1);
							memcpy(loc_poke_des->nombre_pokemon,(buffer+desplazamiento),loc_poke_des->tamanio_nombre);
							desplazamiento += loc_poke_des->tamanio_nombre;

							memcpy(&loc_poke_des->cantidad,(buffer+desplazamiento),sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);

							loc_poke_des->lista_posiciones = list_create();

							for (int i = 0 ; i < loc_poke_des->cantidad ; i++){
							posicion *laPosicion = malloc (sizeof(posicion));
							memcpy(&laPosicion->posicion_x,buffer+desplazamiento,sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);
							memcpy(&laPosicion->posicion_y,buffer+desplazamiento,sizeof(uint32_t));
							desplazamiento += sizeof(uint32_t);
							list_add(loc_poke_des->lista_posiciones,laPosicion);
							}

							loc_poke_des->nombre_pokemon[loc_poke_des->tamanio_nombre] = '\0';

							//return loc_poke;
}
