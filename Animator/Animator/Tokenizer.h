////////////////////////////////////////
// Tokenizer.h
////////////////////////////////////////

#pragma once

#include "Core.h"
#include <vector>
#include <iostream>
#include "Joint.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

// The Tokenizer class for reading simple ascii data files. The GetToken function
// just grabs tokens separated by whitespace, but the GetInt and GetFloat functions
// specifically parse integers and floating point numbers. SkipLine will skip to
// the next carraige return. FindToken searches for a specific token and returns
// true if it found it.

class Tokenizer {
public:
	Tokenizer();
	~Tokenizer();

	bool Open(const char *file);
	bool Close();

	bool Abort(char *error);	// Prints error & closes file, and always returns false

								// Tokenization
	char GetChar();
	char CheckChar();
	int GetInt();
	float GetFloat();
	bool GetToken(char *str);
	bool FindToken(const char *tok);
	bool SkipWhitespace();
	bool SkipLine();
	bool Reset();

	// Access functions
	char *GetFileName() { return FileName; }
	int GetLineNum() { return LineNum; }

	// My functions
	Joint* read();
	void addChild(Joint* parent, vector<char>* parent_name);
	void addInfo(Joint* curr);
	void addVal(Joint* curr, char c, char comp1, char comp2, int ind);
	void addName(Joint* curr, vector<char>* jname);


private:
	void *File;
	char FileName[256];
	int LineNum;
};

////////////////////////////////////////////////////////////////////////////////
