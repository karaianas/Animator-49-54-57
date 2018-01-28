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
	glm::mat4 worldM;
	vector<glm::vec3> positions;
	vector<glm::vec3> normals;
	vector<GLuint> indices;
	vector<glm::vec2> texcoords;
	GLuint VBO, VBO2, VBO3, VAO, EBO;
	GLuint uProjection, uModel, uView;
	GLuint texture, uTexture;
	//GLuint uColor;
	
	void init(vector<Joint*>* ptr);
	void draw(GLuint shaderProgram);
	void update();

	void print();
	void setJointsPtr(vector<Joint*>* ptr);
	glm::vec3 getDeform(int id, bool normalize);
	glm::vec3 getDeform2(int id, bool normalize);

	//ref: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/
	GLuint loadBMP_custom(const char * imagepath);

	vector<Joint*>* jointsPtr;
	vector<Vertex*> vertices;
	vector<Face*> faces;
	vector<glm::mat4> Bmatrices;// This should be inside Joint?

	// Morphing
	vector<float> phiDelta;
	vector<int> iDelta;
};