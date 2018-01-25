#pragma once
#include "Core.h"

using namespace std;

class Vertex {
public:
	Vertex();
	~Vertex();

	void setIndex(int index);
	void setPosition(glm::vec3 position);
	void setNormal(glm::vec3 normal);

	int id;
	glm::vec3 p;
	glm::vec3 n;
};