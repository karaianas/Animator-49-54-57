#include "Vertex.h"

Vertex::Vertex()
{
	vDelta = glm::vec3();
	nDelta = glm::vec3();
	jointId = glm::vec4(0);
	jointW = glm::vec4(0.0f);
}

Vertex::~Vertex()
{
}

void Vertex::setIndex(int index)
{
	//id = index;
}

void Vertex::setPosition(glm::vec3 position)
{
	p = position;
}

void Vertex::setNormal(glm::vec3 normal)
{
	n = normal;
}
