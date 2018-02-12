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
