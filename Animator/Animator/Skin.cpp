#include "Skin.h"

Skin::Skin()
{
	
}

Skin::~Skin()
{
}

void Skin::print()
{
	cout << "# vertices: " << vertices.size() << endl;
	cout << "# faces: " << faces.size() << endl;
	
	cout << "Test weights: " << vertices[0]->jointId[0] << " " << vertices[0]->jointW[0] << endl;

	cout << "# Bmatrices: " << Bmatrices.size() << endl;
	cout << Bmatrices[1][0][0] << " " << Bmatrices[1][0][1] << endl;

}

void Skin::setJointsPtr(vector<Joint*>* ptr)
{
	jointsPtr = ptr;
}

glm::vec3 Skin::getDeformVertex(int id)
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

		M += w * W * glm::inverse(B);
	}

	glm::vec3 result(M * glm::vec4(V->p, 1.0f));
	cout << result[0] << " " << result[1] << " " << result[2] << endl;
	return result;
}
