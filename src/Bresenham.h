#ifndef BRESENHAM_H //Directiva de seguridad para evitar dobles inclusiones.
#define BRESENHAM_H

#include <cmath> //Librería matemática estándar de C++. Nos permite usar std::abs() (Valor Absoluto).
#include "Grilla.h" //Conecta este archivo con la Grilla.

class Bresenham {
public:
    //FUNCION DE LINEA
static void linea(int x0, int y0, int x1, int y1, Grilla& grilla, float r = 0.0f, float g = 1.0f, float b = 1.0f) {
    int dx, sx;
    int dy, sy;

    // // Evalúo el eje X: Si la meta está más a la derecha, resto normal y mi paso es +1.
    // // Si está a la izquierda, invierto la resta (inicio - meta) para forzar un resultado positivo y mi paso es -1.
    if (x0 < x1) {
        dx = x1 - x0;
        sx = 1;
    } else {
        dx = x0 - x1;
        sx = -1;
    }

    // // Evalúo el eje Y: Misma lógica. Si voy hacia un número mayor, resto meta - inicio.
    // // Si voy hacia un número menor, resto inicio - meta. Así siempre obtengo distancias positivas.
    if (y0 < y1) {
        dy = y1 - y0;
        sy = 1;
    } else {
        dy = y0 - y1;
        sy = -1;
    }
    
    // // Inicializo el "termómetro" del error restando las distancias calculadas manualmente.
    int err = dx - dy; 

    // // Empieza el ciclo para pintar la línea punto por punto
    while (true) {
        // // Enciendo el píxel en la coordenada actual
        grilla.paintPixel(x0, y0, r, g, b); 
        
        // // Si mi coordenada actual ya es igual a la meta, rompo el ciclo
        if (x0 == x1 && y0 == y1) break; 

        // // Multiplico el error por 2 (truco maestro para evitar decimales)
        int e2 = 2 * err; 
        
        // // Si el error me lo permite, avanzo en el eje X
        if (e2 > -dy) { 
            err -= dy; 
            x0 += sx;  
        }
        
        // // Si el error me lo permite, avanzo en el eje Y
        if (e2 < dx) { 
            err += dx; 
            y0 += sy;  
        }
    }
}
    //FUNCION DE CIRCULO
    static void circulo(int centroX, int centroY, int radio, Grilla& grilla, float r = 0.0f, float g = 1.0f, float b = 1.0f) {
        //centro x,y = coordenadas del centro
        //radio = radio
        //Grilla es el lienzo y el & para modificarlo directamente
        int x = 0; //comenzamos arria donde x vale 0
        int y = radio; // y seria el punto mas alto
        // Parámetro de decisión de Bresenham para el círculo. Define si el borde ideal pasa por encima o por debajo del píxel.
        int d = 3 - 2 * radio;

        while (y >= x) { //Mientras estemos dibujando el primer octante (hasta llegar a los 45 grados donde X alcanza a Y).
            //Magia de la simetría: Con calcular un solo punto (x, y), iluminamos sus reflejos en los 8 octantes del círculo simultáneamente.
            grilla.paintPixel(centroX + x, centroY + y, r, g, b); //Octante 1
            grilla.paintPixel(centroX - x, centroY + y, r, g, b); //Octante 2
            grilla.paintPixel(centroX + x, centroY - y, r, g, b); //Octante 3
            grilla.paintPixel(centroX - x, centroY - y, r, g, b); //Octante 4
            grilla.paintPixel(centroX + y, centroY + x, r, g, b); //Octante 5
            grilla.paintPixel(centroX - y, centroY + x, r, g, b); //Octante 6
            grilla.paintPixel(centroX + y, centroY - x, r, g, b); //Octante 7
            grilla.paintPixel(centroX - y, centroY - x, r, g, b); //Octante 8

            x++; //Siempre avanzamos un paso hacia la derecha en cada ciclo.
            if (d > 0) { //Si el parametro es positivo significa que estamos fuera del circulo (nos movemos en x y y)
                y--; //tenemos que bajar para acercarnos a la curva
                d = d + 4 * (x - y) + 10; //ajustamos el parametro para el siguiente ciclo
            } else { //por el contrario si es menor estamos dentro del circulo (solo nos movemos en x)
                d = d + 4 * x + 6;//ajustamos el parametro para el siguiente ciclo
            }
        }
    }
};

#endif