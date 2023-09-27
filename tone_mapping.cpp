#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
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


int main(int argc, char* argv[]){
    string opcion = argv[1];
    if(opcion == "clamping"){
        transform_clamping(argv[2], argv[3]);
    } else if(opcion == "equalization"){
        transform_equalization(argv[2], argv[3]);
    }
}