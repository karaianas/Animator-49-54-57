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

	// Read line by line
	for (line; getline(file, line);)
	{
		// Read word by word
		istringstream iss(line);
		do {
			string word;
			iss >> word;
			//cout << temp << endl;
			if (word == "positions")
			{
				string num;
				iss >> num;
				processKeyword(iss, word, stoi(num));
			}
		} while (iss);

	}
}

void Parser::processKeyword(istringstream & iss, string keyword, int num)
{
	//cout << keyword << num << endl;
}
