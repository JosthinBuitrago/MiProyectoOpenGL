#ifndef FIGURA_H // Escudo protector: evita que este archivo se lea más de una vez.
#define FIGURA_H

#include <cmath>        // round (redondear), sin y cos (para rotar).
#include "Grilla.h"     // Necesito el lienzo para poder dibujar en él.
#include "Bresenham.h"  // Necesito mi matemática de líneas y círculos.

// Los tipos de figura que mi paint sabe hacer.
enum TipoFigura { LINEA, CIRCULO };

// Una Figura es un paquete que guarda TODO lo necesario para dibujar algo.
struct Figura {
    TipoFigura tipo; // ¿Es una línea o un círculo?
    float x0, y0;    // LINEA: punto inicial. CIRCULO: centro. (decimales para poder rotar sin deformar)
    float x1, y1;    // LINEA: punto final. CIRCULO: no se usa.
    int radio;       // CIRCULO: el radio. LINEA: no se usa.
    float r, g, b;   // El color de la figura (rojo, verde, azul).

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
            Bresenham::circulo(ix0, iy0, radio, grilla, r, g, b);
        }
    }

    // TRASLACIÓN: desplazo todos los puntos sumándoles dx en X y dy en Y.
    void mover(float dx, float dy) {
        x0 += dx; y0 += dy;
        x1 += dx; y1 += dy;
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
    }
};

#endif