#pragma once
#include "Core.h"

using namespace std;

class Vertex {
public:
	glm::vec3 p;
	glm::vec3 n;

	glm::vec3 vDelta;
	glm::vec3 nDelta;

	glm::vec4 jointId;
	glm::vec4 jointW;
	//vector<int> jointId;
	//vector<float> jointW;

	//int id;
	int flag;

	Vertex();
	~Vertex();

	void setIndex(int index);
	void setPosition(glm::vec3 position);
	void setNormal(glm::vec3 normal);
};