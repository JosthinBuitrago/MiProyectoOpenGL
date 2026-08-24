#version 330 core //version 3.3 moderno por el core
layout (location = 0) in vec3 aPos; //en la ubiacion 0 diremos que:
//in: la variable es una entrada (c++ a la GPU)
//vec3: vector de tres componentes (X, Y, Z)
//aPos: nombre que le dimos a la variable
void main() //funcion principal de la GPU (la hara una vez por cada vertice)
{
gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0); //variable de openGL
//se le da el vector mas el 1.0 para que openGl calcule perspectiva 3D
}