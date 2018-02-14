#pragma once
#include "Core.h"
#include "Channel.h"
#include "Model.h"

using namespace std;

class Animation {
public:

	Animation();

	void readAnim(const char * filepath);
	glm::vec2 processAnimKeyword(string word0, string word1, int num);

	Channel* AddChannel(int expIn, int expOut);
	void SetRange(float start_, float end_);
	void Play(Model* M, float delta);

	void DisplayChannel(int mode, Model* model, int id, GLuint shaderProgram, glm::mat4 M, glm::mat4 MVP);

	float start, end;
	int numChannels;
	vector<Channel*> channels;
private:

};