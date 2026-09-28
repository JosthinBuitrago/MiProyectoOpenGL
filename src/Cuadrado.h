#ifndef CUADRADO_H //Directiva de seguridad: "Si este archivo no ha sido leído antes por el compilador..."
#define CUADRADO_H //"...entonces defínelo y léelo ahora". Evita que el archivo se duplique y genere errores.

struct Vertice { //paquete de datos donde se almacena un vertice
    float x, y, z; //coordenadas de posicion (z debe ser 0)
    float r, g, b; //color (red, green, blue)
};

struct Triangulo { //paquete de datos donde se almacena un triangulo
    Vertice v1, v2, v3; //un triangulo se forma por tres vertices
};

class Cuadrado { //representa una celda en la grilla
public:
    Triangulo t1; //triangulo inferior izquierdo
    Triangulo t2; //triangulo superior derecho
    //funcion que le da el color elegido a todos los vertices al mismo tiempo
    void setColor(float rojo, float verde, float azul) {
        //los tres vertices del primer triangulo
        t1.v1.r = rojo; t1.v1.g = verde; t1.v1.b = azul;
        t1.v2.r = rojo; t1.v2.g = verde; t1.v2.b = azul;
        t1.v3.r = rojo; t1.v3.g = verde; t1.v3.b = azul;
        //los tres vertices del segundo triangulo
        t2.v1.r = rojo; t2.v1.g = verde; t2.v1.b = azul;
        t2.v2.r = rojo; t2.v2.g = verde; t2.v2.b = azul;
        t2.v3.r = rojo; t2.v3.g = verde; t2.v3.b = azul;
    }
};
#endif //Fin de la directiva de seguridad del inicio.