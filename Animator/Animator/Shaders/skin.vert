#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 3) in vec4 index;
layout (location = 4) in vec4 weight;

uniform mat4 MVP;
uniform mat4 itM;
uniform mat4 model;
uniform vec3 color;
uniform vec2 light;
uniform mat4 Bmatrices[50];

//out vec3 P;
out vec3 N;
out vec2 lights;

void main()
{	
	// Skinning
	vec4 newP = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	vec4 newN = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	for(int i = 0;  i < 4; i++)
	{
		int id = int(index[i]);
		newP += weight[i] * Bmatrices[id] * vec4(position, 1.0f);
		newN += weight[i] * Bmatrices[id] * vec4(normal, 0.0f);
	}

	gl_Position =  MVP * vec4(newP.x, newP.y, newP.z, 1.0);
	//P = vec3(model * vec4(position, 1.0f));
	N = mat3(itM) * normalize(vec3(newN));
	lights = light;
}
