#ifndef FIGURA_H // Escudo protector: evita que este archivo se lea más de una vez.
#define FIGURA_H

#include <cmath>        // round (redondear), sin y cos (para rotar).
#include <vector>       // la lista de puntos del rectángulo.
#include "Grilla.h"     // Necesito el lienzo para poder dibujar en él.
#include "Bresenham.h"  // Necesito mi matemática de líneas y círculos.

// Los tipos de figura que mi paint sabe hacer.
enum TipoFigura { LINEA, CIRCULO, RECTANGULO, LAPIZ };

// Una Figura es un paquete que guarda TODO lo necesario para dibujar algo.
struct Figura {
    TipoFigura tipo; // ¿Es una línea o un círculo?
    float x0, y0;    // LINEA: punto inicial. CIRCULO: centro. (decimales para poder rotar sin deformar)
    float x1, y1;    // LINEA: punto final. CIRCULO: no se usa.
    float radio;     // CIRCULO: el radio (decimal para poder escalar sin deformar). LINEA: no se usa.
    float r, g, b;   // El color de la figura (rojo, verde, azul).
    std::vector<float> puntosX; // RECTANGULO: la X de cada una de sus 4 esquinas (en orden alrededor del borde).
    std::vector<float> puntosY; // RECTANGULO: la Y de cada esquina.

    // La figura sabe dibujarse a sí misma usando mi Bresenham.
    void dibujar(Grilla& grilla) {
        // Bresenham trabaja con celdas ENTERAS, así que redondeo mis decimales justo al dibujar.
        int ix0 = (int)std::round(x0);
        int iy0 = (int)std::round(y0);
        int ix1 = (int)std::round(x1);
        int iy1 = (int)std::round(y1);
        if (tipo == LINEA) {
            Bresenham::linea(ix0, iy0, ix1, iy1, grilla, r, g, b);
        } else if (tipo == CIRCULO) {
            Bresenham::circulo(ix0, iy0, (int)std::round(radio), grilla, r, g, b);
        } else if (tipo == RECTANGULO) { // Un rectángulo son 4 líneas, una entre cada par de esquinas vecinas.
            int n = (int)puntosX.size();
            for (int i = 0; i < n; ++i) {
                int j = (i + 1) % n; // La esquina siguiente. El % hace que la última se una con la primera (cierra el borde).
                Bresenham::linea((int)std::round(puntosX[i]), (int)std::round(puntosY[i]),
                                 (int)std::round(puntosX[j]), (int)std::round(puntosY[j]), grilla, r, g, b);
            }
        } else if (tipo == LAPIZ) { // NUEVO: el lápiz une cada punto con el siguiente, pero NO cierra el borde.
            int n = (int)puntosX.size();
            if (n == 1) { // Un solo clic sin arrastrar: no hay con quién unirlo, pinto solo esa celda.
                grilla.paintPixel((int)std::round(puntosX[0]), (int)std::round(puntosY[0]), r, g, b);
            }
            for (int i = 0; i < n - 1; ++i) { // n - 1: la última NO se une con la primera.
                Bresenham::linea((int)std::round(puntosX[i]),     (int)std::round(puntosY[i]),
                                 (int)std::round(puntosX[i + 1]), (int)std::round(puntosY[i + 1]), grilla, r, g, b);
            }
        }
    }

    // TRASLACIÓN: desplazo todos los puntos sumándoles dx en X y dy en Y.
    void mover(float dx, float dy) {
        x0 += dx; y0 += dy;
        x1 += dx; y1 += dy;
        for (int i = 0; i < (int)puntosX.size(); ++i) { // NUEVO: también muevo cada esquina del rectángulo.
            puntosX[i] += dx;
            puntosY[i] += dy;
        }
    }

    // ROTACIÓN alrededor de un pivote (cx, cy), en grados.
    // Son 3 pasos (transformada compuesta): 1) llevo el pivote al origen (resto cx, cy),
    // 2) giro con seno y coseno, 3) devuelvo el pivote a su lugar (sumo cx, cy).
    // Las dos fórmulas del paso 2 son lo que sale de multiplicar la matriz de rotación 2D por el punto.
    void rotar(float grados, float cx, float cy) {
        float rad = grados * 3.14159265f / 180.0f; // Las funciones sin y cos piden radianes, no grados.
        float c = std::cos(rad);
        float s = std::sin(rad);

        float px = x0 - cx; // Punto 0 relativo al pivote.
        float py = y0 - cy;
        x0 = cx + px * c - py * s;
        y0 = cy + px * s + py * c;

        if (tipo == LINEA) { // El círculo solo tiene un punto (su centro); la línea tiene dos.
            px = x1 - cx;
            py = y1 - cy;
            x1 = cx + px * c - py * s;
            y1 = cy + px * s + py * c;
        }
        for (int i = 0; i < (int)puntosX.size(); ++i) { // NUEVO: giro cada esquina del rectángulo con la misma fórmula.
            float qx = puntosX[i] - cx;
            float qy = puntosY[i] - cy;
            puntosX[i] = cx + qx * c - qy * s;
            puntosY[i] = cy + qx * s + qy * c;
        }
    }
    // ESCALA alrededor de un pivote (cx, cy). f = factor: 2.0 agranda al doble, 0.5 achica a la mitad.
    // Mismos 3 pasos: 1) pivote al origen (resto), 2) multiplico por f, 3) devuelvo el pivote (sumo).
    void escalar(float f, float cx, float cy) {
        x0 = cx + (x0 - cx) * f;
        y0 = cy + (y0 - cy) * f;
        x1 = cx + (x1 - cx) * f;
        y1 = cy + (y1 - cy) * f;
        radio = radio * f; // El círculo también crece o se encoge: su radio se multiplica por el factor.
        for (int i = 0; i < (int)puntosX.size(); ++i) { // Escalo cada esquina del rectángulo.
            puntosX[i] = cx + (puntosX[i] - cx) * f;
            puntosY[i] = cy + (puntosY[i] - cy) * f;
        }
    }
};

#endif