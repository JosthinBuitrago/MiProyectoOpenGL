#ifndef BRESENHAM_H //Directiva de seguridad para evitar dobles inclusiones.
#define BRESENHAM_H

#include <cmath> //Librería matemática estándar de C++. Nos permite usar std::abs() (Valor Absoluto).
#include "Grilla.h" //Conecta este archivo con la Grilla.

class Bresenham {
public:
    //FUNCION DE LINEA
    static void linea(int x0, int y0, int x1, int y1, Grilla& grilla) {
            //x0, y0 = Punto de inicio 
            //x1, y1 = Punto de destino
            //Grilla es el lienzo y el & para modificarlo directamente
        // el std:abs elimina el signo negativo
        int dx = std::abs(x1 - x0); //Distancia total a recorrer en x
        int dy = std::abs(y1 - y0); //Distancia total a recorrer en y
        // Direccion de la linea
        int sx = (x0 < x1) ? 1 : -1; //si va de izquierda a derecha +1, si va a la izquierda -1
        int sy = (y0 < y1) ? 1 : -1; //si va para arriba +1, si va para abajo -1
        //Acomulador de error para la aproximacion, inicia en dx-dy
        int err = dx - dy;

        while (true) { //avanzamos pixel por pixel hasta el break
            grilla.paintPixel(x0, y0, 0.0f, 1.0f, 1.0f); //pintamos la coordenada actual
            if (x0 == x1 && y0 == y1) break; //si x0=x1 y y0=y1 terminamos ahi

            int e2 = 2 * err; //multiplicamos por 2 para evaluar el siguiente paso
            if (e2 > -dy) { //si el doble del error es mayor a -dy avanzamos horizontalmente
                err -= dy; //restamos la distancia en y para reajustar el error
                x0 += sx; } //damos el paso segun corresponda en el octante
            if (e2 < dx)  { //si el doble del error es menor a dx avanzamos verticalmente
                err += dx; //sumamos la distancia en x para reajustar el error
                y0 += sy; } //damos el paso segun corresponda en el octante
        }
    }
    //FUNCION DE CIRCULO
    static void circulo(int centroX, int centroY, int radio, Grilla& grilla) {
        //centro x,y = coordenadas del centro
        //radio = radio
        //Grilla es el lienzo y el & para modificarlo directamente
        int x = 0; //comenzamos arria donde x vale 0
        int y = radio; // y seria el punto mas alto
        // Parámetro de decisión de Bresenham para el círculo. Define si el borde ideal pasa por encima o por debajo del píxel.
        int d = 3 - 2 * radio;

        while (y >= x) { //Mientras estemos dibujando el primer octante (hasta llegar a los 45 grados donde X alcanza a Y).
            //Magia de la simetría: Con calcular un solo punto (x, y), iluminamos sus reflejos en los 8 octantes del círculo simultáneamente.
            grilla.paintPixel(centroX + x, centroY + y, 0.0f, 1.0f, 1.0f); //Octante 1
            grilla.paintPixel(centroX - x, centroY + y, 0.0f, 1.0f, 1.0f); //Octante 2
            grilla.paintPixel(centroX + x, centroY - y, 0.0f, 1.0f, 1.0f); //Octante 3
            grilla.paintPixel(centroX - x, centroY - y, 0.0f, 1.0f, 1.0f); //Octante 4
            grilla.paintPixel(centroX + y, centroY + x, 0.0f, 1.0f, 1.0f); //Octante 5
            grilla.paintPixel(centroX - y, centroY + x, 0.0f, 1.0f, 1.0f); //Octante 6
            grilla.paintPixel(centroX + y, centroY - x, 0.0f, 1.0f, 1.0f); //Octante 7
            grilla.paintPixel(centroX - y, centroY - x, 0.0f, 1.0f, 1.0f); //Octante 8

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