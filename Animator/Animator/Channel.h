#pragma once
#include "Core.h"
#include "Keyframe.h"

using namespace std;

class Channel {
public:
	Channel(int expIn_, int expOut_);

	void AddKeyframe(float time, float value, int iRule, int oRule);
	void Precompute();
	void ComputeTangents();
	void ComputeHermite();
	float Evaluate(float time);

	Keyframe* GetNext(int id);
	Keyframe* GetLast(int id);


	// Constant: 0 Cycle: 1 Cycle_offset: 2 
	int expIn, expOut;
	vector<Keyframe*> keyframes;
};