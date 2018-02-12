#include "Keyframe.h"

using namespace std;

Keyframe::Keyframe(float time_, float value_, int iRule_, int oRule_)
{
	time = time_;
	value = value_;
	iRule = iRule_;
	oRule = oRule_;
}
