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
#include "Channel.h"
#include "Model.h"

using namespace std;

class Animation {
public:

	Animation();

	void readAnim(const char * filepath);
	glm::vec2 processAnimKeyword(string word0, string word1, int num);

	Channel* AddChannel(int expIn, int expOut);
	void SetRange(float start_, float end_);
	void Play(Model* M, float delta);

	void DisplayChannel(Joint* joint, GLuint shaderProgram, glm::mat4 MVP, float factor, float time);

	float start, end;
	int numChannels;
	vector<Channel*> channels;

	// Draw
	GLuint VAO_graph;
	GLuint VBO_graph;
	vector<glm::vec2> vertices_graph;

	int size;

private:

};