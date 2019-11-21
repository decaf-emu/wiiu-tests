attribute vec3 position;
attribute vec4 colour;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

varying vec4 outcolour;

void main()
{
   gl_Position = vec4(position, 1.0f) * modelMatrix * viewMatrix * projectionMatrix;
   outcolour = colour;
}
