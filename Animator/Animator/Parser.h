#pragma once
#include "Core.h"

#include <fstream>
#include <string>
#include <sstream>

using namespace std;

class Parser {
public:
	Parser();
	~Parser();

	void readSkin(const char* filepath);
	glm::vec2 processKeyword(string word, int num);

	vector<string> skinKeywords;

	// test
	vector<glm::vec3> positions;
	vector<glm::vec3> normals;
};