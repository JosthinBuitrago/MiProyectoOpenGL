#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    // Inicializar GLFW
    if (!glfwInit()) {
        std::cout << "Error al inicializar GLFW\n";
        return -1;
    }

    // Configurar OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Requerido en macOS

    // Crear ventana
    GLFWwindow* window = glfwCreateWindow(800, 600, "Hola OpenGL en Mac!", NULL, NULL);
    if (!window) {
        std::cout << "Error al crear la ventana\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Cargar funciones de OpenGL con GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Error al inicializar GLAD\n";
        return -1;
    }

    // Bucle principal
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Color de fondo (verde turquesa oscuro)
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}