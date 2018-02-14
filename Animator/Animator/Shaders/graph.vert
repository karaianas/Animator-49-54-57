#version 330 core

layout (location = 0) in vec2 position;

uniform mat4 MVP;
uniform vec3 channel;

out vec3 c;

void main()
{
    gl_Position = MVP * vec4(position.x, position.y, 0.0, 1.0);
	c = channel;
}
