#pragma once
#include "Core.h"
#include "Vertex.h"
#include "Face.h"
#include "Joint.h"

using namespace std;

class Skin {
public:
	Skin();
	~Skin();

	//void draw();
	//void update();

	void print();
	void setJointsPtr(vector<Joint*>* ptr);
	glm::vec3 getDeformVertex(int id);

	vector<Joint*>* jointsPtr;
	vector<Vertex*> vertices;
	vector<Face*> faces;
	vector<glm::mat4> Bmatrices;// This should be inside Joint?
};