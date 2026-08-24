#include <glad/glad.h> //directorio de funciones graficas
#include <GLFW/glfw3.h> //encargada de comunicacion con el sistema operativo
#include <iostream> //herramienta nativa de c++
#include <fstream>
#include <sstream>
#include <string>

// --- HERRAMIENTA: LECTOR DE ARCHIVOS ---------------------------------------------------------
std::string leerShader(const char* ruta) { //es un mensajero que me trae el texto y me lo trae aca
    std::ifstream archivo(ruta);//crea un tubo que conceta el programa con l archivo
    if (!archivo.is_open()) {//salvavidas por si el archivo no existe o la ruta esta mal
        std::cout << "ERROR::ARCHIVO::NO_SE_PUDO_ABRIR: " << ruta << std::endl;
        return "";
    }
    std::stringstream flujoDatos;//balde temporal en la memoria
    flujoDatos << archivo.rdbuf();//el .rdbuf lee todo el texto y lo vacia en el valde
    archivo.close();//cierra el archivo
    return flujoDatos.str();//toma lo del valde y lo convierte en un archivo moderno de c++, y lo entrega
}
//---------------------------------------------------------------------------------------------------

int main() {

    //EXTRACCIÓN Y TRADUCCIÓN DE SHADERS------------------------------------------------------------------
    std::string stringTopografo = leerShader("../shaders/topografo.vert");//llamamos la funcion y lo colocamos en esa variable
    const char* vertexShaderSource = stringTopografo.c_str();//aqui con el c_str transformamos ese texto en texto en texto legible para openGL
    std::string stringPintor = leerShader("../shaders/pintor.frag");
    const char* fragmentShaderSource = stringPintor.c_str();
    //------------------------------------------------------------------------------------------------------

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

    //-------SISTEMA DE ALARMAS (Logs de Shaders) para el vertex shader -----------------------------------------------------------------------
    int success;//variable entera, va a ser 1 si todo salio bien, 0 si el codigo de shader tenia errores, por el momento esta vacia
    char infoLog[512];//hoja en blanco donde la tarjeta va escribir el error con detalle si existe (maximo de 512 caracteres)
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);//le preguntamos al inspector
    //vertexShader: el ID de nuestro empleado
    //GL_COMPILE_STATUS: le preguntamos el estado de compilacion
    //&success: aqui le pasamos la variable, va a poner 1 o 0 dependiendo
    if (!success) {//si es 0
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);//extraemos el error que nos manda la tarjeta
        //vertexShader: ID del empleado que nos fallo
        //512: maximo de caracteres
        //NULL: El maximo de caracteres que nos da la grafica, pero no nos interesa
        //infoLog: donde queremos que imprima el error
        std::cout << "ERROR::SHADER::VERTEX::COMPILACIÓN_FALLIDA\n" << infoLog << std::endl;
        //sea hace un cout donde se dice el error y la variable con los datos especificos
    }
    //---------------------------------------------------------------------------------------------------------------------

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);//lo mismo pero ahora con la asignacion de la variable que maneja el color
    glCompileShader(fragmentShader);

    //-------SISTEMA DE ALARMAS (Logs de Shaders) para el fragment shader -----------------------------------------------------------------------
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);//misma explicacion pero ahora para este apartado
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILACIÓN_FALLIDA\n" << infoLog << std::endl;
    }
    //---------------------------------------------------------------------------------------------------------------------

    unsigned int shaderProgram = glCreateProgram(); //Creacion de variable donde va un programa completo de shaders
    glAttachShader(shaderProgram, vertexShader);//Conectamos la variable del vector a este programa
    glAttachShader(shaderProgram, fragmentShader);//lo mismo pero con la variable del color
    glLinkProgram(shaderProgram);//aqui conceta los dos shaders (internamente recibe el vector y devuelve la grfica ya con el color)

    //-------SISTEMA DE ALARMAS (Logs de Shaders) para la fusion -----------------------------------------------------------------------
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);//la misma explicacion pero para la fusion
        std::cout << "ERROR::SHADER::PROGRAMA::ENLACE_FALLIDO\n" << infoLog << std::endl;
    }
    //---------------------------------------------------------------------------------------------------------------------

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