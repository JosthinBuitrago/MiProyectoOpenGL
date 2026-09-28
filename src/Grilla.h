#ifndef GRILLA_H // Directiva de seguridad: Si no se ha leído, léelo ahora.
#define GRILLA_H

#include <glad/glad.h> //Librería que carga las funciones modernas de la tarjeta gráfica (OpenGL).
#include <vector> //Librería estándar de C++ para crear listas dinámicas (arreglos que pueden cambiar de tamaño).
#include "Cuadrado.h" //Importo mi clase Cuadrado porque la grilla está hecha de cuadrados.

class Grilla {
private:
    int columnas; //ancho de la grilla
    int filas; //alto de la grilla
    std::vector <Cuadrado> pixeles; //lista gigante donde se acomodan los cuadrados uno detras de otro
    //identificadores para la memoria de la tarjeta grafica
    unsigned int VAO, VBO;
    //VBO=Caja donde se almacenan los datos
    //VAO=instrucciones de como leer los datos de la caja

public:
    //Constructor que se ejecuta al crear la grilla (recibe las columnas y filas)
    Grilla(int cols, int fils) : columnas(cols), filas(fils) {
        pixeles.resize(columnas * filas); //numero exacto de todas las celdas
        //open gl va de -1 a 1, osea 2, eso lo divido por el total de filas y columnas
        float anchoCelda = 2.0f / columnas;
        float altoCelda = 2.0f / filas;
        float margen = 0.05f; //le quito parte de contenido de cada celda para que se vea como una grilla

        for (int y = 0; y < filas; ++y) {//recorro cada columna y fila para calcular las coordenadas matematicas de cada celda
            for (int x = 0; x < columnas; ++x) {
                int indice = (y * columnas) + x; //convierto coordenadas 2d en una linea 1d
                //calculo los bordes nuevos inciyendo la margen
                float xIzquierda = -1.0f + (x * anchoCelda) + (anchoCelda * margen);
                float xDerecha   = -1.0f + ((x + 1) * anchoCelda) - (anchoCelda * margen);
                float yAbajo     = -1.0f + (y * altoCelda) + (altoCelda * margen);
                float yArriba    = -1.0f + ((y + 1) * altoCelda) - (altoCelda * margen);
                // Armo el primer triángulo del cuadrado (Mitad inferior izquierda).
                pixeles[indice].t1.v1 = {xIzquierda, yAbajo, 0.0f};
                pixeles[indice].t1.v2 = {xDerecha, yAbajo, 0.0f};
                pixeles[indice].t1.v3 = {xIzquierda, yArriba, 0.0f};
                // Armo el segundo triángulo del cuadrado (Mitad superior derecha).
                pixeles[indice].t2.v1 = {xDerecha, yAbajo, 0.0f};
                pixeles[indice].t2.v2 = {xDerecha, yArriba, 0.0f};
                pixeles[indice].t2.v3 = {xIzquierda, yArriba, 0.0f};
                //pinto el cuadrado de verde oscuro
                pixeles[indice].setColor(0.0f, 0.2f, 0.0f);
            }
        }
        // --- PREPARACIÓN DE LA TARJETA GRÁFICA ---
        glGenVertexArrays(1, &VAO); //creamos el vao
        glGenBuffers(1, &VBO); //creamos el vbo

        glBindVertexArray(VAO);//activo el manual de instrucciones para escribir en el
        glBindBuffer(GL_ARRAY_BUFFER, VBO);//activo la caja fuerte
        //copio toda la lista de pixeles de la ram(cpu) a la vram(gpu)
        glBufferData(GL_ARRAY_BUFFER, pixeles.size() * sizeof(Cuadrado), pixeles.data(), GL_DYNAMIC_DRAW);
        // Le explico a la GPU el atributo 0: "Los primeros 3 números (x,y,z) son la posición de la esquina".
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)0);
        glEnableVertexAttribArray(0);
        // Le explico a la GPU el atributo 1: "Saltando los 3 primeros números, los siguientes 3 (r,g,b) son el color".
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);// Desactivo el manual de instrucciones por seguridad para no modificarlo por error.
    }
    // Destructor: Se ejecuta al cerrar el programa para liberar la memoria de la tarjeta gráfica y no dejar basura.
    ~Grilla() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
    //FUNCION PARA PINTAR CELDAS ESPECIFICAS
    void paintPixel(int x, int y, float r, float g, float b) {
        // Validación de seguridad: Si Bresenham intenta dibujar fuera de los límites de mi grilla, lo ignoro.
        if (x < 0 || x >= columnas || y < 0 || y >= filas) return;
        int indice = (y * columnas) + x;// Calculo qué cuadrado de mi lista le toca.
        pixeles[indice].setColor(r, g, b);// Le cambio el color.
    }
    // Función para borrar la pantalla, volviendo a pintar todo de verde oscuro.
    void limpiar(float r = 0.0f, float g = 0.2f, float b = 0.0f) {
        for (auto& cuadrado : pixeles) {
            cuadrado.setColor(r, g, b);
        }
    }
    // Esta función es vital: Como Bresenham cambia los colores en la RAM, debo mandar esos cambios a la Tarjeta Gráfica.
    void actualizarVRAM() {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, pixeles.size() * sizeof(Cuadrado), pixeles.data());
    }
    // Le da la orden final a la tarjeta gráfica de dibujar los triángulos en el monitor.
    void dibujar() {
        glBindVertexArray(VAO);// Saco mi manual de instrucciones.
        // Ordeno dibujar triángulos. Por cada cuadrado hay 6 vértices, así que multiplico mi total de píxeles por 6.
        glDrawArrays(GL_TRIANGLES, 0, 6 * pixeles.size());
    }
};

#endif