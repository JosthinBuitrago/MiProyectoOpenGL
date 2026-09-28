#version 330 core
out vec4 FragColor;

in vec3 colorParaPintor; // Recibe el color que le mandó el topógrafo

void main()
{
    // Usa el color recibido (agrega 1.0f al final para que no sea transparente)
    FragColor = vec4(colorParaPintor, 1.0f); 
}