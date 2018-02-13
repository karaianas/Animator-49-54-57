#include "Animation.h"

using namespace std;

Animation::Animation()
{
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
