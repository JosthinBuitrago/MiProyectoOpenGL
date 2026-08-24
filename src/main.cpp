#include <glad/glad.h> //directorio de funciones graficas
#include <GLFW/glfw3.h> //encargada de comunicacion con el sistema operativo
#include <iostream> //herramienta nativa de c++

//GLSL (OpenGL Shading Language)------------------------------------------------------------------
// 1. EL PINTOR DE POSICIONES (Vertex Shader)
const char *vertexShaderSource = "#version 330 core\n" //version 3.3 moderno por el core
    "layout (location = 0) in vec3 aPos;\n" //en la ubiacion 0 diremos que:
    //in: la variable es una entrada (c++ a la GPU)
    //vec3: vector de tres componentes (X, Y, Z)
    //aPos: nombre que le dimos a la variable
    "void main()\n"//funcion principal de la GPU (la hara una vez por cada vertice)
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"//variable de openGL
    //se le da el vector mas el 1.0 para que openGl calcule perspectiva 3D
    "}\0";

// 2. EL PINTOR DE COLOR (Fragment Shader - pinta de naranja)
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    //out: sale de la tarjeta grafica a la pantalla para pintar un pixel
    //vec4: vector donde se almacena 4 colores (RGBA: Rojo, Verde, Azul, Transparencia)
    //FragColor: Nombre de la variable donde se almacena el color
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n" //la orden
    //las intensidades van de 0.0 a 1.0. Esta combinacion da naranja
    "}\n\0";
//------------------------------------------------------------------------------------------------------

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

    // --- COMPILANDO LOS SHADERS (Los programitas de la tarjeta gráfica) --------------------------------------------
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); //creacion de variable con numero sin signo
    //donde se guarda el topografo (la funcion crea un espacio vacio en la tarjeta grafica)
    //es como la contratacion de un empleado llamado vertexshader
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);//instrucciones al empleado
    //vertexshader: el empleado a contratar
    //cuantos textos o archivos le vamos a dar (solo uno)
    //&vertexShaderSource: El texto en si (lo que esta arriba del main)
    //NULL: longitud del texto (con el null llee el texto hast que se acabe)
    glCompileShader(vertexShader);//traduce el texto en idioma de la tarjeta grafica

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);//lo mismo pero ahora con la asignacion de la variable que maneja el color
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram(); //Creacion de variable donde va un programa completo de shaders
    glAttachShader(shaderProgram, vertexShader);//Conectamos la variable del vector a este programa
    glAttachShader(shaderProgram, fragmentShader);//lo mismo pero con la variable del color
    glLinkProgram(shaderProgram);//aqui conceta los dos shaders (internamente recibe el vector y devuelve la grfica ya con el color)

    glDeleteShader(vertexShader);//borramos lo que ya no se necesita por la fusion
    glDeleteShader(fragmentShader);//no dejamos basura acumulada
    //-----------------------------------------------------------------------------------------------------------------------

    // --- PREPARANDO EL MATERIAL --------------------------------------------------------------------------------------
    float vertices[] = { //creamos un arreglo con los 3 vertices que formaran la figura
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };//el Z es cero porque estamos en 2D

    unsigned int VBO, VAO; //creamos nuestras herramientas de memoria
    glGenVertexArrays(1, &VAO); //crea el manual de instrucciones en la variable VAO
    glGenBuffers(1, &VBO);//crea la caja del almacenamiento en la variable VBO

    glBindVertexArray(VAO);//le decimos que todo lo que hagamos que lo anote en esa variable sin importar que

    glBindBuffer(GL_ARRAY_BUFFER, VBO);//le decimos a openGl que VBO es una caja para guaradr solo vertices
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//aqui es donde meteos los datos fisicamente
    //GL_ARRAY_BUFFER: tipo de caja
    //sizeof(vertices): el tamaño de nuestra lista de coordenadas
    //vertices: los datos que vamos a meter
    //GL_STATIC_DRAW: le decimos que en la jugada, que esos datos son estaticos y que sea rapido

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

        glUseProgram(shaderProgram);//le indica a la tarjeta grafica que hacer para calcular donde y de que color pintar
        //en este caso llamamos a la fusion que hicimos antes de la variable con el vector y el color
        glBindVertexArray(VAO);//Se activa el Vertex Array Object
        //carpeta donde estan las coordenadas exactas y la configuracion de los puntos
        glDrawArrays(GL_TRIANGLES, 0, 3);//orden de dibujar
        //GL_TRIANGLES: dibuje triangulos
        //0: que empiece desde el punto 0
        //3: y que son 3 puntos que debe procesar

        glfwSwapBuffers(window);//evita parpadeos
        glfwPollEvents();//escucha y procesa eventos del sistema
    }

    glfwTerminate();//una vez cierrada la ventana se finaliza todo los recursos listos para utilizar
    return 0;
}