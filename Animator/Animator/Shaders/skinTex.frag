#version 330 core

uniform sampler2D textureSampler;

in vec3 P;
in vec3 N;
in vec2 T;

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
	float inProduct = clamp( dot( L, norm), 0, 1 );//max(dot(L, norm), 0);
	vec3 diff = k_d * inProduct * lightColor * texture( textureSampler, T ).rgb;

	// ambient
	vec3 ambi = vec3(0.3f, 0.3f, 0.3f) * texture( textureSampler, T).rgb;

	// specular

	vec3 result = diff + ambi;
	color = vec4(result, 1.0f);
}