#include "Skin.h"
#include "Window.h"


Skin::Skin()
{
	worldM = glm::mat4(1.0f);
}

Skin::~Skin()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &VBO3);
	glDeleteBuffers(1, &EBO);
}

void Skin::init(vector<Joint*>* ptr)
{
	jointsPtr = ptr;
	update();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &VBO3);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	// Texture
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(glm::vec2), texcoords.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Skin::draw(GLuint shaderProgram)
{
	// Texture
	uTexture = glGetUniformLocation(shaderProgram, "textureSampler");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(uTexture, 0);

	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uView = glGetUniformLocation(shaderProgram, "view");
	uModel = glGetUniformLocation(shaderProgram, "model");

	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &worldM[0][0]);
	glUniformMatrix4fv(uView, 1, GL_FALSE, &Window::V[0][0]);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Skin::update()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		positions[i] = getDeform(i, 0);
		normals[i] = getDeform(i, 1);
	}
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Skin::print()
{
	cout << "# vertices: " << vertices.size() << endl;
	cout << "# faces: " << faces.size() << endl;
	
	cout << "Test weights: " << vertices[0]->jointId[0] << " " << vertices[0]->jointW[0] << endl;

	cout << "# Bmatrices: " << Bmatrices.size() << endl;
	//cout << Bmatrices[1][0][0] << " " << Bmatrices[1][0][1] << endl;

}

void Skin::setJointsPtr(vector<Joint*>* ptr)
{
	jointsPtr = ptr;
}

glm::vec3 Skin::getDeform(int id, bool normalize)
{
	Vertex* V = vertices[id];
	glm::mat4 M(0.0f);
	glm::vec4 tmp(0.0f, 0.0f, 0.0f, 0.0f);

	for (int i = 0; i < V->jointId.size(); i++)
	{
		float w = V->jointW[i];
		int jointId = V->jointId[i];
		Joint* J = jointsPtr->at(jointId);
		glm::mat4 W = J->worldM;
		glm::mat4 B = Bmatrices[jointId];

		// Should prob use inverse transform for safety
		M += w * W * glm::inverse(B);
		//M += w * W * glm::transpose(glm::inverse(B));
	}


	if (normalize == 1)
	{
		glm::vec3 result = M * glm::vec4(V->n, 0.0f);
		//cout << "n orig: " << result[0] << " " << result[1] << " " << result[2] << endl;
		return glm::normalize(result);
	}
	else
	{
		glm::vec3 result = M * glm::vec4(V->p, 1.0f);
		//cout << "p orig: " << V->p[0] << " " << V->p[1] << " " << V->p[2] << endl;
		return result;
	}
}

GLuint Skin::loadBMP_custom(const char * imagepath)
{
	printf("Reading image %s\n", imagepath);

	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	// Actual RGB data
	unsigned char * data;

	// Open the file
	FILE * file = fopen(imagepath, "rb");
	if (!file) {
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath);
		getchar();
		return 0;
	}

	// Read the header, i.e. the 54 first bytes

	// If less than 54 bytes are read, problem
	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		fclose(file);
		return 0;
	}
	// A BMP files always begins with "BM"
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		fclose(file);
		return 0;
	}
	// Make sure this is a 24bpp file
	if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    fclose(file); return 0; }
	if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    fclose(file); return 0; }

	// Read the information about the image
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)    imageSize = width*height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

										 // Create a buffer
	data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	// Everything is in memory now, the file can be closed.
	fclose(file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	// OpenGL has now copied the data. Free our own version
	delete[] data;

	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

	// ... nice trilinear filtering ...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// ... which requires mipmaps. Generate them automatically.
	glGenerateMipmap(GL_TEXTURE_2D);

	// Set textureID
	texture = textureID;

	// Return the ID of the texture we just created
	return textureID;
}