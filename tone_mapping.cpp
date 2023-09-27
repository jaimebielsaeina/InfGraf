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


void transform(string inFile, string outFile) {
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
        cout << line.substr(0, 5) << endl;
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
            if(i == 0 && j == 0){
                cout << r << " " << g << " " << b << endl;
            }

            clamping(r, g, b);
            
            r *= 255;
            g *= 255;
            b *= 255;
            

            ofs << fixed << (int)r << " " << (int)g << " "  << (int)b << "     ";
        }
        ofs << endl;
    }
    

}

int main(int argc, char* argv[]){
    transform(argv[1], argv[2]);
}