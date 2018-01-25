#include "Parser.h"

using namespace std;

Parser::Parser()
{
	// Skin keywords
	skinKeywords.push_back("positions");
	skinKeywords.push_back("normals");
	skinKeywords.push_back("skinweights");
	skinKeywords.push_back("triangles");
	skinKeywords.push_back("bindings");
}

Parser::~Parser()
{
}

Skin* Parser::readSkin(const char*filepath)
{
	ifstream file(filepath);
	string line;

	if (!file) 
	{
		cout << "Cannot open input file.\n" << endl;
		return nullptr;
	}

	Skin* skin = new Skin();
	glm::vec2 check(-1, 0);
	int vcounter = 0;
	int ncounter = 0;
	int fcounter = 0;

	// Read line by line
	for (line; getline(file, line);)
	{
		istringstream iss(line);

		// Positions
		if (check[0] == 0)
		{
			string p0, p1, p2;
			iss >> p0;
			iss >> p1;
			iss >> p2;

			//cout << p0 << " " << p1 << " " << p2 << endl;
			//skin->positions.push_back(glm::vec3(stoi(p0), stoi(p1), stoi(p2)));
			
			Vertex* V = new Vertex();
			V->setIndex(vcounter);
			V->setPosition(glm::vec3(stoi(p0), stoi(p1), stoi(p2)));
			skin->vertices.push_back(V);
			vcounter++;

			check[1]--;
			if (check[1] == 0)
			{
				check[0] = -1;
				continue;
			}
		}
		// Normals
		else if (check[0] == 1)
		{
			string p0, p1, p2;
			iss >> p0;
			iss >> p1;
			iss >> p2;

			//cout << p0 << " " << p1 << " " << p2 << endl;
			//skin->normals.push_back(glm::vec3(stoi(p0), stoi(p1), stoi(p2)));
			
			skin->vertices[ncounter]->setNormal(glm::vec3(stoi(p0), stoi(p1), stoi(p2)));
			ncounter++;

			check[1]--;
			if (check[1] == 0)
			{
				check[0] = -1;
				continue;
			}
		}
		// Triangles
		else if (check[0] == 3)
		{
			string p0, p1, p2;
			iss >> p0;
			iss >> p1;
			iss >> p2;

			//cout << p0 << " " << p1 << " " << p2 << endl;
			//skin->faces.push_back(glm::vec3(stoi(p0), stoi(p1), stoi(p2)));

			Face* F = new Face();
			F->setIndex(fcounter);
			F->setVertices(skin->vertices[stoi(p0)], skin->vertices[stoi(p1)], skin->vertices[stoi(p2)]);
			skin->faces.push_back(F);
			fcounter++;

			check[1]--;
			if (check[1] == 0)
			{
				check[0] = -1;
				continue;
			}
		}
		// Weights
		else if (check[0] == 2)
		{
			string num, index, weight;
			iss >> num;

			vector<int>* weightInds = new vector<int>();
			vector<float>* weights = new vector<float>();

			for (int i = 0; i < 2 * stoi(num); i+=2)
			{
				iss >> index;
				iss >> weight;
				weightInds->push_back(stoi(index));
				weights->push_back(stoi(weight));
				//cout << index << " " << weight << endl;
			}

			skin->weightInds.push_back(weightInds);
			skin->weights.push_back(weights);

			check[1]--;
			if (check[1] == 0)
			{
				check[0] = -1;
				continue;
			}
		}
		// Binding matrices
		else if (check[0] == 4)
		{
			string r0;
			iss >> r0;

			if (r0 == "matrix")
			{
				check[1]--;
				continue;
			}
			else if (r0 == "}")
				continue;
			else
			{
				string r1, r2;
				iss >> r1;
				iss >> r2;

				//cout << r0 << " " << r1 << " " << r2 << endl;
				tempB.push_back(glm::vec3(stoi(r0), stoi(r1), stoi(r2)));
			}
			
			if (check[1] == 0)
			{
				check[0] = -1;
				continue;
			}
		}
		else if (line[0] == '}')
		{
			continue;
		}
		// Headers
		else
		{
			// Read word by word
			do {
				string word, num;
				iss >> word;
				iss >> num;

				// Find keywords
				check = processKeyword(word, stoi(num));
				if (check[0] != -1)
					break;
			} while (iss);
		}
	}

	vec2matConverter(skin);
	skin->print();

	cout << "[Read] .skin file successfully read" << endl;
	return skin;
}

glm::vec2 Parser::processKeyword(string word, int num)
{
	for (int i = 0; i < skinKeywords.size(); i++)
	{
		if (word == skinKeywords[i])
			if (word == "bindings")
				return glm::vec2(i, num * 4);
			else
				return glm::vec2(i, num);
	}
	return glm::vec2(-1, 0);
}

void Parser::vec2matConverter(Skin * skin)
{
	for (int i = 0; i < tempB.size(); i += 4)
	{
		glm::mat4 B(1.0f);
		B[0] = glm::vec4(tempB[i], 0.0f);
		B[1] = glm::vec4(tempB[i + 1], 0.0f);
		B[2] = glm::vec4(tempB[i + 2], 0.0f);
		B[3] = glm::vec4(tempB[i + 3], 1.0f);
		B = glm::transpose(B);
		skin->Bmatrices.push_back(B);
	}
}
