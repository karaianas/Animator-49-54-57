#pragma once
#include "Core.h"
#include "Channel.h"

using namespace std;

class Animation {
public:

	Animation();
	Channel* AddChannel(int expIn, int expOut);
	void SetRange(float start_, float end_);
	//void Play(Model* M, float s, float e, float delta);

	float start, end;
	int numChannels;
	vector<Channel*> channels;
private:

};