#include "MetodosGC.h"
#include <commons/config.h>
#include "src/sockets.h"


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


int NewPokemon(cola_NEW_POKEMON* Pokemon){

	int step = 0;
	while(step != ERROR){
	switch(step){

	case 0:{ //Paso 1: Verificar si existe
			leerFiles();
			step = findPokemon(Pokemon->nombre_pokemon);
			break;

			}
	case OPEN:{ //el Pokemon existe pero está abierto el archivo. Cada X tiempo debe loopear

				printf("\n el Pokemon %s está abierto, no se puede utilizar",Pokemon->nombre_pokemon);
				break;
			}
	case OK:{ //el Pokemon existe y se puede utilizar
				printf("\n el Pokemon %s está listo para utilizarse",Pokemon->nombre_pokemon);
				break;
			}
	case ERROR:{ //el Pokemon no existe
				printf("\n Error");
				break;
				}
	}
	}
return OK; //retornar resultado
}




int findPokemon(char* Pokemon){

	int exist = 0;

	for(int j = 0; j < list_size(dirList); j++ ){

		t_files* tempPokemon = malloc (sizeof(t_files));
		tempPokemon = list_get(dirList,j);
		if (string_equals_ignore_case(tempPokemon->file,Pokemon)){   //Recorriendo se fija si encuentra el archivo en el FS
			if (string_equals_ignore_case(tempPokemon->type,"N")){   //Si lo encuentra, se fija que efectivamente sea un Pokemon y no un directorio
				if (string_equals_ignore_case(tempPokemon->open,"N")){  //Verifica si el archivo ya está abierto o no
					//Marco el archivo como abierto para que no lo utilice nadie mas
					updateStatusFile(tempPokemon,"Y");
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

