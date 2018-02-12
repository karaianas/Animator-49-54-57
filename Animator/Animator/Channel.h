#pragma once
#include "Core.h"
#include "Keyframe.h"

using namespace std;

class Channel {
public:
	Channel(int expIn_, int expOut_);

	float Evaluate(float time);
	void AddKeyframe(float time, float value, int iRule, int oRule);
	void Precompute();


	// Constant: 0 Cycle: 1 Cycle_offset: 2
	int expIn, expOut;
	vector<Keyframe*> keyframes;
};