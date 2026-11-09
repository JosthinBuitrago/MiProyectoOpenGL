#include <glad/glad.h> // Cargador de funciones de la GPU (SIEMPRE VA PRIMERO EN MAC).
#include <GLFW/glfw3.h> // Librería para crear la ventana y leer el teclado.
#include <iostream> // Para imprimir errores en la consola.
#include <cmath> // Para floor (redondear hacia abajo) y sqrt (raíz cuadrada).
#include <cstdio> // snprintf: para armar los nombres "Linea 1", "Circulo 2"...

#include "Shader.h" // El programa que pinta los colores en la tarjeta gráfica.
#include "Grilla.h" // Mi lienzo de cuadritos.
#include "Bresenham.h" // Mi matemática que traza líneas y círculos.
#include "Figura.h" // El paquete que guarda cada dibujo (tipo, puntos y color).

// --- NUEVO: Dear ImGui (la librería que dibuja los botones y paneles) ---
#include "imgui.h"              // El núcleo: botones, textos, ventanas.
#include "imgui_impl_glfw.h"    // Conecta ImGui con GLFW (para leer el mouse).
#include "imgui_impl_opengl3.h" // Conecta ImGui con OpenGL (para dibujar).

// Convierte un color RGB (valores de 0 a 1) a CMYK (valores de 0 a 1).
// Los "&" hacen que la función pueda devolver cuatro resultados a la vez.
// Ojo: es la fórmula matemática simple; una impresora real usa perfiles de color más complejos.
void rgbACmyk(float r, float g, float b, float& c, float& m, float& y, float& k) {
    float mayor = r;                  // Busco cuál de los tres canales es el mayor.
    if (g > mayor) mayor = g;
    if (b > mayor) mayor = b;
    k = 1.0f - mayor;                 // El negro es lo que le "falta" de luz al color más fuerte.
    if (k >= 1.0f) {                  // Negro puro: evito dividir por cero.
        c = 0.0f; m = 0.0f; y = 0.0f;
    } else {
        c = (1.0f - r - k) / (1.0f - k);
        m = (1.0f - g - k) / (1.0f - k);
        y = (1.0f - b - k) / (1.0f - k);
    }
}

int main() {
    const int ANCHO_PANEL = 200; // El panel mide 200 puntos de ancho y va a la izquierda.
    // Configuro el tamaño de mi mundo virtual (cuántos "píxeles gigantes" quiero).
    int columnas = 40;
    int filas = 30;

    // --- CONFIGURACIÓN DE LA VENTANA (GLFW) ---
    if (!glfwInit()) return -1; // Arranco la librería de la ventana. Si falla, cierro el programa.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);   //VERSION DEL OPEN GL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Configuración obligatoria y vital para macOS.
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // La ventana no se puede agrandar (así el panel y el lienzo no se deforman).
    // Creo la ventana de 1000x600 píxeles reales y le pongo un título.
    GLFWwindow* window = glfwCreateWindow(1000, 600, "Bresenham - Proyecto Final Tercer Corte", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; } // Si hubo un error creando la ventana, me salgo.
    glfwMakeContextCurrent(window); // Le digo a OpenGL: "Dibuja en esta ventana que acabo de crear".
    // Inicializo GLAD (conecta mi código con los drivers de video de la Mac).
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;
    // Cargo mis Shaders (los pequeños programas que se ejecutan directo en el procesador gráfico).

    // --- NUEVO: ENCIENDO IMGUI (siempre DESPUÉS de crear la ventana y cargar GLAD) ---
    IMGUI_CHECKVERSION();                       // Revisa que los archivos de ImGui sean de la misma versión.
    ImGui::CreateContext();                     // Crea la "memoria" interna de ImGui (botones, estados, etc).
    ImGui::StyleColorsDark();                   // Tema oscuro para el panel.
    ImGui_ImplGlfw_InitForOpenGL(window, true); // Le presento mi ventana a ImGui para que lea el mouse.
    ImGui_ImplOpenGL3_Init("#version 330");     // Versión de shaders que usa ImGui (la misma de mi OpenGL 3.3).
    {
    Shader shaderProgram("../shaders/topografo.vert", "../shaders/pintor.frag");
    // Creo mi lienzo gigante (instancio mi clase Grilla).
    Grilla grilla(columnas, filas);

    // Mi lista de dibujos: cada vez que dibujo algo, lo guardo aquí.
    std::vector<Figura> figuras;
    // Estas dos variables recuerdan si la tecla ya estaba presionada en el cuadro anterior.
    // Las necesito porque el bucle gira ~60 veces por segundo: sin esto, una sola pulsación
    // agregaría decenas de figuras repetidas.

    TipoFigura herramienta = LINEA;           // La herramienta elegida en el panel (todavía no la usamos con el mouse).
    float colorActual[3] = {0.0f, 1.0f, 1.0f}; // El color elegido: rojo, verde, azul (cian al inicio).

    bool arrastrando = false;            // ¿Estoy en medio de un clic y arrastre?
    bool botonEstabaPresionado = false;  // ¿El botón del mouse estaba presionado en el cuadro anterior?
    int inicioX = 0, inicioY = 0;        // La celda donde empecé a arrastrar.

    int seleccionada = -1;            // Índice de la figura elegida en la lista (-1 = ninguna).
    float pasoGrados = 15.0f;         // Cuántos grados gira cada clic en "Girar".
    bool girarSobreLienzo = false;    // ¿Giro alrededor del centro del lienzo (true) o de la propia figura (false)?

    float factorEscala = 1.25f; // Cuánto crece o se encoge la figura en cada clic.

    std::vector<float> trazoX, trazoY; // NUEVO: las celdas por donde va pasando el mouse mientras dibujo con el lápiz.

    // --- BUCLE PRINCIPAL (GAMELOOP) ---
    // Este ciclo da vueltas infinitas a 60 cuadros por segundo hasta que el usuario cierre la ventana.
    while (!glfwWindowShouldClose(window)) {
        // --- NUEVO: PANEL DE IMGUI (se reconstruye entero en CADA cuadro) ---
        ImGui_ImplOpenGL3_NewFrame(); // Empieza un cuadro nuevo (parte OpenGL).
        ImGui_ImplGlfw_NewFrame();    // Empieza un cuadro nuevo (parte GLFW: lee el mouse).
        ImGui::NewFrame();            // Empieza un cuadro nuevo (parte ImGui).

        ImGui::SetNextWindowPos(ImVec2(0, 0));                          // Panel pegado arriba a la izquierda.
        ImGui::SetNextWindowSize(ImVec2((float)ANCHO_PANEL, 600.0f));   // Ocupa toda la columna izquierda.
        // Begin abre el panel. Las flags lo dejan fijo: no se mueve, no se redimensiona, no se pliega.
        ImGui::Begin("Herramientas", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        ImGui::Text("Mi Paint");
        ImGui::Separator();

        ImGui::Text("Herramienta");
        // RadioButton: botón redondo; devuelve true cuando lo clickean. Solo uno queda marcado.
        if (ImGui::RadioButton("Linea", herramienta == LINEA)) herramienta = LINEA;
        if (ImGui::RadioButton("Circulo", herramienta == CIRCULO)) herramienta = CIRCULO;
        if (ImGui::RadioButton("Rectangulo", herramienta == RECTANGULO)) herramienta = RECTANGULO;
        if (ImGui::RadioButton("Lapiz", herramienta == LAPIZ)) herramienta = LAPIZ;
        ImGui::Separator();
        ImGui::Text("Color");
        ImGui::SetNextItemWidth(-1); // La barra de color ocupa todo el ancho del panel.
        ImGui::ColorEdit3("##color", colorActual); // Selector RGB. ImGui cambia colorActual directamente.

        float c, m, y, k; // Aquí recibo el resultado de la conversión.
        rgbACmyk(colorActual[0], colorActual[1], colorActual[2], c, m, y, k);
        ImGui::Text("C: %.0f%%  M: %.0f%%", c * 100.0f, m * 100.0f); // %% imprime el símbolo %.
        ImGui::Text("Y: %.0f%%  K: %.0f%%", y * 100.0f, k * 100.0f);

        ImGui::Separator();

        // Button devuelve true SOLO en el cuadro en que lo clickean. ImVec2(-1, 0) = ocupa todo el ancho.
        if (ImGui::Button("Borrar todo", ImVec2(-1, 0))) {
            figuras.clear(); seleccionada = -1; // Mismo efecto que la tecla B.
        }

        ImGui::Separator();
        ImGui::Text("Figuras (elige una)");
        if (seleccionada >= (int)figuras.size()) seleccionada = -1; // Por seguridad: si ya no existe, no hay selección.
        // BeginChild: una cajita con su propio scroll para la lista.
        ImGui::BeginChild("lista", ImVec2(-1, 90), true);
        for (int i = 0; i < (int)figuras.size(); ++i) {
            char etiqueta[32];
            const char* nombre = "Linea";
            if (figuras[i].tipo == CIRCULO) nombre = "Circulo";
            else if (figuras[i].tipo == RECTANGULO) nombre = "Rectangulo";
            else if (figuras[i].tipo == LAPIZ) nombre = "Lapiz";
            snprintf(etiqueta, sizeof(etiqueta), "%s %d", nombre, i + 1);
            // Selectable: un renglón clickeable. Devuelve true cuando lo clickean.
            if (ImGui::Selectable(etiqueta, seleccionada == i)) seleccionada = i;
        }
        ImGui::EndChild();

        if (seleccionada >= 0) { // Solo muestro los controles si hay una figura elegida.
            Figura& sel = figuras[seleccionada]; // "&" = trabajo sobre la figura REAL de la lista, no sobre una copia.

            ImGui::Text("Mover");
            if (ImGui::Button("Izq", ImVec2(86, 0))) sel.mover(-1, 0);
            ImGui::SameLine();
            if (ImGui::Button("Der", ImVec2(86, 0))) sel.mover(1, 0);
            if (ImGui::Button("Abajo", ImVec2(86, 0))) sel.mover(0, -1);
            ImGui::SameLine();
            if (ImGui::Button("Arriba", ImVec2(86, 0))) sel.mover(0, 1);

            ImGui::Text("Rotar");
            ImGui::SetNextItemWidth(-1);
            ImGui::SliderFloat("##paso", &pasoGrados, 1.0f, 90.0f, "Paso: %.0f grados");
            ImGui::Checkbox("Girar sobre el lienzo", &girarSobreLienzo);
            // Elijo el pivote (el punto fijo alrededor del cual se gira).
            float pivoteX, pivoteY;
            if (girarSobreLienzo) {            // El centro del lienzo (las celdas van de 0 a columnas-1).
                pivoteX = (columnas - 1) / 2.0f;
                pivoteY = (filas - 1) / 2.0f;
            } else if (sel.tipo == LINEA) {    // El punto medio de la propia línea.
                pivoteX = (sel.x0 + sel.x1) / 2.0f;
                pivoteY = (sel.y0 + sel.y1) / 2.0f;
                } else if (sel.tipo == RECTANGULO || sel.tipo == LAPIZ) { // El centro: el promedio de todos sus puntos.
                pivoteX = 0.0f;
                pivoteY = 0.0f;
                for (int i = 0; i < (int)sel.puntosX.size(); ++i) {
                    pivoteX += sel.puntosX[i];
                    pivoteY += sel.puntosY[i];
                }
                pivoteX /= (float)sel.puntosX.size();
                pivoteY /= (float)sel.puntosX.size();
            } else {                           // El centro del círculo (girar sobre sí mismo no se nota).
                pivoteX = sel.x0;
                pivoteY = sel.y0;
            }
            if (ImGui::Button("Girar -", ImVec2(86, 0))) sel.rotar(-pasoGrados, pivoteX, pivoteY);
            ImGui::SameLine();
            if (ImGui::Button("Girar +", ImVec2(86, 0))) sel.rotar(pasoGrados, pivoteX, pivoteY);

            ImGui::Text("Escalar");
            ImGui::SetNextItemWidth(-1);
            ImGui::SliderFloat("##escala", &factorEscala, 1.1f, 2.0f, "Factor: x%.2f");
            // Achicar usa 1 / factor, así "Agrandar" y luego "Achicar" vuelven al tamaño original.
            if (ImGui::Button("Achicar", ImVec2(86, 0))) sel.escalar(1.0f / factorEscala, pivoteX, pivoteY);
            ImGui::SameLine();
            if (ImGui::Button("Agrandar", ImVec2(86, 0))) sel.escalar(factorEscala, pivoteX, pivoteY);

            ImGui::Text("Color de esta figura");
            float colorSel[3] = {sel.r, sel.g, sel.b}; // Copio el color actual de la figura...
            ImGui::SetNextItemWidth(-1);
            if (ImGui::ColorEdit3("##colorSel", colorSel)) { // ...ImGui lo edita; si cambió, lo devuelvo a la figura.
                sel.r = colorSel[0]; sel.g = colorSel[1]; sel.b = colorSel[2];
            }

            float cs, ms, ys, ks;
            rgbACmyk(sel.r, sel.g, sel.b, cs, ms, ys, ks);
            ImGui::Text("C: %.0f%%  M: %.0f%%", cs * 100.0f, ms * 100.0f);
            ImGui::Text("Y: %.0f%%  K: %.0f%%", ys * 100.0f, ks * 100.0f);

            if (ImGui::Button("Borrar esta figura", ImVec2(-1, 0))) {
                figuras.erase(figuras.begin() + seleccionada); // La saco de la lista.
                seleccionada = -1;
            }
        }

        ImGui::End(); // Cierro el panel.

        // --- NUEVO: MOUSE ---
        double mx, my;
        glfwGetCursorPos(window, &mx, &my); // Posición del mouse en puntos de ventana (origen: arriba a la izquierda).

        // Convierto la posición del mouse a una CELDA de mi grilla.
        float anchoCeldaPx = 800.0f / columnas;  // El lienzo mide 800 puntos de ancho -> cada celda mide 20.
        float altoCeldaPx  = 600.0f / filas;     // El lienzo mide 600 puntos de alto  -> cada celda mide 20.
        float lienzoX = (float)mx - ANCHO_PANEL; // Quito el panel: el lienzo empieza en x = 200.
        float lienzoY = 600.0f - (float)my;      // Volteo la Y: el mouse cuenta desde arriba, mi grilla desde abajo.
        int celdaX = (int)std::floor(lienzoX / anchoCeldaPx); // floor redondea hacia abajo (bien con negativos).
        int celdaY = (int)std::floor(lienzoY / altoCeldaPx);
        // Si el mouse se sale del lienzo mientras arrastro, lo "pego" al borde para no salirme de la grilla.
        if (celdaX < 0) celdaX = 0;
        if (celdaX > columnas - 1) celdaX = columnas - 1;
        if (celdaY < 0) celdaY = 0;
        if (celdaY > filas - 1) celdaY = filas - 1;

        bool botonPresionado = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);

        // 1) ACABO DE PRESIONAR el botón (y no fue sobre el panel): empieza el arrastre.
        if (botonPresionado && !botonEstabaPresionado && mx >= ANCHO_PANEL && !ImGui::GetIO().WantCaptureMouse) {
            arrastrando = true;
            inicioX = celdaX;
            inicioY = celdaY;
            trazoX.clear(); trazoY.clear();   // Empiezo un recorrido nuevo, vacío...
            trazoX.push_back((float)celdaX);  // ...con la celda donde presioné como primer punto.
            trazoY.push_back((float)celdaY);
        }

        // NUEVO: con el lápiz, voy guardando cada celda nueva por la que pasa el mouse.
        if (arrastrando && herramienta == LAPIZ) {
            // Solo guardo el punto si cambié de celda (si no, la lista se llenaría de repetidos).
            if ((float)celdaX != trazoX.back() || (float)celdaY != trazoY.back()) {
                trazoX.push_back((float)celdaX);
                trazoY.push_back((float)celdaY);
            }
        }

        // 2) Mientras arrastro, armo la figura "nueva" con el inicio y la celda actual del mouse.
        Figura nueva{}; // {} = todos sus datos empiezan en cero.
        if (arrastrando) {
            nueva.tipo = herramienta;                // Línea o círculo, según el panel.
            nueva.x0 = inicioX; nueva.y0 = inicioY;  // Línea: punto inicial. Círculo: centro.
            nueva.x1 = celdaX;  nueva.y1 = celdaY;   // Línea: punto final.
            int dx = celdaX - inicioX;
            int dy = celdaY - inicioY;
            nueva.radio = (int)std::sqrt(dx * dx + dy * dy); // Círculo: el radio es la distancia (Pitágoras) del centro al mouse.
                if (herramienta == RECTANGULO) {
                // Las 4 esquinas, en orden alrededor del borde: inicio -> arriba-derecha -> final -> abajo-izquierda.
                nueva.puntosX = { (float)inicioX, (float)celdaX, (float)celdaX, (float)inicioX };
                nueva.puntosY = { (float)inicioY, (float)inicioY, (float)celdaY, (float)celdaY };
            }
            if (herramienta == LAPIZ) {
                nueva.puntosX = trazoX; // La figura se queda con una copia del recorrido.
                nueva.puntosY = trazoY;
            }
            nueva.r = colorActual[0]; nueva.g = colorActual[1]; nueva.b = colorActual[2];
        }

        // 3) ACABO DE SOLTAR el botón: la figura queda guardada en la lista.
        if (!botonPresionado && botonEstabaPresionado && arrastrando) {
            figuras.push_back(nueva);
            arrastrando = false;
        }
        botonEstabaPresionado = botonPresionado; // Recuerdo el estado para el próximo cuadro.

        // REDIBUJO: limpio el lienzo y vuelvo a pintar TODAS las figuras de la lista.
        grilla.limpiar();
        for (auto& figura : figuras) {
            figura.dibujar(grilla); // Cada figura se dibuja a sí misma con Bresenham.
        }

        // Marco con puntos blancos los extremos (o el centro) de la figura seleccionada.
        if (seleccionada >= 0 && seleccionada < (int)figuras.size()) {
            Figura& s = figuras[seleccionada];
            if (s.tipo == RECTANGULO) { // El rectángulo marca sus 4 esquinas.
                for (int i = 0; i < (int)s.puntosX.size(); ++i) {
                    grilla.paintPixel((int)std::round(s.puntosX[i]), (int)std::round(s.puntosY[i]), 1.0f, 1.0f, 1.0f);
                }
            } else if (s.tipo == LAPIZ) { // El lápiz marca solo donde empieza y donde termina.
                int ultimo = (int)s.puntosX.size() - 1;
                grilla.paintPixel((int)std::round(s.puntosX[0]), (int)std::round(s.puntosY[0]), 1.0f, 1.0f, 1.0f);
                grilla.paintPixel((int)std::round(s.puntosX[ultimo]), (int)std::round(s.puntosY[ultimo]), 1.0f, 1.0f, 1.0f);
            } else {
                grilla.paintPixel((int)std::round(s.x0), (int)std::round(s.y0), 1.0f, 1.0f, 1.0f);
                if (s.tipo == LINEA) grilla.paintPixel((int)std::round(s.x1), (int)std::round(s.y1), 1.0f, 1.0f, 1.0f);
            }
        }

        if (arrastrando) nueva.dibujar(grilla); // Vista previa: se dibuja pero todavía NO está en la lista.
        grilla.actualizarVRAM(); // Mando el resultado a la tarjeta gráfica.
        
        // LIMPIEZA VISUAL: Limpio el color de fondo de la ventana real antes de pintar encima.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // --- NUEVO: PUERTO DE VISTA (viewport) ---
        // Le digo a OpenGL en qué rectángulo de la ventana dibujar mi grilla: solo la parte derecha.
        // Mac Retina: la pantalla tiene el doble de píxeles reales que de "puntos" de ventana,
        // y glViewport trabaja en píxeles REALES. Por eso calculo la escala comparando ambos tamaños.
        int anchoVentana, altoVentana;
        glfwGetWindowSize(window, &anchoVentana, &altoVentana);   // Tamaño en puntos (1000x600).
        int anchoReal, altoReal;
        glfwGetFramebufferSize(window, &anchoReal, &altoReal);    // Tamaño en píxeles reales (2000x1200 en Retina).
        float escala = (float)anchoReal / (float)anchoVentana;    // 2.0 en Retina, 1.0 en pantallas normales.
        int panelEnPixeles = (int)(ANCHO_PANEL * escala);         // Cuánto mide el panel en píxeles reales.
        // glViewport(x, y, ancho, alto): el lienzo empieza DESPUÉS del panel y ocupa el resto.
        glViewport(panelEnPixeles, 0, anchoReal - panelEnPixeles, altoReal);

        // LE DIGO A LA GPU: "Usa mi programa de colores (Shader) y dibuja mi grilla completa".
        shaderProgram.usar();
        grilla.dibujar();
        // --- NUEVO: ImGui dibuja su panel ENCIMA de la grilla ---
        ImGui::Render();                                        // ImGui prepara lo que va a dibujar.
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // ImGui lo manda a la GPU.
        // OpenGL usa una técnica de "doble buffer". Mientras yo veo una pantalla, él pinta en una pantalla oculta.
        // SwapBuffers intercambia esas dos pantallas para que yo vea el dibujo nuevo sin parpadeos.
        glfwSwapBuffers(window);
        // Reviso si el mouse se movió, si se presionó otra tecla o si le dieron click a la 'X' de cerrar ventana.
        glfwPollEvents();
    }
    // Cuando el usuario cierra la ventana, apago todo de forma segura para no trabar la Mac.
}
    // --- NUEVO: apago ImGui en orden inverso al que lo encendí ---
    ImGui_ImplOpenGL3_Shutdown(); // Apago la parte OpenGL de ImGui.
    ImGui_ImplGlfw_Shutdown();    // Apago la parte GLFW de ImGui.
    ImGui::DestroyContext();      // Libero la memoria interna de ImGui.
    glfwDestroyWindow(window);    // Cierro la ventana.

    glfwTerminate();
    // Termino el programa con éxito.
    return 0;
}