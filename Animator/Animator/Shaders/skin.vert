#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 P;
out vec3 N;

void main()
{
    gl_Position = projection * view * model * vec4(position.x, position.y, position.z, 1.0);
	P = vec3(model * vec4(position, 1.0f));
	N = mat3(transpose(inverse(model))) * normal;
}
