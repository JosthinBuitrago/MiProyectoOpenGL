#version 330 core
out vec4 FragColor;
//out: sale de la tarjeta grafica a la pantalla para pintar un pixel
//vec4: vector donde se almacena 4 colores (RGBA: Rojo, Verde, Azul, Transparencia)
//FragColor: Nombre de la variable donde se almacena el color
void main()
{
   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f); //la orden
   //las intensidades van de 0.0 a 1.0. Esta combinacion da naranja
}