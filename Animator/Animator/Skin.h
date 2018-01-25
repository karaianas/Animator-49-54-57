#pragma once
#include "Core.h"
#include "Vertex.h"
#include "Face.h"

using namespace std;

class Skin {
public:
	Skin();
	~Skin();

	void print();

	//vector<glm::vec3> positions;// vector<Vertex> positions;
	//vector<glm::vec3> normals;// should be part of Vertex
	vector<Vertex*> vertices;
	vector<vector<int>*> weightInds;
	vector<vector<float>*> weights;
	//vector<glm::vec3> faces;// vector<Face> faces;
	vector<Face*> faces;
	vector<glm::mat4> Bmatrices;

};