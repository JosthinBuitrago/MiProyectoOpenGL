#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// 1. EL PINTOR DE POSICIONES (Vertex Shader)
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

// 2. EL PINTOR DE COLOR (Fragment Shader - pinta de naranja)
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n" // RGBA (Rojo, Verde, Azul, Transparencia)
    "}\n\0";

int main() {
    // --- CONFIGURACIÓN INICIAL (Lo que ya teníamos) ---
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Vital para tu Mac

    GLFWwindow* window = glfwCreateWindow(800, 600, "Mi Primer Triangulo OpenGL", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    // --- COMPILANDO LOS SHADERS (Los programitas de la tarjeta gráfica) ---
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // --- PREPARANDO EL TRIÁNGULO ---
    // Coordenadas de los 3 puntos (Izquierda, Derecha, Arriba)
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // Izquierda abajo
         0.5f, -0.5f, 0.0f, // Derecha abajo
         0.0f,  0.5f, 0.0f  // Arriba en el medio
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO); // Crea el manual de instrucciones
    glGenBuffers(1, &VBO);      // Crea la caja del correo

    glBindVertexArray(VAO);

    // Mete los datos en la caja
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Le dice a OpenGL cómo leer la caja (de 3 en 3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // --- BUCLE PRINCIPAL (El dibujo en vivo) ---
    while (!glfwWindowShouldClose(window)) {
        // Fondo verde oscuro
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // DIBUJAR EL TRIÁNGULO
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3); // 3 significa "dibuja 3 puntos"

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}