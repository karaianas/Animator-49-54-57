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
	glDeleteBuffers(1, &EBO);
}

void Skin::init(vector<Joint*>* ptr)
{
	jointsPtr = ptr;
	update();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &VBO2);
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

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Skin::draw(GLuint shaderProgram)
{
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

	for (int i = 0; i < V->jointId.size(); i++)
	{
		float w = V->jointW[i];
		int jointId = V->jointId[i];
		Joint* J = jointsPtr->at(jointId);
		glm::mat4 W = J->worldM;
		glm::mat4 B = Bmatrices[jointId];

		// Should prob use inverse transform for safety
		M += w * W * glm::inverse(glm::transpose(B));
	}


	if (normalize == 1)
	{
		glm::vec3 result = M * glm::vec4(V->n, 0.0f);
		return glm::normalize(result);
	}
	else
	{
		glm::vec3 result = M * glm::vec4(V->p, 1.0f);
		return result;
	}
}
