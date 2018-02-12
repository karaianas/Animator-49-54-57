#include "Channel.h"

Channel::Channel(int expIn_, int expOut_)
{
	expIn = expIn_;
	expOut = expOut_;
}

void Channel::AddKeyframe(float time, float value, int iRule, int oRule)
{
	Keyframe* key = new Keyframe(time, value, iRule, oRule);
	//cout << iRule << " " << oRule << endl;
	keyframes.push_back(key);
}
