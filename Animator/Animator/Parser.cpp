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

void Parser::readSkin(const char*filepath)
{
	ifstream file(filepath);
	string line;

	if (!file) {
		cout << "Cannot open input file.\n" << endl;
		return;
	}

	glm::vec2 check(-1, 0);

	// Read line by line
	for (line; getline(file, line);)
	{
		istringstream iss(line);

		// Positions and normals
		if (check[0] == 0 || check[0] == 1)
		{
			string p0, p1, p2;
			iss >> p0;
			iss >> p1;
			iss >> p2;

			// Should parse for values here
			cout << p0 << " " << p1 << " " << p2 << endl;
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
				check = processKeyword(word, stoi(num));
				if (check[0] != -1)
					break;
			} while (iss);
		}
	}
}

glm::vec2 Parser::processKeyword(string word, int num)
{
	for (int i = 0; i < skinKeywords.size(); i++)
	{
		if (word == skinKeywords[i])
			return glm::vec2(i, num);
	}
	return glm::vec2(-1, 0);
}
