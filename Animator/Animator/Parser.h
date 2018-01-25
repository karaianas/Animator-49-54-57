#pragma once
#include "Core.h"
#include "Skin.h"

#include <fstream>
#include <string>
#include <sstream>

using namespace std;

class Parser {
public:
	Parser();
	~Parser();

	Skin* readSkin(const char* filepath);
	glm::vec2 processKeyword(string word, int num);
	void vec2matConverter(Skin* skin);

	vector<string> skinKeywords;
	vector<glm::vec3> tempB;
};