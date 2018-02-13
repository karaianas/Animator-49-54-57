#include "Model.h"
#define PI 3.14159

Model::Model()
{
	// One instance of cube to draw all joints and bones
	cube = new Cube();
}

Model::~Model()
{
}

Joint* Model::readSkel(const char * filepath)
{
	Tokenizer* T = new Tokenizer();
	T->Open(filepath);
	root = T->readSkel();

	// Set independent IDs
	int counter = 0;
	root->id = counter;
	allJoints.push_back(root);
	counter++;

	for (auto child : root->children)
		counter = setId(child, counter);

	worldM = glm::mat4(1.0f);

	// Set local matrices
	for (int i = 0; i < allJoints.size(); i++)
		calcLocalM(i);

	// Set world matrices
	calcWorldM(root, worldM);

	// Set scale matrices
	for (auto joint : allJoints)
	{
		for (int i = 0; i < 3; i++)
			joint->scaleM[i][i] = (joint->boxmax[i] - joint->boxmin[i]) / 2.0f;
	}

	// Set local angles
	for (auto joint : allJoints)
	{
		for (int i = 0; i < 3; i++)
		{
			float val = glm::clamp(joint->pose[i], joint->container[i + 3]->at(0), joint->container[i + 3]->at(1));
			joint->localA[i] = val;
		}
	}
	 
	cout << "Joint size: " << allJoints.size() << endl;
	return root;
}

Skin* Model::readSkin(const char * filepath)
{
	Parser* P = new Parser();
	skin = P->readSkin(filepath);

	// Initialize
	if (allJoints.size() > 0)
		skin->init(&allJoints);

	// Test zone
	// Read .morph if available
	if (string(filepath) == ".//Resources//skin//head.skin.txt" || string(filepath) == ".//Resources//skin//head_tex.skin.txt")
	{
		P->readMorph(".//Resources//morph//head2.morph.txt", skin, 2);
		P->readMorph(".//Resources//morph//head1.morph.txt", skin, 1);
		skin->update(0.0f, 0);
	}

	return skin;
}

void Model::readMorph(const char * filepath)
{
	if (skin)
	{
		Parser* P = new Parser();
		P->readMorph(filepath, skin, 0);
		skin->update(0.0f, 0);
	}

}

int Model::setId(Joint * curr, int id)
{
	curr->id = id;
	allJoints.push_back(curr);
	int counter = id + 1;

	for (auto child : curr->children)
		counter = setId(child, counter);

	return counter;
}

void Model::printAllJoints()
{
	for (auto joint : allJoints)
		cout << joint->id << endl;
}

void Model::calcLocalM(int id)
{
	Joint* curr = allJoints[id];

	// Limit DOFs
	float x = glm::clamp(curr->pose[0], curr->rotxlim[0], curr->rotxlim[1]);
	float y = glm::clamp(curr->pose[1], curr->rotylim[0], curr->rotylim[1]);
	float z = glm::clamp(curr->pose[2], curr->rotzlim[0], curr->rotzlim[1]);

	// Rotation mtx
	glm::mat4 R = glm::mat4(1.0f);
	R = glm::rotate(glm::mat4(1.0f), z, glm::vec3(0.0f, 0.0f, 1.0f));
	R = R * glm::rotate(glm::mat4(1.0f), y, glm::vec3(0.0f, 1.0f, 0.0f));
	R = R * glm::rotate(glm::mat4(1.0f), x, glm::vec3(1.0f, 0.0f, 0.0f));

	// Translation mtx
	glm::mat4 T = glm::mat4(1.0f);
	for(int i = 0; i < 3; i++)
		T[3][i] = curr->offset[i];

	curr->localM = T * R;

	// Extra translation mtx
	glm::mat4 ET = glm::translate(glm::mat4(1.0f), glm::vec3((curr->boxmax[0] + curr->boxmin[0]) / 2.0f, \
			(curr->boxmax[1] + curr->boxmin[1]) / 2.0f, \
			(curr->boxmax[2] + curr->boxmin[2]) / 2.0f));

	curr->offLocalM = ET;
}

void Model::calcWorldM(Joint * curr, glm::mat4 M)
{
	glm::mat4 newM = M * curr->localM;
	curr->worldM = newM;

	for (auto child : curr->children)
		calcWorldM(child, newM);
}

void Model::draw(GLuint program)
{
	for (auto child : allJoints)
		child->draw(program, cube, worldM);
}

glm::vec2 Model::getLimit(int id, int axis)
{
	float lim1 = allJoints[id]->container[axis + 3]->at(0);
	float lim2 = allJoints[id]->container[axis + 3]->at(1);
	glm::vec2 limit(lim1, lim2);
	return limit;
}

glm::vec3 Model::getLocalAngle(int id)
{
	return allJoints[id]->localA;
}

void Model::updateJoint(int id, float angleStep, int axis)
{
	Joint* selected = allJoints[id];
	glm::vec2 limit = getLimit(id, axis);

	if (selected->localA[axis] + angleStep >= limit[0] && selected->localA[axis] + angleStep <= limit[1])
	{
		selected->update(angleStep, axis);

		if (selected->parent)
			calcWorldM(selected, selected->parent->worldM);
		else
			calcWorldM(selected, worldM);
	}
}

void Model::updateJointXYZ(int id, glm::vec3 angleStep)
{
	Joint* selected = allJoints[id];

	selected->setUpdate(angleStep);

	if (selected->parent)
		calcWorldM(selected, selected->parent->worldM);
	else
		calcWorldM(selected, worldM);
}

int Model::getNNid(glm::vec2 screenpos, glm::mat4 V, glm::mat4 P, int w, int h)
{
	for (auto joint : allJoints)
	{
		glm::vec4 center = joint->getPos();
		center = P * V * center;
		glm::vec2 center2(center.x / center.w, center.y / center.w);
		glm::vec2 center3(center2.x * w / 2 + center2.x + w / 2, - (h / 2 * center2.y) + center2.y + h / 2);

		float len = glm::length(center3 - screenpos);

		if (len < 5.0f)
			return joint->id;
	}
	return -1;
}

int Model::IKsolver(int id, glm::vec3 goal)
{
	Joint* curr = allJoints[id];

	glm::vec3 eff = curr->getPos();
	glm::vec3 dP = goal - eff;

	vector<Joint*> ancestors = getAncestors(id);
	
	for (auto joint : ancestors)
	{
		// Get each Jacobian mtx
		glm::vec3 pos = joint->getPos();
		glm::mat3 jac(glm::cross(joint->getX(), eff - pos), \
			glm::cross(joint->getY(), eff - pos), \
			glm::cross(joint->getZ(), eff - pos));

		glm::vec3 dAngle = dP * jac;

		// Apply angle change by a small amount
		float thres = 0.5f * PI / 180.0f;

		float beta = thres / max(thres, max(max(abs(dAngle[0]), abs(dAngle[1])), abs(dAngle[2])));

		glm::vec3 A = dAngle * beta;

		joint->IKupdate(A);

		if (joint->parent)
			calcWorldM(joint, joint->parent->worldM);
		else
			calcWorldM(joint, worldM);
	}

	// If goal is reached
	float len = glm::length(glm::vec3(curr->getPos()) - goal);
	if (len > 0.2f)// 0.2f is 1/2 the length of the tip cube
	{
		//cout << "len: " << len << endl;
		return 0;
	}
	else
		return -1;
}

vector<Joint*> Model::getAncestors(int id)
{
	Joint* curr = allJoints[id];
	vector<Joint*> ancestors;
	Joint* parent = curr->parent;
	while (parent)
	{
		ancestors.push_back(parent);
		parent = parent->parent;
	}

	return ancestors;
}

