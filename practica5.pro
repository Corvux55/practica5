TEMPLATE = app
CONFIG += c++17

CONFIG -= app_bundle
QT += core gui widgets charts

SOURCES += \
    main.cpp \
    obstaculo.cpp \
    particula.cpp \
    simulacion.cpp

HEADERS += \
    obstaculo.h \
    particula.h \
    simulacion.h

DISTFILES += \
    data/salida.csv