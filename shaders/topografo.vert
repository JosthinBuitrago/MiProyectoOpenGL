#version 330 core
layout (location = 0) in vec3 aPos;   // Recibe la coordenada (X, Y, Z)
layout (location = 1) in vec3 aColor; // NUEVO: Recibe el color (R, G, B) del struct

out vec3 colorParaPintor; // Tubo que envía el color al Fragment Shader

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    colorParaPintor = aColor; // Pasa el color intacto al pintor
}