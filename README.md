# tp-2020-1c-Digimon

## Utilización Commons

1) Se debe agregar la variable de la biblioteca nuestra con el comando -> export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/workspace/digiCommons/Debug
2) Validar con ldd ${PROJECT} si se agrego correctamente

## Para el Debug

1) Dentro del eclipse hacer click en la flecha abajo de debug
2) Ir a Run Configurations
3) C/C++ Applications -> New
4) Ir a la pestaña Environments -> New
5) En name ingresar -> LD_LIBRARY_PATH
6) En value ingresar -> ${workspace_loc}/digiCommons/Debug
7) Reiniciar Eclipse

