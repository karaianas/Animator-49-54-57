#pragma once
#include "Core.h"
#include "Vertex.h"

using namespace std;

class Face {
public:
	Face();
	~Face();

	void setIndex(int index);
	void setVertices(Vertex* v0, Vertex* v1, Vertex* v2);
	//void calcNormal();

	int id;
	vector<Vertex*> vertices;
	//glm::vec3 n;
};