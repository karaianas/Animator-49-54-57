#ifndef _CUBE_H_
#define _CUBE_H_

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
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

using namespace std;

class Cube
{
public:
	Cube();
	~Cube();

	glm::mat4 worldM;
	vector<glm::vec3> normals;
	glm::vec2 lightMode;

	void draw(GLuint, glm::mat4, glm::vec3);
	void update();
	void spin(float);

	//glm::vec3 color;

	// These variables are needed for the shader program
	GLuint VBO, VAO, EBO, VBO2;
	GLuint uProjection, uModel, uView;
	GLuint uColor, uLight;
};

// Define the coordinates and indices needed to draw the cube. Note that it is not necessary
// to use a 2-dimensional array, since the layout in memory is the same as a 1-dimensional array.
// This just looks nicer since it's easy to tell what coordinates/indices belong where.
const GLfloat vertices[8][3] = {
	// "Front" vertices
	{ -1.0, -1.0,  1.0 },{ 1.0, -1.0,  1.0 },{ 1.0,  1.0,  1.0 },{ -1.0,  1.0,  1.0 },
	// "Back" vertices
	{ -1.0, -1.0, -1.0 },{ 1.0, -1.0, -1.0 },{ 1.0,  1.0, -1.0 },{ -1.0,  1.0, -1.0 }
};

// Note that GL_QUADS is deprecated in modern OpenGL (and removed from OSX systems).
// This is why we need to draw each face as 2 triangles instead of 1 quadrilateral
const GLuint indices[6][6] = {
	// Front face
	{ 0, 1, 2, 2, 3, 0 },
	// Top face
	{ 1, 5, 6, 6, 2, 1 },
	// Back face
	{ 7, 6, 5, 5, 4, 7 },
	// Bottom face
	{ 4, 0, 3, 3, 7, 4 },
	// Left face
	{ 4, 5, 1, 1, 0, 4 },
	// Right face
	{ 3, 2, 6, 6, 7, 3 }
};


#endif
