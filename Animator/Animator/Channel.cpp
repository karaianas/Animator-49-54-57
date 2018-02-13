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

void Channel::Precompute()
{

}

void Channel::ComputeTangents()
{
	int id = 0;
	for (auto keyframe : keyframes)
	{
		// Flat
		if (keyframe->iRule == 0)
			keyframe->tangentIn = 0.0f;
		// Linear
		else if (keyframe->iRule == 1)
		{
			if (id == 0)
			{
				keyframe->tangentIn = 0.0f;
			}
			else
			{
				Keyframe* last = GetLast(id);
				float den = keyframe->time - last->time;
				float num = keyframe->value - last->value;
				keyframe->tangentIn = num / den;
			}
		}
		// Smooth
		else if (keyframe->iRule == 2)
		{
			// Use linear instead
			if (id = 0)
			{
				keyframe->tangentIn = 0.0f;
			}
			else if (id == keyframes.size() - 1)
			{
				Keyframe* last = GetLast(id);
				float den = keyframe->time - last->time;
				float num = keyframe->value - last->value;
				keyframe->tangentIn = num / den;
			}
			else
			{
				Keyframe* last = GetLast(id);
				Keyframe* next = GetNext(id);
				float den = next->time - last->time;
				float num = next->value - last->value;
				keyframe->tangentIn = num / den;
			}
		}

		// Flat
		if (keyframe->oRule == 0)
			keyframe->tangentOut = 0.0f;
		// Linear
		else if (keyframe->oRule == 1)
		{
			if (id == keyframes.size() - 1)
			{
				keyframe->tangentOut = 0.0f;
			}
			else
			{
				Keyframe* next = GetNext(id);
				float den = keyframe->time - next->time;
				float num = keyframe->value - next->value;
				keyframe->tangentOut = num / den;
			}
		}
		// Smooth
		else if (keyframe->oRule == 2)
		{
			// Use linear instead
			if (id = 0)
			{
				Keyframe* next = GetNext(id);
				float den = keyframe->time - next->time;
				float num = keyframe->value - next->value;
				keyframe->tangentOut = num / den;
			}
			else if (id == keyframes.size() - 1)
			{
				keyframe->tangentOut = 0.0f;
			}
			else
			{
				Keyframe* last = GetLast(id);
				Keyframe* next = GetNext(id);
				float den = next->time - last->time;
				float num = next->value - last->value;
				keyframe->tangentOut = num / den;
			}
		}

		id++;
	}
}

void Channel::ComputeHermite()
{
	float p0, p1, v0, v1;

	for (auto keyframe : keyframes)// minus 1
	{

	}
}

Keyframe * Channel::GetNext(int id)
{
	return keyframes[(id + 1) % keyframes.size()];
}

Keyframe * Channel::GetLast(int id)
{
	return keyframes[(id - 1) % keyframes.size()];
}
