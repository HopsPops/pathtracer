#version 330 core

layout (location = 0) in vec3 positionIn;

//uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    gl_Position = projection * view * vec4(positionIn, 1.0);
}