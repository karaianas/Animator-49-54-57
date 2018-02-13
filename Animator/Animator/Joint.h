#pragma once

#include "Core.h"
#include "Cube.h"

using namespace std;

class Joint {
public:
	Joint();
	~Joint();

	// Values
	vector<vector<float>*> container;
	vector<float> offset;
	vector<float> boxmin;
	vector<float> boxmax;
	vector<float> rotxlim;
	vector<float> rotylim;
	vector<float> rotzlim;
	vector<float> pose;

	glm::vec3 localA;

	// Matrices
	glm::mat4 localM;
	glm::mat4 worldM;
	glm::mat4 offLocalM;
	glm::mat4 scaleM;

	// Relations
	Joint* parent;
	vector<Joint*> children;

	// Identity
	vector<char>* name;
	int id;

	// Functions
	void draw(GLuint program, Cube* cube, glm::mat4 M);
	void update(float angle_step, int axis);
	void IKupdate(glm::vec3 angle);
	void setUpdate(glm::vec3 angle);

	void addChild(Joint * child);
	void checkValid();
	int getChildrenNum() { return children.size(); };
	glm::vec4 getPos() { return worldM[3]; };
	glm::vec3 getX() { return worldM[0]; };
	glm::vec3 getY() { return worldM[1]; };
	glm::vec3 getZ() { return worldM[2]; };
};