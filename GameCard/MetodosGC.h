#include "Inicializacion.h"
#include <commons/config.h>
#include "src/sockets.h"
#include "src/mensajeria.h"
<<<<<<< HEAD
=======
#include <pthread.h>

>>>>>>> origin/master

#define OPEN 2
#define NW_UPDATE_POS 3
#define NW_INSERT_POS 4
#define NW_SAVE 5
<<<<<<< HEAD

typedef struct{
	char* file;
	char* parent;
	char* type;
	char* open;
=======
#define EXIST 8
#define NOT_EXIST 6
#define NOT_EXIST_POSITION 9
#define RESPONDER 7
#define CHECK_POSITION 8
#define NW_SAVE_DEL_POSITION 10


typedef struct{
	char* file;
	/*char* parent;
	char* type;
	char* open;*/
>>>>>>> origin/master
	int size;
	t_list* blocks;
	t_list* positions;
}t_files;

<<<<<<< HEAD
typedef struct{
	int Pos_x;
	int Pos_y;
	int Cantidad;
} t_positions;
=======




>>>>>>> origin/master

int NewPokemon(cola_NEW_POKEMON* Pokemon);

