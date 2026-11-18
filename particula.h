#ifndef PARTICULAR_H
#define PARTICULAR_H

class particula {
private:
    int id;
    double x, y;
    double vx, vy;
    double masa;
    double radio;
    double coefRestitucion;
    double gravedad;

public:
    particula(int id, double x, double y, double vx, double vy,
              double masa, double radio, double e);

    // Aplica el movimiento (integración de Euler)
    void actualizar(double dt);

    // Calcula el módulo de la velocidad
    double obtenerVelocidad() const;

    int obtenerId() const { return id; }
    double obtenerX() const { return x; }
    double obtenerY() const { return y; }
    double obtenerVx() const { return vx; }
    double obtenerVy() const { return vy; }
    double obtenerMasa() const { return masa; }
    double obtenerRadio() const { return radio; }
    double obtenerCoefRestitucion() const { return coefRestitucion; }

    void establecerPosicion(double nx, double ny) { x = nx; y = ny; }
    void establecerVelocidad(double nvx, double nvy) { vx = nvx; vy = nvy; }
    void establecerMasa(double m) { masa = m; }
    void establecerRadio(double r) { radio = r; }
    void establecerCoefRestitucion(double e) { coefRestitucion = e; }
    void establecerGravedad(double g) { gravedad = g; }
};

#endif