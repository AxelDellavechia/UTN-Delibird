#include "MetodosGC.h"
#include <commons/config.h>
#include "src/sockets.h"





int NewPokemon(cola_NEW_POKEMON* Pokemon){

	int step = 0;
	t_files dataPokemon;// = malloc (sizeof(t_files));
	while(step != ERROR){
	switch(step){

	case 0:{ //Paso 1: Verificar si existe
			leerFiles();
				step = findPokemon(Pokemon->nombre_pokemon, &dataPokemon);
			break;

			}
	case OPEN:{ //el Pokemon existe pero está abierto el archivo. Cada X tiempo debe reintentar

				printf("\n el Pokemon %s está abierto, no se puede utilizar",Pokemon->nombre_pokemon);
				usleep(config_File->TIEMPO_DE_REINTENTO_OPERACION *1000);
				step = 0;
				break;
			}
	case OK:{ //el Pokemon existe y se puede utilizar
				printf("\n el Pokemon %s está listo para utilizarse",Pokemon->nombre_pokemon);
				leerBloques(Pokemon, &dataPokemon);
				for(int j = 0; j < list_size(dataPokemon.positions); j++ ){
					 t_positions* pos = malloc (sizeof(t_positions));
					pos = list_get(dataPokemon.positions,j);
					printf("\nPosicion: %s  Cantidad:%i",pos->LatLong, pos->Cantidad);
				}
				break;
			}
	case ERROR:{ //el Pokemon no existe
				printf("\n El Pokemon no existe, se debe crear");
				break;
				}
	}
	}
return OK; //retornar resultado
}







void leerFiles(){
	cantFiles = 0;
	dirList = list_create();
	cargarArbolDirectorios(PuntoMontaje->FILES);
	printf("Elementos de la lista: %i",list_size(dirList));
	  for(int i = 0; i< cantFiles; i++){
			  t_files* archivo = malloc (sizeof(t_files));
			  archivo = list_get(dirList, i);
			  printf("\nArchivo: %s   Padre:%s   Tipo:%s ", archivo->file, archivo->parent, archivo->type);
			  if (string_equals_ignore_case(archivo->type,"N")){
				  printf("  Estado: %s Bloques: ", archivo->open);

				  for(int j = 0; j < list_size(archivo->blocks); j++ ){
					  printf("%s  ",list_get(archivo->blocks,j));
				  }
			  }
			  char* subDir = malloc (string_length(archivo->parent) + string_length(archivo->file) + 2);
			  strcpy(subDir,archivo->parent);
			  strcat(subDir,archivo->file);
			  strcat(subDir,"/");
			  cargarArbolDirectorios(subDir);

		  }

}

void cargarArbolDirectorios(char* Directorio){

	  //dirList = list_create();
	  DIR *dir;
	  /* en *ent habrá información sobre el archivo que se está "sacando" a cada momento */
	  struct dirent *ent;

	  /* Empezaremos a leer en el directorio actual */
	  dir = opendir (Directorio);

	  /* Miramos que no haya error */
	  if (dir == NULL){
	    log_info(logger,"No se pudo cargar la estructura de Directorios");
	  }else{

	  /* Leyendo uno a uno todos los archivos que hay */
	  while ((ent = readdir (dir)) != NULL)
	    {
	      /* Nos devolverá el directorio actual (.) y el anterior (..), como hace ls */
	      if ( (strcmp(ent->d_name, ".")!=0) && (strcmp(ent->d_name, "..")!=0) && (strcmp(ent->d_name, "Metadata.bin")!=0))
	    {
	      /* Una vez tenemos el archivo, lo pasamos a una función para procesarlo. */

	    t_files* File = malloc (sizeof(t_files));
		File->file = malloc (string_length(ent->d_name) + 1) ;
		File->parent = malloc (string_length(Directorio) + 1) ;

		strcpy(File->file,ent->d_name);
		strcpy(File->parent,Directorio);


		//Leo el metadata del archivo

		/*char* dirMetadata = (char*) malloc(string_length(File->file) + string_length(File->parent) + string_length("/Metadata.bin") + 1 );
		strcpy(dirMetadata, File->parent);
		string_append(&dirMetadata,File->file);
		string_append(&dirMetadata, "/Metadata.bin");*/

		t_config *MetadataFiles;
		//MetadataFiles = config_create(dirMetadata);
		char* dirMetadata = string_new();
		int res = getMetadataDir(&dirMetadata, File);
		MetadataFiles = config_create(dirMetadata);
		File->type = string_duplicate(config_get_string_value(MetadataFiles,"DIRECTORY"));

		if (config_has_property(MetadataFiles, "SIZE")){
			File->size = config_get_int_value(MetadataFiles,"SIZE");
		}
		if (config_has_property(MetadataFiles, "OPEN")){
			File->open = string_duplicate(config_get_string_value(MetadataFiles,"OPEN"));
		}

		if (config_has_property(MetadataFiles, "BLOCKS")){
		char** BlocksAux = config_get_array_value(MetadataFiles, "BLOCKS");
		File->blocks = list_create();
		int posicionLista = 0;
			while(BlocksAux[posicionLista] != NULL) {
				list_add(File->blocks, string_duplicate(BlocksAux[posicionLista]));
				posicionLista++;
			}
		}

	    list_add(dirList, File);
	    //mantengo la cantidad de archivos que voy leyendo para luego ir recorriendolos y agregando los nuevos
	    cantFiles++;
	    }
	    }
	  closedir (dir);
	  }
}


int leer_metaData_files(){
/*	char *direccionArchivoMedata=(char *) malloc(strlen(PuntoMontaje->METADATA));

	string_append(&direccionArchivoMedata,PuntoMontaje->METADATA);
	printf("direccionArchivoMedata: %s\n",direccionArchivoMedata);*/

	config_MetaData = reservarMemoria(sizeof(t_config_MetaData));

	t_config *archivo_MetaData;
	archivo_MetaData=config_create(PuntoMontaje->METADATA_FILE);
	config_MetaData->cantidad_bloques=config_get_int_value(archivo_MetaData,"BLOCKS");
    config_MetaData->magic_number=string_duplicate(config_get_string_value(archivo_MetaData,"MAGIC_NUMBER"));
	config_MetaData->tamanio_bloques=config_get_int_value(archivo_MetaData,"BLOCK_SIZE");
	//free(direccionArchivoMedata);
	//config_destroy(archivo_MetaData);
	return 0;
}




int findPokemon(char* Pokemon, t_files *dataPokemon){

	int exist = 0;
	t_files* tempPokemon = malloc(sizeof(t_files));
	for(int j = 0; j < list_size(dirList); j++ ){

		tempPokemon = list_get(dirList,j);
		if (string_equals_ignore_case(tempPokemon->file,Pokemon)){   //Recorriendo se fija si encuentra el archivo en el FS
			if (string_equals_ignore_case(tempPokemon->type,"N")){   //Si lo encuentra, se fija que efectivamente sea un Pokemon y no un directorio
				if (string_equals_ignore_case(tempPokemon->open,"N")){  //Verifica si el archivo ya está abierto o no
					//Marco el archivo como abierto para que no lo utilice nadie mas
					dataPokemon->file = malloc(string_length(tempPokemon->file));
					strcpy(dataPokemon->file, tempPokemon->file);
					dataPokemon->open = malloc(string_length(tempPokemon->open));
					strcpy(dataPokemon->open, tempPokemon->open);
					dataPokemon->parent = malloc(string_length(tempPokemon->parent));
					strcpy(dataPokemon->parent, tempPokemon->parent);
					dataPokemon->type = malloc(string_length(tempPokemon->type));
					strcpy(dataPokemon->type, tempPokemon->type);
					dataPokemon->size = tempPokemon->size;
					dataPokemon->blocks = list_create();
					list_add_all(dataPokemon->blocks, tempPokemon->blocks);

					//list_add_all(dataPokemon->positions, tempPokemon->positions);

					updateStatusFile(dataPokemon,"Y");
					return OK;
				}
				else{
					return OPEN;
				}
			}
		}
	}
	return ERROR; //No se encontró el Pokemon
}


//Función que cambia el estado del Pokemon a abierto para que nadie mas pueda utilizarlo.
int updateStatusFile(t_files* tempPokemon, char* estado){

	t_config *archivo_MetaData;
	char* dirMetadata = string_new();
	int res = getMetadataDir(&dirMetadata, tempPokemon);
	archivo_MetaData=config_create(dirMetadata);
	config_set_value(archivo_MetaData, "OPEN", estado);
	config_save(archivo_MetaData);
	return OK;
}

void getMetadataDir(char** result, t_files* tempPokemon){

	char* dirMetadata = (char*) malloc(string_length(tempPokemon->file) + string_length(tempPokemon->parent) + string_length("/Metadata.bin") + 1 );
	strcpy(dirMetadata, tempPokemon->parent);
	string_append(&dirMetadata,tempPokemon->file);
	string_append(&dirMetadata, "/Metadata.bin");

	*result = realloc(*result, strlen(*result) + strlen(dirMetadata) + 1);
	string_append(result,dirMetadata);
}




void leerBloques(cola_NEW_POKEMON* Pokemon, t_files *dataPokemon)
{
	t_files* tempPokemon = malloc(sizeof(t_files));
	for(int j = 0; j < list_size(dirList); j++ ){
			tempPokemon = list_get(dirList,j);
			if ((string_equals_ignore_case(tempPokemon->file,Pokemon->nombre_pokemon)) && (string_equals_ignore_case(tempPokemon->type,"N" ))){   //Recorriendo se fija si encuentra el archivo en el FS
				dataPokemon->positions = list_create();
				for(int i = 0; i< list_size(tempPokemon->blocks); i++){
					FILE* block;
					char* bloque = list_get(tempPokemon->blocks,i);
					char* dirBloque = (char*) malloc(string_length(PuntoMontaje->BLOCKS)+ string_length(bloque)+ string_length(".bin"));
					strcpy(dirBloque,PuntoMontaje->BLOCKS);
					strcat(dirBloque,bloque);
					strcat(dirBloque,".bin");
					block = fopen(dirBloque,"r");
				    //Lee línea a línea para cargar todas las posiciones en la data del Pokemon
				    char* linea = malloc (config_MetaData->tamanio_bloques);
				    tempPokemon->positions = list_create();
					while(fgets(linea, config_MetaData->tamanio_bloques, (FILE*) block)) {
				        t_positions* pos = malloc (sizeof(t_positions));
				        char** posCant = string_n_split(linea, 2, "=");
				        pos->LatLong = malloc(string_length(posCant[0]));
				        strcpy(pos->LatLong,posCant[0]);
				        pos->Cantidad = atoi(posCant[1]);
				        list_add(tempPokemon->positions, pos);
				    }

					list_add_all(dataPokemon->positions, tempPokemon->positions);
				    fclose(block);

				}

			}
		}
}


/*
void insertPosition(cola_NEW_POKEMON* Pokemon)
{

	char* row = string_new();
	row = malloc (1+ string_length(Pokemon->posicion_x) + string_length(Pokemon->posicion_y) + string_length(Pokemon->cantidad) + string_length("-=\n"));

	strcpy(row,Pokemon->posicion_x);
	strcat(row,"-");
	strcat(row,Pokemon->posicion_y);
	strcat(row,"=");
	strcat(row,Pokemon->cantidad);


 		ptr_aux = realloc(ptr,tamanio_total + 1 );
		ptr = ptr_aux;
		ptr[tamanio_total] = '\0';
		//printf("ptr finalll: %s\n",ptr);

		ptr_aux = NULL;
		int tam_bloque = config_MetaData.tamanio_bloques;
		//printf("tam_bloque: %i\n",tam_bloque);
		int tamanio_a_grabar = tamanio_total ;
		int desplazamiento = 0;

		//Una vez que tengo el buffer con la info de una tabla los guardo en los
		//bloques, guardando los nros de bloques utilizados
		while(tamanio_a_grabar > 0){
			int bloque_libre = proximobloqueLibre();
			printf("bloque_libre : %i\n",bloque_libre);

			if(tamanio_a_grabar >= tam_bloque){//Si lo que voy a grabar ocupa mas de un bloque
				ptr_aux = malloc(tam_bloque + 1);
				memcpy(ptr_aux,ptr + desplazamiento,tam_bloque);
				memcpy(ptr_aux + tam_bloque,"\0",CHAR);
				grabarBloque(ptr_aux,bloque_libre);
				tamanio_a_grabar -= tam_bloque;
				desplazamiento += tam_bloque;
			}else{//Si lo que voy a grabar ocupa menos de un bloque
				ptr_aux = malloc(tamanio_a_grabar + 1);
				memcpy(ptr_aux,ptr + desplazamiento,tamanio_a_grabar);
				memcpy(ptr_aux + tamanio_a_grabar,"\0",CHAR);
				grabarBloque(ptr_aux,bloque_libre);
				tamanio_a_grabar -= tam_bloque;
				desplazamiento += tamanio_a_grabar;
			}

			free(ptr_aux);
			ptr_aux =  NULL;

			int* nro_particion = malloc(INT);
			*nro_particion = bloque_libre;

			list_add(lista_bloques,nro_particion);


		}

		dump_crear_tmp(tabla->nombre_tabla,tamanio_total,lista_bloques);


	}

	limpiar_memtable();

}

*/

