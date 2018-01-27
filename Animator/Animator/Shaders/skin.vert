#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 color;

out vec3 P;
out vec3 N;
//out vec2 T;

void main()
{
    //gl_Position =  projection * view * model * vec4(normal.x, normal.y, normal.z, 1.0);
	gl_Position =  projection * view * model * vec4(position.x, position.y, position.z, 1.0);
	P = vec3(model * vec4(position, 1.0f));
	N = mat3(transpose(inverse(model))) * normal;
}
