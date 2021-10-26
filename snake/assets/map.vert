#version 410

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec4 inColor;
//layout(location = 2) in vec2 inTranslation;

//uniform vec4 color;
uniform float scale;
uniform vec2 translation;

out vec4 fragColor;

void main() { 
  
  vec2 newPosition = inPosition * scale + translation;
  gl_Position = vec4(newPosition, 0, 1);
  fragColor = inColor;
}