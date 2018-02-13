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
	ComputeTangents();
	ComputeHermite();

	/*
	cout << "-------------" << endl;
	for (auto keyframe : keyframes)
	{
		cout << keyframe->tangentIn << " " << keyframe->tangentOut << endl;
		//cout << keyframe->coeff[0] << " " << keyframe->coeff[1] << " " << keyframe->coeff[2] << " " << keyframe->coeff[3] << endl;
	}

	cout << "value: ";
	float value = Evaluate(0.1f);
	cout << value << endl;

	cout << "value: ";
	value = Evaluate(1.2f);
	cout << value << endl;

	cout << "value: ";
	value = Evaluate(-0.5f);
	cout << value << endl;
	*/
}

void Channel::ComputeTangents()
{
	if (keyframes.size() == 1)
	{
		keyframes[0]->tangentIn = 0.0f;
		keyframes[0]->tangentOut = 0.0f;
		return;
	}

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
			if (id == 0)
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
				float den = next->time - keyframe->time;
				float num = next->value - keyframe->value;
				keyframe->tangentOut = num / den;
			}
		}
		// Smooth
		else if (keyframe->oRule == 2)
		{
			// Use linear instead
			if (id == 0)
			{
				Keyframe* next = GetNext(id);
				float den = next->time - keyframe->time;
				float num = next->value - keyframe->value;
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
	glm::mat4 M;
	M[0] = glm::vec4(2.0f, -3.0f, 0.0f, 1.0f);
	M[1] = glm::vec4(-2.0f, 3.0f, 0.0f, 0.0f);
	M[2] = glm::vec4(1.0f, -2.0f, 1.0f, 0.0f);
	M[3] = glm::vec4(1.0f, -1.0f, 0.0f, 0.0f);

	for (int i = 0; i < keyframes.size() - 1; i++)
	{
		Keyframe* keyframe = keyframes[i];
		Keyframe* next = keyframes[i + 1];

		p0 = keyframe->value;
		p1 = next->value;
		v0 = keyframe->tangentOut;
		v1 = next->tangentIn;

		float span = next->time - keyframe->time;
		float vspan = p1 - p0;
		keyframe->span = span;
		keyframe->coeff = M * glm::vec4(p0, p1, span * v0, span * v1);
		//cout << keyframe->coeff[0] << " " << keyframe->coeff[1] << " " << keyframe->coeff[2] << " " << keyframe->coeff[3] << endl;
	}
}

float Channel::Evaluate(float time)
{
	float start = keyframes.front()->time;
	float end = keyframes.back()->time;

	float t, x = 0.0f;
	if (time > end)
	{
		if (expOut == 0)
			return keyframes.back()->value;
		else if(expOut == 1)
			t = fmod(time - end, end - start) + start;
		else
		{
			t = fmod(time - end, end - start) + start;
			x = floor((time - start)/ (end - start));
		}
	}
	else if (time < start)
	{
		if (expIn == 0)
			return keyframes.front()->value;
		else if(expIn == 1)
			t = end - fmod(start - time, end - start);
		else
		{
			t = end - fmod(start - time, end - start);
			x = -floor((end - time)/( end - start));
		}
	}
	else
	{
		t = time;
	}

	float offset = keyframes.back()->value - keyframes.front()->value;

	// Need to optimize the search
	for (auto keyframe : keyframes)
	{
		if (t == keyframe->time)
			return keyframe->value + x * offset;
		else if (t < keyframe->time + keyframe->span)
		{
			float u = (t - keyframe->time) / (keyframe->span);
			glm::vec4 U(u*u*u, u*u, u, 1.0f);
			return glm::dot(U, keyframe->coeff) + x * offset;
		}
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
