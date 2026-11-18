#ifndef OBSTACULO_H
#define OBSTACULO_H

class obstaculo {
private:
    double x, y;
    double ancho, alto;

public:
    obstaculo(double x, double y, double ancho, double alto);

    double obtenerX() const { return x; }
    double obtenerY() const { return y; }
    double obtenerAncho() const { return ancho; }
    double obtenerAlto() const { return alto; }
    double obtenerXMax() const { return x + ancho; }
    double obtenerYMax() const { return y + alto; }
};

#endif