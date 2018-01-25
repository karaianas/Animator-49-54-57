#include "Vertex.h"

Vertex::Vertex()
{
}

Vertex::~Vertex()
{
}

void Vertex::setIndex(int index)
{
	id = index;
}

void Vertex::setPosition(glm::vec3 position)
{
	p = position;
}

void Vertex::setNormal(glm::vec3 normal)
{
	n = normal;
}
