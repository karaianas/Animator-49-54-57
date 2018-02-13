#include "Joint.h"


Joint::Joint()
{
	// Allocation
	for (int i = 0; i < 7; i++)
	{
		vector<float>* temp = new vector<float>();
		container.push_back(temp);
	}

	// Address setting
	container[0] = &offset;
	container[1] = &boxmin;
	container[2] = &boxmax;
	container[3] = &rotxlim;
	container[4] = &rotylim;
	container[5] = &rotzlim;
	container[6] = &pose;

	scaleM = glm::mat4(1.0f);
	localA = glm::vec3();
	
}

Joint::~Joint()
{
}

void Joint::draw(GLuint program, Cube* cube, glm::mat4 M)
{
	glm::mat4 S1 = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f));
	
	// Joint
	cube->draw(program, M * worldM * S1, glm::vec3(1.0f, 0.0f, 0.0f));
	
	// Bone
	cube->draw(program, M * worldM * offLocalM * scaleM, glm::vec3(1.0f, 1.0f, 1.0f));//offWorldM * scaleM);
}

void Joint::update(float angle_step, int axis)
{
	if (axis < 0)
		return;
	glm::vec3 temp(0.0f, 0.0f, 0.0f);
	temp[axis] = 1.0f;
	localA[axis] += angle_step;

	localM = localM * glm::rotate(glm::mat4(1.0f), angle_step, temp);
}

void Joint::IKupdate(glm::vec3 angle)
{
	glm::mat4 R_x = glm::mat4(); 
	glm::mat4 R_y = glm::mat4(); 
	glm::mat4 R_z = glm::mat4(); //glm::rotate(angle[2], glm::vec3(0, 0, 1));

	float x = localA[0] + angle[0];
	float y = localA[1] + angle[1];
	float z = localA[2] + angle[2];

	if (x >= container[3]->at(0) && x <= container[3]->at(1))
		R_x = glm::rotate(angle[0], glm::vec3(1, 0, 0));
	if (y >= container[4]->at(0) && y <= container[4]->at(1))
		R_y = glm::rotate(angle[1], glm::vec3(0, 1, 0));
	if (z >= container[5]->at(0) && z <= container[5]->at(1))
		R_z = glm::rotate(angle[2], glm::vec3(0, 0, 1));

	localM = localM * R_z * R_y * R_x;
	localA += angle;
}

void Joint::animUpdate(glm::vec3 angle)
{
	glm::mat4 R_x = glm::mat4();
	glm::mat4 R_y = glm::mat4();
	glm::mat4 R_z = glm::mat4(); 

	float x = angle[0];
	float y = angle[1];
	float z = angle[2];

	if (x >= container[3]->at(0) && x <= container[3]->at(1))
		R_x = glm::rotate(angle[0], glm::vec3(1, 0, 0));
	if (y >= container[4]->at(0) && y <= container[4]->at(1))
		R_y = glm::rotate(angle[1], glm::vec3(0, 1, 0));
	if (z >= container[5]->at(0) && z <= container[5]->at(1))
		R_z = glm::rotate(angle[2], glm::vec3(0, 0, 1));

	glm::vec4 temp = localM[3];
	localM = R_z * R_y * R_x;
	localM[3] = temp;
	localA = glm::vec3(x, y, z);
}

void Joint::addChild(Joint * child)
{
	children.push_back(child);
}

void Joint::checkValid()
{
	if (pose.size() < 3)
	{
		pose.push_back(0.0f);
		pose.push_back(0.0f);
		pose.push_back(0.0f);
	}

	if (boxmin.size() < 3)
	{
		boxmin.push_back(-0.1f);
		boxmin.push_back(-0.1f);
		boxmin.push_back(-0.1f);
	}
	if (boxmax.size() < 3)
	{
		boxmax.push_back(0.1f);
		boxmax.push_back(0.1f);
		boxmax.push_back(0.1f);
	}

	for (int i = 3; i <= 5; i++)
		if (container[i]->size() < 2)
		{
			container[i]->push_back(-100.0f);
			container[i]->push_back(100.0f);
		}
}
