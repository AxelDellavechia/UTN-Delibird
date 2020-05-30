#include <stdio.h>
#include <stdlib.h>
#include "Team.h"

int main(void) {

	iniciar_log();

	crearEstructuras();

	leerArchivoDeConfiguracion(RUTA_CONFIG_MEM,logger);

	iniciar_logCatedra();

	inicializar_semaforos();

	crearHilos();

}


