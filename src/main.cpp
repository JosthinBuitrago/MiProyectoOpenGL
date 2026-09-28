#include <glad/glad.h> // Cargador de funciones de la GPU (SIEMPRE VA PRIMERO EN MAC).
#include <GLFW/glfw3.h> // Librería para crear la ventana y leer el teclado.
#include <iostream> // Para imprimir errores en la consola.

#include "Shader.h" // El programa que pinta los colores en la tarjeta gráfica.
#include "Grilla.h" // Mi lienzo de cuadritos.
#include "Bresenham.h" // Mi matemática que traza líneas y círculos.

int main() {
    // Configuro el tamaño de mi mundo virtual (cuántos "píxeles gigantes" quiero).
    int columnas = 40;
    int filas = 30;
    // Defino las coordenadas de prueba para mi línea
    int linea_x0 = 1, linea_y0 = 1;
    int linea_x1 = 20, linea_y1 = 30;
    // Defino las coordenadas de prueba para mi círculo
    int circulo_centroX = 20, circulo_centroY = 15;
    int circulo_radio = 10;

    // --- CONFIGURACIÓN DE LA VENTANA (GLFW) ---
    if (!glfwInit()) return -1; // Arranco la librería de la ventana. Si falla, cierro el programa.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);   //VERSION DEL OPEN GL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Configuración obligatoria y vital para macOS.
    // Creo la ventana de 800x600 píxeles reales y le pongo un título.
    GLFWwindow* window = glfwCreateWindow(800, 600, "Bresenham - Proyecto Final Segundo Corte", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; } // Si hubo un error creando la ventana, me salgo.
    glfwMakeContextCurrent(window); // Le digo a OpenGL: "Dibuja en esta ventana que acabo de crear".
    // Inicializo GLAD (conecta mi código con los drivers de video de la Mac).
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;
    // Cargo mis Shaders (los pequeños programas que se ejecutan directo en el procesador gráfico).
    Shader shaderProgram("../shaders/topografo.vert", "../shaders/pintor.frag");
    // Creo mi lienzo gigante (instancio mi clase Grilla).
    Grilla grilla(columnas, filas);

    // --- BUCLE PRINCIPAL (GAMELOOP) ---
    // Este ciclo da vueltas infinitas a 60 cuadros por segundo hasta que el usuario cierre la ventana.
    while (!glfwWindowShouldClose(window)) {
        // INTERACCIÓN: Si el usuario presiona la tecla 'L', dibujo la Línea.
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
            grilla.limpiar(); // Primero borro lo que haya en pantalla.
            Bresenham::linea(linea_x0, linea_y0, linea_x1, linea_y1, grilla); // Calculo la línea matemática.
            grilla.actualizarVRAM(); // Mando los nuevos colores a la tarjeta gráfica.
        }
        // INTERACCIÓN: Si el usuario presiona la tecla 'C', dibujo el Círculo.
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
            grilla.limpiar(); // Borro la pantalla.
            Bresenham::circulo(circulo_centroX, circulo_centroY, circulo_radio, grilla); // Calculo el círculo.
            grilla.actualizarVRAM(); // Mando los nuevos colores a la tarjeta gráfica.
        }
        // LIMPIEZA VISUAL: Limpio el color de fondo de la ventana real antes de pintar encima.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // LE DIGO A LA GPU: "Usa mi programa de colores (Shader) y dibuja mi grilla completa".
        shaderProgram.usar();
        grilla.dibujar();
        // OpenGL usa una técnica de "doble buffer". Mientras yo veo una pantalla, él pinta en una pantalla oculta.
        // SwapBuffers intercambia esas dos pantallas para que yo vea el dibujo nuevo sin parpadeos.
        glfwSwapBuffers(window);
        // Reviso si el mouse se movió, si se presionó otra tecla o si le dieron click a la 'X' de cerrar ventana.
        glfwPollEvents();
    }
    // Cuando el usuario cierra la ventana, apago todo de forma segura para no trabar la Mac.
    glfwTerminate();
    // Termino el programa con éxito.
    return 0;
}