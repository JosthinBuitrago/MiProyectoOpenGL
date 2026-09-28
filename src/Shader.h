#ifndef SHADER_H //escudos protectores
#define SHADER_H //evitan que sea lea este archivo mas de una vez y todo colapse
#include <glad/glad.h> //activa las herramientas graficas
#include <string> //permite usar texto moderno
#include <fstream> //abre la conexion fisica con el disco duro
#include <sstream>   //crea un espacio en la ram para usar ese texto
#include <iostream> //nos sirve para todo

class Shader { //creacion de la fabrica
private: //funciones de uso interno
    // Función auxiliar interna (privada) para leer los archivos
    std::string leerArchivo(const char* ruta) { //es un mensajero que me trae el texto y me lo trae aca
        std::ifstream archivo(ruta);//crea un tubo que conceta el programa con l archivo
        if (!archivo.is_open()) {//salvavidas por si el archivo no existe o la ruta esta mal
            std::cout << "ERROR::ARCHIVO::NO_SE_PUDO_ABRIR: " << ruta << std::endl;
            return "";
        }
        std::stringstream flujo;//balde temporal en la memoria
        flujo << archivo.rdbuf();//el .rdbuf lee todo el texto y lo vacia en el valde
        archivo.close();//cierra el archivo
        return flujo.str();//toma lo del valde y lo convierte en un archivo moderno de c++, y lo entrega
    }

public:
    unsigned int ID; // Identificador único del Shader Program en la GPU

    Shader(const char* rutaVert, const char* rutaFrag) {// Constructor: se ejecuta automáticamente al crear el objeto
        std::string textoVert = leerArchivo(rutaVert);
        std::string textoFrag = leerArchivo(rutaFrag);//llama a leerarchivo para obtener el texto de los dos archivos

        const char* cVert = textoVert.c_str();
        const char* cFrag = textoFrag.c_str();//y con c_str lo convertimos en fomrato C que openGL exige

        // 1. Compilar Topógrafo
        unsigned int vert = glCreateShader(GL_VERTEX_SHADER); //creamos em la GPU
        glShaderSource(vert, 1, &cVert, NULL); //les ponemos el texto que acabamos de traer traducido
        glCompileShader(vert); //traduce el texto al idioma de la tarjeta grafica

        // 2. Compilar Pintor
        unsigned int frag = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frag, 1, &cFrag, NULL);
        glCompileShader(frag);

        // 3. Enlazar ambos en el programa final
        ID = glCreateProgram(); //crea la caja principal
        glAttachShader(ID, vert);
        glAttachShader(ID, frag);//donde conectamos a los dos
        glLinkProgram(ID);//y los fusionamos

        glDeleteShader(vert);//borramos los sahders antiguos para no gastar memoria
        glDeleteShader(frag);
    }

    // Método para activar este shader antes de dibujar
    void usar() {
        glUseProgram(ID);
    }
};

#endif