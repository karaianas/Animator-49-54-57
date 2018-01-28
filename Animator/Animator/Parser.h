#pragma once
#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include "Core.h"
#include "Skin.h"
#include "Vertex.h"
#include "Face.h"

#include <fstream>
#include <string>
#include <sstream>

using namespace std;

class Parser {
public:
	Parser();
	~Parser();

	Skin* readSkin(const char* filepath);
	int readMorph(const char* filepath, Skin* skin);

	glm::vec2 processKeyword(string word, int num);
	void vec2matConverter(Skin* skin);

	vector<string> skinKeywords;
	vector<glm::vec3> tempB;
};