#version 330 core

uniform sampler2D textureSampler;

in vec3 P;
in vec3 N;
in vec2 T;
in vec2 lights;

out vec4 color;

void directional();

void main()
{
	directional();
}

void directional()
{
	vec3 norm = normalize(N);

	vec3 L1 = normalize(vec3(1.0f, 1.0f, 1.0f));
	vec3 lightColor1 = vec3(0.0f, 0.0f, 1.0f);

	vec3 L2 = normalize(vec3(-1.0f, 1.0f, 1.0f));
	vec3 lightColor2 = vec3(1.0f, 0.0f, 0.0f);

	// diffuse
	float k_d = 0.6f;
	float inProduct1 = clamp( dot( L1, norm), 0, 1 );
	vec3 diff1 = k_d * inProduct1 * lightColor1 * texture( textureSampler, T ).rgb;

	float inProduct2 = clamp( dot( L2, norm), 0, 1 );
	vec3 diff2 = k_d * inProduct2 * lightColor2 * texture( textureSampler, T ).rgb;

	// ambient
	vec3 ambi1 = vec3(0.5f, 0.5f, 0.5f) * texture( textureSampler, T).rgb;

	// specular

	vec3 total = vec3(0.0f, 0.0f, 0.0f);
	if(lights[0] - 0.5f > 0.0f)
		total += diff1;
	if(lights[1] - 0.5f > 0.0f)
		total += diff2;

	vec3 result = (total + ambi1) * vec3(1.0f, 1.0f, 1.0f);
	color = vec4(result, 1.0f);
}