#include <glad/glad.h> //directorio de funciones graficas
#include <GLFW/glfw3.h> //encargada de comunicacion con el sistema operativo
#include <iostream> //herramienta nativa de c++
#include <fstream> // craga de herramientas para abriri y leer archivos fisicos
#include <sstream> //carga herramienats para crear streams en la memoria RAM
#include <string> //permite utilizar tipos de datos mas libres
#include "Shader.h" //Importa nuestra propia clase personalizada

int main() {

    //Inicializacion de libreria de creacion de ventana
    if (!glfwInit()) return -1; //chequeo: por si no esta creado
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //reglas antes de construccion de la ventana
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //version 3.3 de oprnGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //perfil moderno CORE
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //para que la mac coora el codigo, sono esta esto: paila

    GLFWwindow* window = glfwCreateWindow(800, 600, "Mi Primer Triangulo OpenGL", NULL, NULL); //Construccion de la ventana en la variable window
    //Los parentesis exigen 5 datos exactos (ancho, alto, Titulo, monitor, compartir)
    if (!window) { glfwTerminate(); return -1; }//chequeo: por si no se abre termina el preceso y cierra el programa
    glfwMakeContextCurrent(window);//todo lo que se va a hacer en la ventana se hara en la ventana llamada window, no en otra

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;//chequeo: asegurandose de que el sistema le de las direcciones de la tarjeta de video para trabajar

    Shader shaderProgram("../shaders/topografo.vert", "../shaders/pintor.frag");//creamos un objeto con caracteristics de nuestra clase personalizada
    //tiene como funcion leer los dos textos, el de los vertices y el color

    // --- PREPARANDO EL MATERIAL --------------------------------------------------------------------------------------
    float vertices[] = { //creamos un arreglo con los 4 vertices que formaran la figura
          0.5f,  0.5f, 0.0f,
          0.5f, -0.5f, 0.0f,
         -0.5f, -0.5f, 0.0f,
         -0.5f,  0.5f, 0.0f
    };//el Z es cero porque estamos en 2D

    unsigned int indices[] = {
        0, 1, 3,   // Primer triángulo
        1, 2, 3    // Segundo triángulo
    };

    unsigned int VBO, VAO, EBO; //creamos nuestras herramientas de memoria
    glGenVertexArrays(1, &VAO); //crea el manual de instrucciones en la variable VAO
    glGenBuffers(1, &VBO);//crea la caja del almacenamiento en la variable VBO
    glGenBuffers(1, &EBO); // Generamos la caja para el EBO

    glBindVertexArray(VAO);//le decimos que todo lo que hagamos que lo anote en esa variable sin importar que

    glBindBuffer(GL_ARRAY_BUFFER, VBO);//le decimos a openGl que VBO es una caja para guaradr solo vertices
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//aqui es donde meteos los datos fisicamente
    //GL_ARRAY_BUFFER: tipo de caja
    //sizeof(vertices): el tamaño de nuestra lista de coordenadas
    //vertices: los datos que vamos a meter
    //GL_STATIC_DRAW: le decimos que en la jugada, que esos datos son estaticos y que sea rapido

    // Conectamos y llenamos el EBO (DEBE hacerse mientras el VAO está activado)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);//toma nuestra lista de indices la RAM y la mete en la caja de EBO

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);//interpretacion de todo a la GPU
    //0: datos dirigidos a la locacion 0
    //3: cuantos numeros forman un solo vertice(X,Y,Z)
    //GL_FLOAT: decimales los numeros
    //GL_FALSE: que no fuerce un rango en los numeros
    //3 * sizeof(float):STRIDE. A cada cuanto esta el siguiente punto (tres porque son tres coordenadas(X, Y, Z))
    //(void*)0:OffSet. Desde donde empezar a leer en la caja, osea 0

    glEnableVertexAttribArray(0);//prendemos la locacion donde estaban los datos, en el punto 0
    //-----------------------------------------------------------------------------------------------------------------------

    // --- BUCLE PRINCIPAL (El dibujo en vivo) ---------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {//mientras el usuario no haya cerrado la ventana se repite lo de adentro
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//el fondo es verde oscuro
        glClear(GL_COLOR_BUFFER_BIT);//borra todo lo que se tenia antes y la prepara para dibujar de nuevo

        shaderProgram.usar();//ejecuta el metodo usar de nuestra clase, que le ordena a nuestra GPU activar nuestro programa compilado de shaders
        glBindVertexArray(VAO);//Se activa el Vertex Array Object
        //carpeta donde estan las coordenadas exactas y la configuracion de los puntos
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//orden de dibujar
        //GL_TRIANGLES: dibuja triangulos
        //6: procesando 6 indices en total
        //GL_UNSIGNED_INT: que son enteros sin signo
        //0: y empieza desde el indice 0

        glfwSwapBuffers(window);//evita parpadeos
        glfwPollEvents();//escucha y procesa eventos del sistema
    }

    glfwTerminate();//una vez cierrada la ventana se finaliza todo los recursos listos para utilizar
    return 0;
}