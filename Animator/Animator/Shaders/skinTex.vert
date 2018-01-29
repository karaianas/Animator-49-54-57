#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec2 light;

out vec3 P;
out vec3 N;
out vec2 T;
out vec2 lights;

void main()
{
	gl_Position =  projection * view * model * vec4(position.x, position.y, position.z, 1.0);
	P = vec3(model * vec4(position, 1.0f));
	N = mat3(transpose(inverse(model))) * normal;
	T = texcoords;
	lights = light;
}
