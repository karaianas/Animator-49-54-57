#include "Animation.h"

using namespace std;

Animation::Animation()
{
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

//void Animation::Play(Model* M, float s, float e, float delta)
//{
//	//float angle = channels[3]->Evaluate(delta);
//	//M->updateJoint(1, angle, 0);
//
//	//angle = channels[4]->Evaluate(delta);
//	//M->updateJoint(1, angle, 1);
//
//	//angle = channels[5]->Evaluate(delta);
//	//M->updateJoint(1, angle, 2);
//}
