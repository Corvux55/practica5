#include "simulacion.h"
#include <cmath>//pa raiz y valor absoluto
#include <algorithm>//min y max
#include <stdexcept>
#include <iostream>

simulacion::simulacion(double ancho, double alto, double dt, const std::string& rutaArchivo)
    : anchoCaja(ancho), altoCaja(alto), dt(dt), archivo(rutaArchivo) {
    if (!archivo.is_open())
        throw std::runtime_error("No se pudo abrir el archivo: " + rutaArchivo);
}
//vectores
void simulacion::agregarParticula(const particula& p){ particulas.push_back(p); }
void simulacion::agregarObstaculo(const obstaculo& o){ obstaculos.push_back(o); }

// 1.paredes elasticas (e = 1 sin perdida de energía)
void simulacion::verificarColisionesParedes(int idx, std::string& evento) {
    particula& p = particulas[idx];//indice
    double r = p.obtenerRadio();

    if (p.obtenerX() - r < 0){//pared izquierda si el borde izquierdo (x - radio) se sale de la caja (es menor que 0)
        p.establecerPosicion(r, p.obtenerY());//reposisciona en el borde
        p.establecerVelocidad(-p.obtenerVx(), p.obtenerVy());//invierte velocidad
        evento = "colision_pared_izq";
        std::cout << "c" << p.obtenerId() << " choco con pared (izquierda)\n";
    } else if (p.obtenerX() + r > anchoCaja){//pared derecha si el borde derecho (x + radio) se sale de la caja
        p.establecerPosicion(anchoCaja - r, p.obtenerY());//reposiciona en el borde
        p.establecerVelocidad(-p.obtenerVx(), p.obtenerVy());//invierte la velocidad
        evento = "colision_pared_der";
        std::cout << "c" << p.obtenerId() << " choco con pared (derecha)\n";
    }
//lo mismo que lo anterior pero ahora en y
    if (p.obtenerY() - r < 0) {
        p.establecerPosicion(p.obtenerX(), r);
        p.establecerVelocidad(p.obtenerVx(), -p.obtenerVy());
        evento = "colision_pared_sup";
        std::cout << "c" << p.obtenerId() << " choco con pared (arriba)\n";
    } else if (p.obtenerY() + r > altoCaja) {
        p.establecerPosicion(p.obtenerX(), altoCaja - r);
        p.establecerVelocidad(p.obtenerVx(), -p.obtenerVy());
        evento = "colision_pared_inf";
        std::cout << "c" << p.obtenerId() << " choco con pared (abajo)\n";
    }
}

// 2. obstaculos Inelasticas usando el 'e' de la partícula
void simulacion::verificarColisionesObstaculos(int idx, std::string& evento) {
    particula& p = particulas[idx];
    double r = p.obtenerRadio();
    double e = p.obtenerCoefRestitucion();

    for (const auto& obs : obstaculos) {
        // Límites del obstáculo
        double xmin = obs.obtenerX();
        double xmax = obs.obtenerXMax();
        double ymin = obs.obtenerY();
        double ymax = obs.obtenerYMax();

        // Punto más cercano del rectángulo a la partícula
        double cx = std::max(xmin, std::min(p.obtenerX(), xmax));
        double cy = std::max(ymin, std::min(p.obtenerY(), ymax));
        double dx = p.obtenerX() - cx;
        double dy = p.obtenerY() - cy;

        // Si hay colisión
        if (dx * dx + dy * dy < r * r) {
            // Calcular penetración en cada lado
            double penIzq = (p.obtenerX() + r) - xmin;
            double penDer = xmax - (p.obtenerX() - r);
            double penSup = (p.obtenerY() + r) - ymin;
            double penInf = ymax - (p.obtenerY() - r);
            double penMin = std::min({penIzq, penDer, penSup, penInf});

            // Aplicar rebote según el lado golpeado
            if (penMin == penIzq) {
                p.establecerPosicion(xmin - r, p.obtenerY());//saca la particula del obs
                p.establecerVelocidad(-e * p.obtenerVx(), p.obtenerVy());//invierte y reduce v
                evento = "colision_obs_izq";
                std::cout << "c" << p.obtenerId() << " choco con obstaculo (izquierda)\n";
            } else if (penMin == penDer){//lo mismo que lo anterior pero con ejes diferentes
                p.establecerPosicion(xmax + r, p.obtenerY());
                p.establecerVelocidad(-e * p.obtenerVx(), p.obtenerVy());
                evento = "colision_obs_der";
                std::cout << "c" << p.obtenerId() << " choco con obstaculo (derecha)\n";
            } else if (penMin == penSup) {
                p.establecerPosicion(p.obtenerX(), ymin - r);
                p.establecerVelocidad(p.obtenerVx(), -e * p.obtenerVy());
                evento = "colision_obs_sup";
                std::cout << "c" << p.obtenerId() << " choco con obstaculo (arriba)\n";
            } else {
                p.establecerPosicion(p.obtenerX(), ymax + r);
                p.establecerVelocidad(p.obtenerVx(), -e * p.obtenerVy());
                evento = "colision_obs_inf";
                std::cout << "c" << p.obtenerId() << " choco con obstaculo (abajo)\n";
            }
        }
    }
}

// 3. particulas Fusión totalmente inelástica
void simulacion::verificarColisionesParticulas(std::string& evento) {
    for (size_t i = 0; i < particulas.size(); ++i) {
        for (size_t j = i + 1; j < particulas.size(); ++j) {
            const particula& p1 = particulas[i];
            const particula& p2 = particulas[j];
            // calcula la distancia entre los centros de ambas partículas
            double dx = p2.obtenerX() - p1.obtenerX();
            double dy = p2.obtenerY() - p1.obtenerY();
            double dist = std::sqrt(dx * dx + dy * dy);

            if (dist < p1.obtenerRadio() + p2.obtenerRadio()){//si la distancia es menor a la suma de los radios, están chocando
                double m1 = p1.obtenerMasa(), m2 = p2.obtenerMasa();
                double M = m1 + m2;
                //posición del centro de masa donde nace la nueva partícula
                double nx = (m1 * p1.obtenerX() + m2 * p2.obtenerX())/ M;
                double ny = (m1 * p1.obtenerY() + m2 * p2.obtenerY())/ M;
                //conservación del momento lineal nueva velocidad del centro de masa
                double nvx = (m1 * p1.obtenerVx() + m2 * p2.obtenerVx()) / M;
                double nvy = (m1 * p1.obtenerVy() + m2 * p2.obtenerVy()) / M;
                //conservación del area,para el nuevo radio se usp r_nuevo^2 = r1^2 + r2^2
                double nr = std::sqrt(p1.obtenerRadio() * p1.obtenerRadio() +p2.obtenerRadio() * p2.obtenerRadio());

                // Usar un contador secuencial para asignar el nuevo ID
                int nuevoId = siguienteIdFusion;
                siguienteIdFusion++; // Aumentar el contador para la próxima fusión

                particula fusionada(nuevoId, nx, ny, nvx, nvy, M, nr, p1.obtenerCoefRestitucion());
                //borramos las particulas y agregamos la fusion
                particulas.erase(particulas.begin() + j);
                particulas.erase(particulas.begin() + i);
                particulas.push_back(fusionada);

                evento = "fusion_particulas";
                std::cout<< "Fusion: Particulas "<< p1.obtenerId() << " y " << p2.obtenerId()<< " -> Nueva Particula " << nuevoId << "\n";
                return;
            }
        }
    }
}

void simulacion::ejecutar(int numPasos){
    archivo << "t,id,x,y,vx,vy,masa,evento\n"; //escribe la cabecera del archivo CSV

    for (int paso = 0; paso < numPasos; ++paso) {
        double t = paso * dt;//calcula el tiempo actual
        for (auto& p : particulas) p.actualizar(dt);//mueve todas las partículas (aplica velocidad y gravedad)
        std::string evento = "ninguno";
        for (size_t i = 0; i < particulas.size(); ++i) {
            verificarColisionesParedes(i, evento);
            verificarColisionesObstaculos(i, evento);
        }
        verificarColisionesParticulas(evento);

        for (const auto& p : particulas) {
            archivo << t << "," << p.obtenerId() << ","<< p.obtenerX() << "," << p.obtenerY() << ","<< p.obtenerVx() << "," << p.obtenerVy() << ","<< p.obtenerMasa() << "," << evento << "\n";
        }
    }
    archivo.close();
}