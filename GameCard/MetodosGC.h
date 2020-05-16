#include "Inicializacion.h"
#include <commons/config.h>
#include "src/sockets.h"
#include "src/mensajeria.h"

#define OPEN 2

typedef struct{
	char* file;
	char* parent;
	char* type;
	char* open;
	int size;
	t_list* blocks;
}t_files;

int NewPokemon(cola_NEW_POKEMON* Pokemon);

