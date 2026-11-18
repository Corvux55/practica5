#include "simulacion.h"
#include <iostream>
#include <QApplication>// Requerido para iniciar una aplicacion Qt
#include <QChartView>// El widget que muestra el gráfico en pantalla
#include <QLineSeries>// Para dibujar las líneas de las trayectorias
#include <QScatterSeries>// Para dibujar los puntos de colisión y fusión
#include <QChart>// El contenedor principal del gráfico
#include <QFile> // Para abrir el archivo CSV
#include <QTextStream>// Para leer el texto del archivo CSV
#include <QMap>// Para guardar las series por id de partícula
#include <QValueAxis>// Para configurar los ejes X e Y
#include <QAreaSeries>// Para dibujar los obstáculos como rectángulos


void mostrarGrafica(const QString& titulo, const QString& rutaCsv, bool dibujarObstaculos) {
    QChart *chart = new QChart();//crea el contenedor del grafico
    chart->setTitle(titulo);

    QMap<int, QLineSeries*> seriesTrayectorias;//diccionario para guardar una linea por cada ID de particula

    // Serie para las colisiones (puntos)
    QScatterSeries *serieColisiones = new QScatterSeries();
    serieColisiones->setMarkerShape(QScatterSeries::MarkerShapeCircle);//forma
    serieColisiones->setMarkerSize(6.0);//tamaño
    serieColisiones->setColor(Qt::red);
    serieColisiones->setName("Colisiones");

    // Serie para las fusiones
    QScatterSeries *serieFusiones = new QScatterSeries();
    serieFusiones->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    serieFusiones->setMarkerSize(10.0);
    serieFusiones->setColor(Qt::magenta);
    serieFusiones->setName("Fusiones");

    //leer el archivo CSV
    QFile file(rutaCsv);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        if (!in.atEnd()) in.readLine(); //saltar cabecera

        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(",");

            // Si la línea tiene las 8 columnas esperadas
            if (parts.size() == 8) {
                double x = parts[2].toDouble();//columna 2: Posición X
                double y = parts[3].toDouble();//columna 3: Posición Y
                int id = parts[1].toInt();//columna 1: ID de la partícula
                QString evento = parts[7];//columna 7: Tipo de evento

                // Crear serie de trayectoria si no existe para este ID, fusiones
                if (!seriesTrayectorias.contains(id)) {
                    QLineSeries *serie = new QLineSeries();
                    serie->setName("Particula " + QString::number(id));
                    seriesTrayectorias[id] = serie;
                    chart->addSeries(serie);
                }
                seriesTrayectorias[id]->append(x, y);

                // Clasificar los eventos
                if (evento.contains("colision")) {
                    serieColisiones->append(x, y);
                } else if (evento == "fusion_particulas") {
                    serieFusiones->append(x, y);
                }
            }
        }
        file.close();
    }

    // Dibujar los obstáculos (Solo si se pide con el booleano)
    if (dibujarObstaculos){
        struct Rect { double x, y, a, h; };
        QList<Rect> misObstaculos = {
            {200, 150, 80, 80}, {520, 150, 80, 80},
            {200, 370, 80, 80}, {520, 370, 80, 80}
        };
        for (const auto& rect : misObstaculos) {
            QLineSeries *superior = new QLineSeries();
            superior->append(rect.x, rect.y);
            superior->append(rect.x + rect.a, rect.y);
            QLineSeries *inferior = new QLineSeries();
            inferior->append(rect.x, rect.y + rect.h);
            inferior->append(rect.x + rect.a, rect.y + rect.h);
            QAreaSeries *areaObstaculo = new QAreaSeries(superior, inferior);
            areaObstaculo->setPen(QPen(Qt::black));//contorno
            areaObstaculo->setBrush(QBrush(QColor(100, 100, 100, 150)));
            chart->addSeries(areaObstaculo);
        }
    }

    chart->addSeries(serieColisiones);
    chart->addSeries(serieFusiones);
    chart->createDefaultAxes();//configura los ejes automáticamente basandose en los datos

    // Invertir eje Y
    QValueAxis *axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
    if (axisY) axisY->setReverse(true);
    // Crea la vista (la ventana) y muestra el grafico
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing); //suaviza las lineas
    chartView->resize(800, 600);
    chartView->setWindowTitle(titulo);
    chartView->show();
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    // SIMULACION 1: Rebotes Elasticos con Paredes
    simulacion sim1(800, 600, 0.01, "data/simulacion1.csv");
    particula p1(1, 100, 500, 150, -350, 1.0, 10, 1.0);//e=1
    p1.establecerGravedad(250.0);
    sim1.agregarParticula(p1);
    sim1.ejecutar(1000);//1000 pasos
    mostrarGrafica("Simulacion 1: Parabolico y Rebotes Elasticos", "data/simulacion1.csv", false);

    // SIMULACION 2: Choques Inelasticos con Obstaculos
    simulacion sim2(800, 600, 0.01, "data/simulacion2.csv");
    sim2.agregarObstaculo(obstaculo(200, 150, 80, 80));
    sim2.agregarObstaculo(obstaculo(520, 150, 80, 80));
    sim2.agregarObstaculo(obstaculo(200, 370, 80, 80));
    sim2.agregarObstaculo(obstaculo(520, 370, 80, 80));

    particula p2(1, 50, 550, 200, -350, 1.0, 12, 0.6);//e=0.6
    p2.establecerGravedad(250.0);
    sim2.agregarParticula(p2);
    sim2.ejecutar(1000);
    mostrarGrafica("Simulacion 2: Parabolico con Obstaculos Inelasticos", "data/simulacion2.csv", true);

    // SIMULACION 3: Tiro Parabolico y Fusion en el aire
    simulacion sim3(800, 600, 0.01, "data/simulacion3.csv");

    //dos partículas lanzadas en parabola desde extremos opuestos
    particula p3a(1, 100, 550, 150, -150, 2.0, 15, 0.7);
    particula p3b(2, 700, 550, -150, -150, 2.0, 15, 0.7);

    p3a.establecerGravedad(250.0);
    p3b.establecerGravedad(250.0);

    sim3.agregarParticula(p3a);
    sim3.agregarParticula(p3b);
    sim3.ejecutar(500);
    mostrarGrafica("Simulacion 3: Fusion en el Aire (Parabolico)", "data/simulacion3.csv",false);
    return app.exec();//mantiene la aplicacion Qt abierta hasta que se cierren todas las ventanas
}