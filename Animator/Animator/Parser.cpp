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
	skinKeywords.push_back("texcoords");

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
		cout << "Cannot open input skin file.\n" << endl;
		return nullptr;
	}

	Skin* skin = new Skin();
	glm::vec2 check(-1, 0);
	int vcounter = 0;
	int ncounter = 0;
	int fcounter = 0;
	int wcounter = 0;
	int tcounter = 0;

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
			skin->positions.push_back(glm::vec3(stof(p0), stof(p1), stof(p2)));
			
			// Vertex class
			Vertex* V = new Vertex();
			V->setIndex(vcounter);
			V->setPosition(glm::vec3(stof(p0), stof(p1), stof(p2)));
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
			skin->normals.push_back(glm::vec3(stof(p0), stof(p1), stof(p2)));
			skin->vertices[ncounter]->setNormal(glm::vec3(stof(p0), stof(p1), stof(p2)));
			ncounter++;

			check[1]--;
			if (check[1] == 0)
			{
				check[0] = -1;
				continue;
			}
		}
		// Texcoords
		else if (check[0] == 5)
		{
			string p0, p1;
			iss >> p0;
			iss >> p1;

			//cout << p0 << " " << p1 << " " << endl;
			skin->texcoords.push_back(glm::vec2(stof(p0), stof(p1)));
			tcounter++;

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
			skin->indices.push_back(stoi(p0));
			skin->indices.push_back(stoi(p1));
			skin->indices.push_back(stoi(p2));

			// Face class
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
			
			int counter = 0;
			for (int i = 0; i < 2 * stoi(num); i+=2)
			{
				iss >> index;
				iss >> weight;
				//cout << index << " " << weight << endl;

				skin->vertices[wcounter]->jointId[counter] = (stoi(index));
				skin->vertices[wcounter]->jointW[counter] = (stof(weight));
				counter++;
			}
			
			wcounter++;

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
				tempB.push_back(glm::vec3(stof(r0), stof(r1), stof(r2)));
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
				if (word == "material" || word == "texture")
					break;

				// Find keywords
				check = processSkinKeyword(word, stoi(num));
				if (check[0] != -1)
					break;
			} while (iss);
		}
	}

	vec2matConverter(skin);

	cout << "# vertices: " << vcounter << endl;
	cout << "# normals: " << ncounter << endl;
	cout << "# triangles: " << fcounter << endl;
	cout << "# weights: " << wcounter << endl;
	cout << "# texCoords: " << tcounter << endl;

	if (tcounter > 0)
		skin-> isTex = true;
	//skin->print();
	cout << "[Read] .skin file successfully read" << endl;

	return skin;
}

int  Parser::readMorph(const char * filepath, Skin* skin, int flag)
{
	ifstream file(filepath);
	string line;

	if (!file)
	{
		cout << "Cannot open input morph file.\n" << endl;
		return -1;
	}

	glm::vec2 check(-1, 0);
	int vcounter = 0;
	int ncounter = 0;

	// Read line by line
	for (line; getline(file, line);)
	{
		istringstream iss(line);

		// Positions
		if (check[0] == 0)
		{
			string ind, p0, p1, p2;
			iss >> ind;
			iss >> p0;
			iss >> p1;
			iss >> p2;

			//cout << ind << ": " << p0 << " " << p1 << " " << p2 << endl;
			skin->vertices[stoi(ind)]->vDelta = glm::vec3(stof(p0), stof(p1), stof(p2)) - skin->vertices[stoi(ind)]->p;
			skin->vertices[stoi(ind)]->flag = flag;
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
			string ind, p0, p1, p2;
			iss >> ind;
			iss >> p0;
			iss >> p1;
			iss >> p2;

			//cout << ind << ": " << p0 << " " << p1 << " " << p2 << endl;
			skin->vertices[stoi(ind)]->nDelta = glm::vec3(stof(p0), stof(p1), stof(p2)) - skin->vertices[stoi(ind)]->n;
			ncounter++;

			check[1]--;
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
				check = processSkinKeyword(word, stoi(num));
				if (check[0] != -1)
					break;
			} while (iss);
		}
	}

	cout << "[Read] .morph file successfully read" << endl;
	return 0;
}

glm::vec2 Parser::processSkinKeyword(string word, int num)
{
	for (int i = 0; i < skinKeywords.size(); i++)
	{
		// No need to compare entire word
		if (word[0] == skinKeywords[i][0] && word[1] == skinKeywords[i][1])
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
		//B = glm::transpose(B);
		skin->Bmatrices.push_back(B);
	}
}
