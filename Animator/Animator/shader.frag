#version 330 core

in vec3 sampleExtraOutput;
in vec3 P;
in vec3 N;

out vec4 color;

void directional();

void main()
{
	directional();
}

void directional()
{
	vec3 L = normalize(vec3(1.0f, 1.0f, 1.0f));
	vec3 norm = normalize(N);
	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

	// diffuse
	float k_d = 0.6f;
	float inProduct = max(dot(L, norm), 0);
	vec3 diff = k_d * inProduct * lightColor;

	// ambient
	vec3 ambi = vec3(0.5f, 0.5f, 0.5f);

	// specular

	vec3 result = (diff + ambi) * sampleExtraOutput;
	color = vec4(result, 1.0f);
}