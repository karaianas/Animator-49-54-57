#pragma once
#include "Core.h"
#include "Joint.h"
#include "Cube.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "Skin.h"

using namespace std;

class Model {
public:
	Model();
	~Model();

	vector<Joint*> allJoints;
	Cube* cube;
	glm::mat4 worldM;
	Joint* root;
	Skin* skin;

	// Loading
	Joint* readSkel(const char *filepath);
	Skin* readSkin(const char *filepath);
	void readMorph(const char *filepath);

	// Joint initialization
	int setId(Joint* curr, int id);
	void printAllJoints();

	void calcLocalM(int id);
	void calcWorldM(Joint * curr, glm::mat4 M);
	void draw(GLuint program);

	glm::vec2 getLimit(int id, int axis);
	glm::vec3 getLocalAngle(int id);

	void updateJoint(int id, float angleStep, int axis);
	int getNNid(glm::vec2 screenpos, glm::mat4 V, glm::mat4 P, int w, int h);

	int IKsolver(int id, glm::vec3 goal);// niter, goal boolean
	vector<Joint*> getAncestors(int id);
};