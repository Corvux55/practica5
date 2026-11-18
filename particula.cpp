#include "particula.h"
#include <cmath>

particula::particula(int id, double x, double y, double vx, double vy,double masa, double radio, double e):
    id(id), x(x), y(y), vx(vx), vy(vy), masa(masa), radio(radio), coefRestitucion(e),gravedad(0.0){}
// Integración de Euler explícita:
//   x(t+dt) = x(t) + vx * dt
//   y(t+dt) = y(t) + vy * dt

void particula::actualizar(double dt){
    vy += gravedad * dt;
    x += vx * dt;
    y += vy * dt;
}

double particula::obtenerVelocidad() const {
    return std::sqrt(vx * vx + vy * vy);
}