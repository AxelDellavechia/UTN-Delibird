#include "Team.h"

int main(void) {

	iniciar_log();

	inicializar_semaforos();

	crearEstructuras();

	leerArchivoDeConfiguracion(RUTA_CONFIG_MEM,logger);

	iniciar_logCatedra();

	crearHilos();

	//return EXIT_SUCCESS;
}


