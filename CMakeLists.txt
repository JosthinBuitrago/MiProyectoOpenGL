cmake_minimum_required(VERSION 3.10)
project(OpenGL_Mac)

set(CMAKE_CXX_STANDARD 17)

# Buscar GLFW instalado por Homebrew
find_package(glfw3 REQUIRED)

# Incluir las cabeceras locales de GLAD y de Homebrew
include_directories(include /opt/homebrew/include)

# Definir el ejecutable agregando main.cpp y glad.c
add_executable(App src/main.cpp src/glad.c)

# Enlazar las librerías nativas de macOS y GLFW
target_link_libraries(App PRIVATE glfw "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")