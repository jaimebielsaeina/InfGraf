#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>
using namespace std;

void clamping(double &r, double &g, double &b){
    if (r > 1) r = 1;
    if (g > 1) g = 1;
    if (b > 1) b = 1;
}

void transform_clamping(string inFile, string outFile) {
    int w, h;
    double m, c;
    bool still = true;
    // reads a ppm file
    ifstream ifs(inFile);
    // writes a ppm file
    ofstream ofs(outFile);
    // reads the header
    string header;
    getline(ifs, header);
    ofs << header << endl;
    // lee la siguiente linea; si es un comentario, lo ignora a no ser que sea #MAX
    string line;
    while (still) {
        getline(ifs, line);
        if (line[0] != '#') {
            still = false;
        } else if (line.substr(0, 5) == "#MAX=") {
            ofs << "#MAX=1\n";
            m = stod(line.substr(5, -1));
        } else {
            ofs << line << endl;
        }
    }
    // lee el ancho y el alto
    w = stoi(line.substr(0, line.find(' ')));
    h = stoi(line.substr(line.find(' ') + 1, -1));

    ofs << w << " " << h << endl;
    // lee el valor de c
    getline(ifs, line);
    c = stod(line);

    ofs << 255 << endl;

    double r, g, b;

    // lee el valor de cada pixel
    for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++){
            ifs >> r >> g >> b;
            r = r * m / c;
            g = g * m / c;
            b = b * m / c;

            clamping(r, g, b);
            
            r *= 255;
            g *= 255;
            b *= 255;
            

            ofs << fixed << (int)r << " " << (int)g << " "  << (int)b << "     ";
        }
        ofs << endl;
    }
    

}

void equalization(double &r, double &g, double &b, double max){
    r /= max;
    g /= max;
    b /= max;
}

void transform_equalization(string inFile, string outFile){
    int w, h;
    int counter = 3;
    double m, c;
    bool still = true;
    // reads a ppm file
    ifstream ifs(inFile);
    // writes a ppm file
    ofstream ofs(outFile);
    // reads the header
    string header;
    getline(ifs, header);
    ofs << header << endl;
    // lee la siguiente linea; si es un comentario, lo ignora a no ser que sea #MAX
    string line;
    while (still) {
        getline(ifs, line);
        if (line[0] != '#') {
            still = false;
        } else if (line.substr(0, 5) == "#MAX=") {
            ofs << "#MAX=1\n";
            m = stod(line.substr(5, -1));
            counter++;
        } else {
            ofs << line << endl;
            counter++;
        }
    }
    // lee el ancho y el alto
    w = stoi(line.substr(0, line.find(' ')));
    h = stoi(line.substr(line.find(' ') + 1, -1));

    ofs << w << " " << h << endl;
    // lee el valor de c
    getline(ifs, line);
    c = stod(line);

    ofs << 255 << endl;

    double r, g, b;
    double max = 0;
    // lee el valor de cada pixel para encontrar el máximo
    for (int i = 0; i < h; i++){
        for (int j = 0; j < w; j++){
            ifs >> r >> g >> b;
            r = r * m / c;
            g = g * m / c;
            b = b * m / c;
            if(r > max) max = r;
            if(g > max) max = g;
            if(b > max) max = b;
        }
    }

    cout << max << endl;
    // Deja de asociarse con el fichero
    ifs.close();
    // Reseteamos el ifstream
    ifs.open(inFile);

    // Saltamos las lineas previas a los píxeles
    for (int i = 0; i < counter; i++){
        getline(ifs, line);
    }

    // Aplicamos equalization con el valor máximo obtenido
    for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++){
            ifs >> r >> g >> b;

            r = r * m / c;
            g = g * m / c;
            b = b * m / c;
            
            equalization(r, g, b, max);
            
            r *= 255;
            g *= 255;
            b *= 255;
            

            ofs << fixed << (int)r << " " << (int)g << " "  << (int)b << "     ";
        }
        ofs << endl;
    }
}

void clamping_equalization(double &r, double &g, double &b, double max){
    r = r>max ? 1 : r/max;
    g = g>max ? 1 : g/max;
    b = b>max ? 1 : b/max;
}

void transform_clamp_equal(string inFile, string outFile, int threshold){
    int w, h;
    int counter = 3;
    double m, c;
    bool still = true;
    // reads a ppm file
    ifstream ifs(inFile);
    // writes a ppm file
    ofstream ofs(outFile);
    // reads the header
    string header;
    getline(ifs, header);
    ofs << header << endl;
    // lee la siguiente linea; si es un comentario, lo ignora a no ser que sea #MAX
    string line;
    while (still) {
        getline(ifs, line);
        if (line[0] != '#') {
            still = false;
        } else if (line.substr(0, 5) == "#MAX=") {
            ofs << "#MAX=1\n";
            m = stod(line.substr(5, -1));
            counter++;
        } else {
            ofs << line << endl;
            counter++;
        }
    }
    // lee el ancho y el alto
    w = stoi(line.substr(0, line.find(' ')));
    h = stoi(line.substr(line.find(' ') + 1, -1));

    ofs << w << " " << h << endl;
    // lee el valor de c
    getline(ifs, line);
    c = stod(line);

    ofs << 255 << endl;

    double r, g, b;

    // Aplicamos equalization con el valor aportado como umbral para pasar de equalization a clamping.
    for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++){
            ifs >> r >> g >> b;

            r = r * m / c;
            g = g * m / c;
            b = b * m / c;
            
            clamping_equalization(r, g, b, threshold);
            
            r *= 255;
            g *= 255;
            b *= 255;
            

            ofs << fixed << (int)r << " " << (int)g << " "  << (int)b << "     ";
        }
        ofs << endl;
    }
}

void gamma(double &r, double &g, double &b, double gamma_coef){
    double factor = 255.0 / pow(255.0, 1/gamma_coef);
    r = pow (r, 1/gamma_coef) * factor;
    g = pow (g, 1/gamma_coef) * factor;
    b = pow (b, 1/gamma_coef) * factor;
}

void transform_gamma(string inFile, string outFile, double gamma_coef){
    int w, h;
    int counter = 3;
    double m, c;
    bool still = true;
    // reads a ppm file
    ifstream ifs(inFile);
    // writes a ppm file
    ofstream ofs(outFile);
    // reads the header
    string header;
    getline(ifs, header);
    ofs << header << endl;
    // lee la siguiente linea; si es un comentario, lo ignora a no ser que sea #MAX
    string line;
    while (still) {
        getline(ifs, line);
        if (line[0] != '#') {
            still = false;
        } else if (line.substr(0, 5) == "#MAX=") {
            ofs << "#MAX=1\n";
            m = stod(line.substr(5, -1));
            counter++;
        } else {
            ofs << line << endl;
            counter++;
        }
    }
    // lee el ancho y el alto
    w = stoi(line.substr(0, line.find(' ')));
    h = stoi(line.substr(line.find(' ') + 1, -1));

    ofs << w << " " << h << endl;
    // lee el valor de c
    getline(ifs, line);
    c = stod(line);

    ofs << 255 << endl;

    double r, g, b;
    double max = 0;
    // lee el valor de cada pixel para encontrar el máximo
    for (int i = 0; i < h; i++){
        for (int j = 0; j < w; j++){
            ifs >> r >> g >> b;
            r = r * m / c;
            g = g * m / c;
            b = b * m / c;
            if(r > max) max = r;
            if(g > max) max = g;
            if(b > max) max = b;
        }
    }

    cout << max << endl;
    // Deja de asociarse con el fichero
    ifs.close();
    // Reseteamos el ifstream
    ifs.open(inFile);

    // Saltamos las lineas previas a los píxeles
    for (int i = 0; i < counter; i++){
        getline(ifs, line);
    }

    // Aplicamos equalization con el valor máximo obtenido
    // A continuación se aplica la transformación gamma
    for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++){
            ifs >> r >> g >> b;

            r = r * m / c;
            g = g * m / c;
            b = b * m / c;
            
            equalization (r, g, b, max);
            
            r *= 255;
            g *= 255;
            b *= 255;

            gamma (r, g, b, gamma_coef);

            ofs << fixed << (int)r << " " << (int)g << " "  << (int)b << "     ";
        }
        ofs << endl;
    }
}

void transform_clamp_gamma(string inFile, string outFile, int threshold, double gamma_coef){
    int w, h;
    int counter = 3;
    double m, c;
    bool still = true;
    // reads a ppm file
    ifstream ifs(inFile);
    // writes a ppm file
    ofstream ofs(outFile);
    // reads the header
    string header;
    getline(ifs, header);
    ofs << header << endl;
    // lee la siguiente linea; si es un comentario, lo ignora a no ser que sea #MAX
    string line;
    while (still) {
        getline(ifs, line);
        if (line[0] != '#') {
            still = false;
        } else if (line.substr(0, 5) == "#MAX=") {
            ofs << "#MAX=1\n";
            m = stod(line.substr(5, -1));
            counter++;
        } else {
            ofs << line << endl;
            counter++;
        }
    }
    // lee el ancho y el alto
    w = stoi(line.substr(0, line.find(' ')));
    h = stoi(line.substr(line.find(' ') + 1, -1));

    ofs << w << " " << h << endl;
    // lee el valor de c
    getline(ifs, line);
    c = stod(line);

    ofs << 255 << endl;

    double r, g, b;

    // Aplicamos equalization con el valor aportado como umbral para pasar de equalization a clamping.
    for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++){
            ifs >> r >> g >> b;

            r = r * m / c;
            g = g * m / c;
            b = b * m / c;
            
            clamping_equalization(r, g, b, threshold);
            
            r *= 255;
            g *= 255;
            b *= 255;

            gamma(r, g, b, gamma_coef);

            ofs << fixed << (int)r << " " << (int)g << " "  << (int)b << "     ";
        }
        ofs << endl;
    }
}


int main(int argc, char* argv[]){
    string opcion = argv[1];

    if(opcion == "clamping") {
        if (argc == 4) {
            cout << "Realizando una transformación de clamping sobre el fichero " << argv[2] << endl;
            transform_clamping(argv[2], argv[3]);
            cout << "Completado. Resultados en el fichero " << argv[3] << "." << endl;
        } else {
            cout << "Número de argumentos incorrecto, uso de clamping:" << endl;
            cout << argv[0] << " clamping <fichero entrada> <fichero salida>" << endl;
        }

    } else if(opcion == "equalization") {
        if (argc == 4) {
            cout << "Realizando una transformación de ecualización sobre el fichero " << argv[2] << endl;
            transform_equalization(argv[2], argv[3]);
            cout << "Completado. Resultados en el fichero " << argv[3] << "." << endl;
        } else {
            cout << "Número de argumentos incorrecto, uso de ecualización:" << endl;
            cout << argv[0] << " equalization <fichero entrada> <fichero salida>" << endl;
        }

    } else if (opcion == "clamping-equalization") {
        if (argc == 5) {
            cout << "Realizando una transformación de clamping-ecualización con umbral " << argv[4] << " sobre el fichero " << argv[2] << endl;
            transform_clamp_equal(argv[2], argv[3], stoi(argv[4]));
            cout << "Completado. Resultados en el fichero " << argv[3] << "." << endl;
        } else {
            cout << "Número de argumentos incorrecto, uso de clamping-ecualización:" << endl;
            cout << argv[0] << " clamping-equalization <fichero entrada> <fichero salida> <valor umbral>" << endl;
        }

    } else if (opcion == "gamma") {
        if (argc == 5) {
            cout << "Realizando una transformación gamma con coeficiente " << argv[4] << " sobre el fichero " << argv[2] << endl;
            transform_gamma(argv[2], argv[3], stof(argv[4]));
            cout << "Completado. Resultados en el fichero " << argv[3] << "." << endl;
        } else {
            cout << "Número de argumentos incorrecto, uso de corrección gamma:" << endl;
            cout << argv[0] << " gamma <fichero entrada> <fichero salida> <coeficiente gamma>" << endl;
        }

    } else if (opcion == "clamping-gamma") {
        if (argc == 6) {
            cout << "Realizando una transformación de clamping-gamma con umbral " << argv[4] << " y coeficiente " << argv[5] << " sobre el fichero " << argv[2] << endl;
            transform_clamp_gamma(argv[2], argv[3], stoi(argv[4]), stof(argv[5]));
            cout << "Completado. Resultados en el fichero " << argv[3] << "." << endl;
        } else {
            cout << "Número de argumentos incorrecto, uso de clamping-gamma:" << endl;
            cout << argv[0] << " clamping-gamma <fichero entrada> <fichero salida> <umbral> <corf. gamma>" << endl;
        }

    } else {
        cout << "Opción no válida. Opciones disponibles:" << endl;
        cout << argv[0] << " clamping <fichero entrada> <fichero salida>" << endl;
        cout << argv[0] << " equalization <fichero entrada> <fichero salida>" << endl;
        cout << argv[0] << " clamping-equalization <fichero entrada> <fichero salida> <valor umbral>" << endl;
        cout << argv[0] << " gamma <fichero entrada> <fichero salida> <coeficiente gamma>" << endl;
        cout << argv[0] << " clamping-gamma <fichero entrada> <fichero salida> <umbral> <corf. gamma>" << endl;
    }
}