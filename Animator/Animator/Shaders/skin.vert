#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 3) in vec4 index;
layout (location = 4) in vec4 weight;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 color;
uniform vec2 light;
uniform mat4 Bmatrices[50];

out vec3 P;
out vec3 N;
out vec2 lights;

void main()
{
	//gl_Position =  projection * view * model * vec4(position.x, position.y, position.z, 1.0);
	P = vec3(model * vec4(position, 1.0f));
	N = mat3(transpose(inverse(model))) * normal;
	lights = light;

	vec4 newPos = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	for(int i = 0;  i < 4; i++)
	{
		int id = int(index[i]);
		newPos += weight[i] * Bmatrices[id] * vec4(position, 1.0f);
	}

	gl_Position =  projection * view * model * vec4(newPos.x, newPos.y, newPos.z, 1.0);

}
