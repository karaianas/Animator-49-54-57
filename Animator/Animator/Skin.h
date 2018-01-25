#pragma once
#include "Core.h"

using namespace std;

class Skin {
public:
	Skin();
	~Skin();

private:
	vector<glm::vec3> positions;
	vector<glm::vec3> normals;
};