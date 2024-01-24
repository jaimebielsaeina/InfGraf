COMPILACIÓN

La compilación de los ejecutables con G++ debe realizarse con los siguientes comandos:

g++ pathTracer.cpp -o3 -o pathTracer
g++ photonMapping.cpp -std=c++17 -o3 -o photonMapping
g++ toneMapping.cpp -o3 -o toneMapping



EJECUCIÓN DE PATH TRACER Y PHOTON MAPPING

La ejecución del renderizador se realiza con el siguiente comando:

./pathTracer <entrada_escena> <imagen_salida>
./photonMapping <entrada_escena> <imagen_salida> <next-event estimation (y/n)>

El fichero de entrada debe tener formato de escena OBJX. Se explica con detalle el formato de archivo en OBJX.md.
El fichero de salida tendrá formato de imagen PPM.

Para obtener las imágenes de los informes se han usado las escenas de la carpeta ./scenes.
Para adaptar los parámetros de cada imágen leer el archivo OBJX.md donde se detalla el formato .objx.



EJECUCIÓN DE TONE MAPPING

Para el tone mapping existen varios modos de ejuste de imagen. Se muestran todas las opciones a continuación:

./photonMapping clamping <imagen_entrada> <imagen_salida>
./photonMapping equalization <imagen_entrada> <imagen_salida>
./photonMapping clamping-equalization <imagen_entrada> <imagen_salida> <valor_umbral>
./photonMapping gamma <imagen_entrada> <imagen_salida> <coeficiente_gamma>
./photonMapping clamping-gamma <imagen_entrada> <imagen_salida> <umbral> <coef_gamma>

Los ficheros de entrada y salida tendrán formato de imagen PPM.