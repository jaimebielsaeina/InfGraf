#include <iostream>
#include "light/toneMapping.h"

// Función que realiza una transformación de clamping sobre un fichero de imagen.
int main(int argc, char* argv[]){

	// Obtiene la opción de transformación.
	string opcion = argv[1];

	if(opcion == "clamping") {

		// Comprueba que el número de argumentos es correcto.
		if (argc == 4) {

			// Realiza la transformación.
			cout << "Realizando una transformación de clamping sobre el fichero " << argv[2] << endl;
			transform_clamping(argv[2], argv[3]);
			cout << "Completado. Resultados en el fichero " << argv[3] << "." << endl;

		} else {
			cout << "Número de argumentos incorrecto, uso de clamping:" << endl;
			cout << argv[0] << " clamping <fichero entrada> <fichero salida>" << endl;
		}

	} else if(opcion == "equalization") {

		// Comprueba que el número de argumentos es correcto.
		if (argc == 4) {

			// Realiza la transformación.
			cout << "Realizando una transformación de ecualización sobre el fichero " << argv[2] << endl;
			transform_equalization(argv[2], argv[3]);
			cout << "Completado. Resultados en el fichero " << argv[3] << "." << endl;

		} else {
			cout << "Número de argumentos incorrecto, uso de ecualización:" << endl;
			cout << argv[0] << " equalization <fichero entrada> <fichero salida>" << endl;
		}

	} else if (opcion == "clamping-equalization") {

		// Comprueba que el número de argumentos es correcto.
		if (argc == 5) {

			// Realiza la transformación.
			cout << "Realizando una transformación de clamping-ecualización con umbral " << argv[4] << " sobre el fichero " << argv[2] << endl;
			transform_clamp_equal(argv[2], argv[3], stoi(argv[4]));
			cout << "Completado. Resultados en el fichero " << argv[3] << "." << endl;

		} else {
			cout << "Número de argumentos incorrecto, uso de clamping-ecualización:" << endl;
			cout << argv[0] << " clamping-equalization <fichero entrada> <fichero salida> <valor umbral>" << endl;
		}

	} else if (opcion == "gamma") {

		// Comprueba que el número de argumentos es correcto.
		if (argc == 5) {

			// Realiza la transformación.
			cout << "Realizando una transformación gamma con coeficiente " << argv[4] << " sobre el fichero " << argv[2] << endl;
			transform_gamma(argv[2], argv[3], stof(argv[4]));
			cout << "Completado. Resultados en el fichero " << argv[3] << "." << endl;

		} else {
			cout << "Número de argumentos incorrecto, uso de corrección gamma:" << endl;
			cout << argv[0] << " gamma <fichero entrada> <fichero salida> <coeficiente gamma>" << endl;
		}

	} else if (opcion == "clamping-gamma") {

		// Comprueba que el número de argumentos es correcto.
		if (argc == 6) {

			// Realiza la transformación.
			cout << "Realizando una transformación de clamping-gamma con umbral " << argv[4] << " y coeficiente " << argv[5] << " sobre el fichero " << argv[2] << endl;
			transform_clamp_gamma(argv[2], argv[3], stoi(argv[4]), stof(argv[5]));
			cout << "Completado. Resultados en el fichero " << argv[3] << "." << endl;

		} else {
			cout << "Número de argumentos incorrecto, uso de clamping-gamma:" << endl;
			cout << argv[0] << " clamping-gamma <fichero entrada> <fichero salida> <umbral> <corf. gamma>" << endl;
		}

	}

	// Si la opción no es válida, muestra un mensaje de error.
	else {
		cout << "Opción no válida. Opciones disponibles:" << endl;
		cout << argv[0] << " clamping <fichero entrada> <fichero salida>" << endl;
		cout << argv[0] << " equalization <fichero entrada> <fichero salida>" << endl;
		cout << argv[0] << " clamping-equalization <fichero entrada> <fichero salida> <valor umbral>" << endl;
		cout << argv[0] << " gamma <fichero entrada> <fichero salida> <coeficiente gamma>" << endl;
		cout << argv[0] << " clamping-gamma <fichero entrada> <fichero salida> <umbral> <corf. gamma>" << endl;
	}
}