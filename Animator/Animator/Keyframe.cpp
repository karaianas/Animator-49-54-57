#include "Keyframe.h"

using namespace std;

Keyframe::Keyframe(float time_, float value_, int iRule_, int oRule_)
{
	time = time_;
	value = value_;
	iRule = iRule_;
	oRule = oRule_;

	coeff = glm::vec4(0.0f);
	span = 0.0f;
}
