#pragma once

#include "Core.h"

class Keyframe {
public:

	Keyframe(float time_, float value_, int iRule_, int oRule_);

	float time;
	float value;
	float tangentIn, tangentOut;
	char ruleIn, ruleOut; // Tangent rules

	// Flat: 0 Linear: 1 Smooth: 2
	int iRule, oRule;
	float A, B, C, D; // Cubic coefficients
};