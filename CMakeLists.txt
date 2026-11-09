cmake_minimum_required(VERSION 3.10)
project(OpenGL_Mac)

set(CMAKE_CXX_STANDARD 17)

# Buscar GLFW instalado por Homebrew
find_package(glfw3 REQUIRED)

# NUEVO: carpeta donde está Dear ImGui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/libs/imgui)

# Incluir las cabeceras locales de GLAD y de Homebrew
include_directories(include /opt/homebrew/include ${IMGUI_DIR} ${IMGUI_DIR}/backends)

# Definir el ejecutable agregando main.cpp y glad.c
add_executable(App
    src/main.cpp
    src/glad.c
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Enlazar las librerías nativas de macOS y GLFW
target_link_libraries(App PRIVATE glfw "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")