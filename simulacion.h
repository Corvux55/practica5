#ifndef SIMULACION_H
#define SIMULACION_H

#include "particula.h"
#include "obstaculo.h"
#include <vector>
#include <string>
#include <fstream>

class simulacion {
private:
    std::vector<particula> particulas;
    std::vector<obstaculo> obstaculos;
    double anchoCaja, altoCaja;
    double dt;
    int siguienteIdFusion = 5; //en 5 porque ya hay 4 partículas
    std::ofstream archivo;

    void verificarColisionesParedes(int idx, std::string& evento);
    void verificarColisionesObstaculos(int idx, std::string& evento);
    void verificarColisionesParticulas(std::string& evento);

public:
    simulacion(double ancho, double alto, double dt, const std::string& rutaArchivo);
    void agregarParticula(const particula& p);
    void agregarObstaculo(const obstaculo& o);
    void ejecutar(int numPasos);
};

#endif