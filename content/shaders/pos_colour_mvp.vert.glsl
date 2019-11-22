attribute vec3 position;
attribute vec4 colour;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

varying vec4 outcolour;

void main()
{
   gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);
   gl_PointSize = 4.0f;
   outcolour = colour;
}
