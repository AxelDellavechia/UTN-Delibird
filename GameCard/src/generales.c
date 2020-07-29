/*
<<<<<<< HEAD

=======
 * generales.c
 *
 *  Created on: 29 abr. 2020
 *      Author: utnso
 */
>>>>>>> origin/master
#include "generales.h"
#include <time.h>
#include <sys/time.h>


<<<<<<< HEAD
unsigned long long obtener_timestamp()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long long timestamp = ((unsigned long long)tv.tv_sec*1e3) + ((unsigned long long)tv.tv_usec/1000);
	return timestamp;
}

*/
=======

unsigned long long obtener_timestamp()
{ 	struct timeval tv;
gettimeofday(&tv, NULL);
unsigned long long timestamp = ((unsigned long long)tv.tv_sec*1e3) + ((unsigned long long)tv.tv_usec/1000);
return timestamp;
}
unsigned int token() {
	srand(time(NULL));
	return rand() % random();
}
>>>>>>> origin/master
