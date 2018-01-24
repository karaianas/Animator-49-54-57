#pragma once
#include "Core.h"

#include <fstream>
#include <string>
#include <sstream>

using namespace std;

class Parser {
public:
	Parser();
	~Parser();

	void readSkin(const char* filepath);
	void processKeyword(istringstream & iss, string keyword, int num);

	vector<string> skinKeywords;
};