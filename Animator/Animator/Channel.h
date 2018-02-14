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
#include "Keyframe.h"

using namespace std;

class Channel {
public:
	Channel(int expIn_, int expOut_);

	void AddKeyframe(float time, float value, int iRule, int oRule);
	void Precompute();
	void ComputeTangents();
	void ComputeHermite();
	void DrawUpdate();
	float Evaluate(float time);

	void Draw(GLuint shaderProgram, glm::mat4 MVP, int DOF);

	Keyframe* GetNext(int id);
	Keyframe* GetLast(int id);


	// Constant: 0 Cycle: 1 Cycle_offset: 2 
	int expIn, expOut;
	vector<Keyframe*> keyframes;

	// For drawing
	GLuint VAO_inter, VAO_key;
	GLuint VBO_inter, VBO_key;
	vector<glm::vec2> vertices_inter;
	vector<glm::vec2> vertices_key;
};