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
#include "Vertex.h"
#include "Face.h"
#include "Joint.h"

using namespace std;

class Skin {
public:
	Skin();
	~Skin();

	bool isTex;
	glm::vec2 lightMode;

	glm::mat4 worldM;
	vector<glm::vec3> positions;
	vector<glm::vec3> normals;
	vector<GLuint> indices;
	vector<glm::vec2> texcoords;
	GLuint VBO_p, VBO_n, VBO_t, VAO, EBO;
	GLuint uMVP, uITM;
	GLuint uModel, uLight;
	GLuint texture, uTexture;
	//GLuint uColor;

	// Test zone
	vector<glm::vec4> jIndices;
	vector<glm::vec4> jWeights;
	vector<glm::mat4> WBmatrices;
	void testZone();
	GLuint VBO_i, VBO_w, uB;
	
	void init(vector<Joint*>* ptr);
	void draw(GLuint shaderProgram);
	void update(float phi, int flag);
	void computeWB();

	void print();
	void setJointsPtr(vector<Joint*>* ptr);

	//ref: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/
	GLuint loadBMP_custom(const char * imagepath);

	vector<Joint*>* jointsPtr;
	vector<Vertex*> vertices;
	vector<Face*> faces;
	vector<glm::mat4> Bmatrices;
};