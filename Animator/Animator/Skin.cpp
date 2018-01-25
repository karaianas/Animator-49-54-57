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

	//cout << "# positions: " << positions.size() << endl;
	//cout << positions[0].x << " " << positions[1].y << " " << positions[2].z << endl;

	//cout << "# normals: " << normals.size() << endl;
	//cout << normals[0].x << " " << normals[1].y << " " << normals[2].z << endl;

	cout << "# wInd: " << weightInds.size() << endl;
	cout << weightInds[0]->size() << endl;

	cout << "# Bmatrices: " << Bmatrices.size() << endl;
	cout << Bmatrices[1][0][0] << " " << Bmatrices[1][0][1] << endl;
}
