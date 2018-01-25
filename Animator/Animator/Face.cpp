#include "Face.h"

Face::Face()
{
}

Face::~Face()
{
}

void Face::setIndex(int index)
{
	id = index;
}

void Face::setVertices(Vertex * v0, Vertex * v1, Vertex * v2)
{
	vertices.push_back(v0);
	vertices.push_back(v1);
	vertices.push_back(v2);
}
