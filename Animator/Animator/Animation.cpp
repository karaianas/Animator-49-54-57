#include "Animation.h"

using namespace std;

Animation::Animation()
{
	for (int i = -10; i < 11; i++)
		vertices_graph.push_back(glm::vec2(i, 0));

	glGenVertexArrays(1, &VAO_graph);
	glGenBuffers(1, &VBO_graph);

	glBindVertexArray(VAO_graph);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_graph);
	glBufferData(GL_ARRAY_BUFFER, vertices_graph.size() * sizeof(glm::vec2), vertices_graph.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Animation::readAnim(const char * filepath)
{
	ifstream file(filepath);
	string line;

	if (!file)
	{
		cout << "Cannot open input anim file.\n" << endl;
		return;
	}

	Channel* curr = new Channel(-1, -1);
	string w0, w1, w2, w3;

	// Read line by line
	for (line; getline(file, line);)
	{
		istringstream iss(line);

		string word;
		iss >> word;

		if (word == "range")
		{
			iss >> w0 >> w1;
			SetRange(stof(w0), stof(w1));
		}
		else if (word == "numchannels")
		{
			iss >> w0;
			numChannels = stoi(w0);
		}
		else if (word == "channel")
		{
			getline(file, line);
			istringstream iss(line);
			iss >> w0 >> w1 >> w2;
			glm::vec2 rules = processAnimKeyword(w1, w2, 0);
			curr = AddChannel(rules[0], rules[1]);
		}
		else if (word == "keys")
		{
			iss >> w0;
			int numKeys = stoi(w0);

			// Add keyframes to the current channel
			for (int i = 0; i < numKeys; i++)
			{
				getline(file, line);
				istringstream iss(line);

				iss >> w0 >> w1 >> w2 >> w3;
				glm::vec2 rules = processAnimKeyword(w2, w3, 1);
				curr->AddKeyframe(stof(w0), stof(w1), rules[0], rules[1]);
			}
		}
	}

	// Precomputation
	for (auto channel : channels)
		channel->Precompute();
}

glm::vec2 Animation::processAnimKeyword(string word0, string word1, int num)
{
	glm::vec2 numbers;

	// Extrapolation rule
	if (num == 0)
	{
		if (word0 == "constant")
			numbers[0] = 0;
		else if (word0 == "cycle")
			numbers[0] = 1;
		else if (word0 == "cycle_offset")
			numbers[0] = 2;
		else
			numbers[0] = -1;

		if (word1 == "constant")
			numbers[1] = 0;
		else if (word1 == "cycle")
			numbers[1] = 1;
		else if (word1 == "cycle_offset")
			numbers[1] = 2;
		else
			numbers[1] = -1;
	}
	// Tanget rule
	else if (num == 1)
	{
		if (word0 == "flat")
			numbers[0] = 0;
		else if (word0 == "linear")
			numbers[0] = 1;
		else if (word0 == "smooth")
			numbers[0] = 2;
		else
			numbers[0] = -1;

		if (word1 == "flat")
			numbers[1] = 0;
		else if (word1 == "linear")
			numbers[1] = 1;
		else if (word1 == "smooth")
			numbers[1] = 2;
		else
			numbers[1] = -1;
	}

	return numbers;
}


Channel* Animation::AddChannel(int expIn, int expOut)
{
	Channel* channel = new Channel(expIn, expOut);
	channels.push_back(channel);

	//cout << expIn << " " << expOut << endl;

	return channel;
}

void Animation::SetRange(float start_, float end_)
{
	start = start_;
	end = end_;
}

void Animation::Play(Model* M, float delta)
{
	// Translation
	float x = channels[0]->Evaluate(delta);
	float y = channels[1]->Evaluate(delta);
	float z = channels[2]->Evaluate(delta);
	M->updateJointT(0, glm::vec3(x, y, z));

	// Rotation
	for (int i = 0; i < M->allJoints.size(); i++)
	{

		float angleX = channels[3 + 3 * i]->Evaluate(delta);
		float angleY = channels[3 + 3 * i + 1]->Evaluate(delta);
		float angleZ = channels[3 + 3 * i + 2]->Evaluate(delta);
	
		M->updateJointXYZ(i, glm::vec3(angleX, angleY, angleZ));
	}
}

void Animation::DisplayChannel(Joint* joint, GLuint shaderProgram, glm::mat4 MVP, int DOF)
{
	GLuint uMVP = glGetUniformLocation(shaderProgram, "MVP");
	GLuint uChannel = glGetUniformLocation(shaderProgram, "channel");

	// Now send these values to the shader program
	glUniformMatrix4fv(uMVP, 1, GL_FALSE, &MVP[0][0]);

	// Draw graph
	glBindVertexArray(VAO_graph);
	glUniform3f(uChannel, 1.0f, 1.0f, 1.0f);
	glDrawArrays(GL_LINE_STRIP, 0, vertices_graph.size());
	glBindVertexArray(0);

	if (joint)
	{
		glm::vec3 current = joint->localA;

		channels[3 + 3 * joint->id + DOF]->Draw(shaderProgram, MVP, DOF);
	}

	// This will be used for marking the current position
	//if (id >= 0)
	//{
	//	Joint* j = model->allJoints[id];
	//	glm::vec3 current = j->localA;
	//	//cout << current[0] << " " << current[1] << " " << current[2] << endl;

	//	int chNum = 3 + 3 * id + mode;
	//	channels[chNum]->Draw(shaderProgram, M, MVP, mode);
	//}

}
