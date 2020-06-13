#include "MetodosGC.h"
#include <commons/config.h>
#include "src/sockets.h"
#include <string.h>





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

	case NOT_EXIST:{
				step = CreatePokemon(Pokemon);
				break;
			}
	case OPEN:{ //el Pokemon existe pero está abierto el archivo. Cada X tiempo debe reintentar

				printf("\n el Pokemon %s está abierto, no se puede utilizar",Pokemon->nombre_pokemon);
				usleep(config_File->TIEMPO_DE_REINTENTO_OPERACION *1000);
				step = 0;
				break;
			}
	case OK:{ //el Pokemon existe y se puede utilizar, entonces busco las posiciones que tiene.
				printf("\n el Pokemon %s está listo para utilizarse",Pokemon->nombre_pokemon);
				leerBloques(Pokemon, &dataPokemon);
				step = NW_UPDATE_POS;
				break;
			}
	case NW_UPDATE_POS:{
				t_positions* newPosition = malloc (sizeof(t_positions));;

				newPosition->Pos_x = Pokemon->posicion_x;
				newPosition->Pos_y = Pokemon->posicion_y;
				newPosition->Cantidad = Pokemon->cantidad;
				InsertUpdatePosition(newPosition, &dataPokemon);
				step = NW_SAVE;
				break;
			}
	case NW_SAVE:{
				dataPokemon.size = SavePositionInBlocks(&dataPokemon);
				update_metaData_files(&dataPokemon);
				updateStatusFile(&dataPokemon,"N");
				step= RESPONDER; //cambiar esto, se debe seguir con enviar la respuesta a quien pidió la instrucción
				break;
			}
	case ERROR:{ //el Pokemon no existe
				printf("\n El Pokemon no existe, se debe crear");
				break;
				}

	case RESPONDER:{ //el Proceso se hizo OK. Se debe responder al usuario con APPEARED_POKEMON
				printf("\n El Proceso finalizó correctamente.");
				break;
				}
	}
	}
return OK; //retornar resultado
}




int CatchPokemon(cola_CATCH_POKEMON* Pokemon){

	int step = 0;
	t_files dataPokemon;// = malloc (sizeof(t_files));
	while(step != ERROR){
	switch(step){

	case 0:{ //Paso 1: Verificar si existe
			leerFiles();
			step = findPokemon(Pokemon->nombre_pokemon, &dataPokemon);
			break;

			}

	case NOT_EXIST:{ //el Pokemon no existe, se debe informar el error
				printf("El Pokemon no existe");
				break;
			}
	case OPEN:{ //el Pokemon existe pero está abierto el archivo. Cada X tiempo debe reintentar

				printf("\n el Pokemon %s está abierto, no se puede utilizar",Pokemon->nombre_pokemon);
				usleep(config_File->TIEMPO_DE_REINTENTO_OPERACION *1000);
				step = 0;
				break;
			}
	case OK:{ //el Pokemon existe y se puede utilizar, entonces busco las posiciones que tiene.
				printf("\n el Pokemon %s está listo para utilizarse",Pokemon->nombre_pokemon);
				leerBloques(Pokemon, &dataPokemon);
				step = NW_UPDATE_POS;
				break;
			}

	case NW_UPDATE_POS:{
				t_positions* newPosition = malloc (sizeof(t_positions));;
				newPosition->Pos_x = Pokemon->posicion_x;
				newPosition->Pos_y = Pokemon->posicion_y;
				newPosition->Cantidad = -1; //decrementa la posición
				//Intenta actualizar la posición. Si la encuentra continua grabandola en los bloques
				//Si no encuentra la posición, devuelve NOT_EXIST para que se le informa el broker el error.
				step = InsertUpdatePosition(newPosition, &dataPokemon);

				break;
			}
	case NW_SAVE:{
				dataPokemon.size = SavePositionInBlocks(&dataPokemon);
				update_metaData_files(&dataPokemon);
				updateStatusFile(&dataPokemon,"N");
				step= RESPONDER; //cambiar esto, se debe seguir con enviar la respuesta a quien pidió la instrucción
				break;
			}
	case ERROR:{ //el Pokemon no existe
				printf("\n El Pokemon no existe, se debe crear");
				return ERROR;
				break;
				}

	case RESPONDER:{ //el Proceso se hizo OK. Se debe responder al usuario con APPEARED_POKEMON
				printf("\n El Proceso finalizó correctamente.");
				step= ERROR; //le pongo ERROR para que salga del switch nada mas.
				break;
				}
	}
	}
return OK; //retornar resultado
}




int CreatePokemon(cola_NEW_POKEMON* Pokemon){

	char* dirFile = malloc( string_length(PuntoMontaje->FILES) + string_length(Pokemon->nombre_pokemon));
	strcpy(dirFile,PuntoMontaje->FILES);
	strcat(dirFile,Pokemon->nombre_pokemon);
	int result = mkdir(dirFile,0777);
	if (result  == 0){
		char* dirMetadata = malloc (string_length(dirFile) + string_length("/Metadata.bin"));
		strcpy(dirMetadata,dirFile);
		strcat(dirMetadata,"/Metadata.bin");
		FILE *fMetadata = fopen (dirMetadata, "wb");
		if(fMetadata == NULL){
		printf("No se puede crear el Metadata del Pokemon %s\n", Pokemon->nombre_pokemon);
		return ERROR;
		}else{
		fprintf(fMetadata,"DIRECTORY=N\nOPEN=N\nSIZE=0\nBLOCKS=[]\n");
		}
		fclose(fMetadata);

	}else
	{
		return ERROR;
	}
	return result;
}

int InsertUpdatePosition(t_positions* newPos, t_files *posPokemon){

	for(int j = 0; j < list_size(posPokemon->positions) ; j++ ){
		t_positions* pos = malloc (sizeof(t_positions));
		pos = list_get(posPokemon->positions,j);
		if(pos->Pos_x == newPos->Pos_x && pos->Pos_y == newPos->Pos_y){
			newPos->Cantidad = pos->Cantidad + newPos->Cantidad;
			list_remove(posPokemon->positions,j);
			if(newPos->Cantidad > 0)
				list_add(posPokemon->positions,newPos);
			return NW_SAVE; //Si encontró y actualizó la posición, retorna para que grabe los cambios.
		}
	}
	list_add(posPokemon->positions,newPos);
	return NOT_EXIST; //Si no retorna la posición retorna ERROR.
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

int update_metaData_files(t_files *dataPokemon){

	t_config *MetadataFiles;
	char* dirMetadata = string_new();
	int res = getMetadataDir(&dirMetadata, dataPokemon);
	MetadataFiles = config_create(dirMetadata);

	if (config_has_property(MetadataFiles, "SIZE")){
			char* tempSize = malloc(string_length(string_itoa(dataPokemon->size)));
			strcpy(tempSize,string_itoa(dataPokemon->size));
			config_set_value(MetadataFiles,"SIZE", tempSize);
			free(tempSize);
	}
	if (config_has_property(MetadataFiles, "BLOCKS")){
		char* tempBlocks = (char*) malloc(string_length("[")+string_length("]"));
		strcpy(tempBlocks,"[");
		for(int i = 0; i< list_size(dataPokemon->blocks); i++){
			char* bloque = list_get(dataPokemon->blocks,i);
			tempBlocks= realloc(tempBlocks, strlen(tempBlocks) + strlen(bloque));
			strcat(tempBlocks,bloque);
			if(i+1 < list_size(dataPokemon->blocks) ){
				tempBlocks= realloc(tempBlocks, strlen(tempBlocks) + strlen(","));
				strcat(tempBlocks,",");
			}
		}
		strcat(tempBlocks,"]");
		config_set_value(MetadataFiles,"BLOCKS", tempBlocks);


	}

	config_save(MetadataFiles);
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
	return NOT_EXIST; //No se encontró el Pokemon
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


				char* lecturaBloques = string_new();
				lecturaBloques = (char*) malloc (tempPokemon->size);//config_MetaData->tamanio_bloques * list_size(tempPokemon->blocks));
				strcpy(lecturaBloques,"");
				int tamanio_a_leer = tempPokemon->size;

				for(int i = 0; i< list_size(tempPokemon->blocks); i++){
					FILE* block;
					char* bloque = list_get(tempPokemon->blocks,i);
					char* dirBloque = (char*) malloc(string_length(PuntoMontaje->BLOCKS)+ string_length(bloque)+ string_length(".bin"));
					strcpy(dirBloque,PuntoMontaje->BLOCKS);
					strcat(dirBloque,bloque);
					strcat(dirBloque,".bin");
					block = fopen(dirBloque,"r");

					int desplazamiento = 0;
					char cadena[config_MetaData->tamanio_bloques];
					 while (fgets(cadena,config_MetaData->tamanio_bloques,(FILE*) block) != NULL)
						 strcat(lecturaBloques,cadena);

					 fclose(block);
				}


				tempPokemon->positions = list_create();
				char** strPos = string_split(lecturaBloques,"\n");
				t_list* tempPos = list_create();
				int i = 0;
				while(strPos[i] != NULL){
					t_positions* pos = malloc (sizeof(t_positions));
					char** posCant = string_split(strPos[i], "=");
					char** coordenadas = string_split(posCant[0],"-");
					pos->Pos_x=atoi(coordenadas[0]);
					pos->Pos_y =atoi(coordenadas[1]);
					pos->Cantidad = atoi(posCant[1]);
					list_add(tempPokemon->positions, pos);
					i++;

				}
				dataPokemon->positions = list_create();
				list_add_all(dataPokemon->positions, tempPokemon->positions);

			}
		}
}

int SavePositionInBlocks(t_files *dataPokemon){
	//Limpio los bloques existentes. Reescribo todos los bloques para evitar fragmentación interna,
	//o que el tamaño del bloque se exceda del límite.
	CleanBlocks(dataPokemon);
	char* buffer = string_new();
	int bloquesUsados = 0;
	bloquesUsados = list_size(dataPokemon->blocks);
	serializarPosiciones(&buffer,dataPokemon);

	int sizeBuffer = string_length(buffer);
	int resultBuffer = sizeBuffer;
	//Me fijo cuantos bloques necesito para grabar la información actualizada.
	int bloquesNecesarios = 0;
	if((string_length(buffer)%config_MetaData->tamanio_bloques)==0){
		bloquesNecesarios = sizeBuffer/config_MetaData->tamanio_bloques;
	}else{
		bloquesNecesarios = (sizeBuffer/config_MetaData->tamanio_bloques) + 1;
	}

	/*1° Verifico si tengo la cantidad de bloques que necesito.
	 * 2° Si necesito los mismos o mas bloques
	 * */

if (bloquesNecesarios <= bloquesUsados + cantidadDeBloquesLibres()){

	t_list* bloquesExtras = list_create();

	for (int i = 0; i < (bloquesNecesarios - bloquesUsados); i++) //Si necesito bloques extra, ya los reservo para que otro hilo no me los utilice.
	{
		int nextBlock = proximobloqueLibre();
		list_add(bloquesExtras,nextBlock);
	}

	int desplazamiento = 0;
	for(int i = 0; i<bloquesNecesarios;i++)
	{
		char* bloque = list_get(dataPokemon->blocks,i);
		int intBloque;
		if(bloque != NULL){
			intBloque =atoi(bloque);
		}else{
			intBloque = list_get(bloquesExtras,0);
			list_add(dataPokemon->blocks, string_itoa(intBloque));
			list_remove(bloquesExtras,0);
		}

		char* tempData = string_substring(buffer, desplazamiento,config_MetaData->tamanio_bloques);
		grabarBloque(tempData,intBloque);
		free(tempData);
		sizeBuffer -= config_MetaData->tamanio_bloques;
		desplazamiento += config_MetaData->tamanio_bloques;

	}

	//Ahora debo liberar los bloques de mas
		while( bloquesNecesarios < list_size(dataPokemon->blocks))
		{
			int bit = atoi(list_get(dataPokemon->blocks,bloquesNecesarios));
			list_remove(dataPokemon->blocks,bloquesNecesarios);
			bitarray_clean_bit(bitarray,bit);
		}



		return resultBuffer;
}

else{
	log_info(logger,"No hay bloques suficientes para grabar lo solicitado. Se descarta solicitud.");
	return ERROR;
}

}

void CleanBlocks(t_files *dataPokemon){

	for(int i = 0; i< list_size(dataPokemon->blocks); i++){
		FILE* block;
		char* bloque = list_get(dataPokemon->blocks,i);
		char* dirBloque = (char*) malloc(string_length(PuntoMontaje->BLOCKS)+ string_length(bloque)+ string_length(".bin"));
		strcpy(dirBloque,PuntoMontaje->BLOCKS);
		strcat(dirBloque,bloque);
		strcat(dirBloque,".bin");
		block = fopen(dirBloque,"w");
		fclose(block);
	}
}

void serializarPosiciones(char** buffer, t_files *dataPokemon){

	for(int j = 0; j < list_size(dataPokemon->positions) ; j++ ){
			t_positions* pos = malloc (sizeof(t_positions));
			pos = list_get(dataPokemon->positions,j);
			char* temp = (char*) malloc(string_length(string_itoa(pos->Pos_x)) + string_length("-") +string_length(string_itoa(pos->Pos_y)) + string_length("=") +string_length(string_itoa(pos->Cantidad))+ string_length("\n"));
			strcpy(temp,string_itoa(pos->Pos_x));
			strcat(temp,"-");
			strcat(temp,string_itoa(pos->Pos_y));
			strcat(temp,"=");
			strcat(temp,string_itoa(pos->Cantidad));
			strcat(temp,"\n");
			*buffer = realloc(*buffer,string_length(*buffer) + string_length(temp) + 1);
			//strcat(buffer,temp);
			string_append(buffer,temp);

	}

}


int grabarBloque(char* data, int bloque)
{
	FILE *block;

	char* dirBloque = (char*) malloc(string_length(PuntoMontaje->BLOCKS)+ string_length(string_itoa(bloque))+ string_length(".bin"));
	strcpy(dirBloque,PuntoMontaje->BLOCKS);
	strcat(dirBloque,string_itoa(bloque));
	strcat(dirBloque,".bin");

	block = fopen (dirBloque, "w");
	//printf("dirBloque: %s\n",dirBloque);
	if(block != NULL)
	{
		fprintf(block,"%s",data);
		bitarray_set_bit(bitarray, bloque);
		fclose (block);
	}else
	{
		printf ("\nNo se pudo grabar el bloque %i", bloque);
	}


	int acum=0;
	int i;
	for(i=0; i<string_length(data); i++) // se fija cuantas veces aparece el \n para no contarlo entre los bytes que dumpeo.
	{
		 if(data[i]=="\n")
		 {
	        acum++;

		 }
	}


	free(dirBloque);


	return string_length(data) - acum;
}
