////////////////////////////////////////
// Tokenizer.cpp
////////////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS

#include "Tokenizer.h"
using namespace std;
////////////////////////////////////////////////////////////////////////////////

Tokenizer::Tokenizer() {
	File = 0;
	LineNum = 0;
	strcpy(FileName, "");
}

////////////////////////////////////////////////////////////////////////////////

Tokenizer::~Tokenizer() {
	if (File) {
		printf("ERROR: Tokenizer::~Tokenizer()- Closing file '%s'\n", FileName);
		fclose((FILE*)File);
	}
}

////////////////////////////////////////////////////////////////////////////////

bool Tokenizer::Open(const char *fname) {
	File = (void*)fopen(fname, "r");
	LineNum = 1;
	if (File == 0) {
		printf("ERROR: Tokenzier::Open()- Can't open file '%s'\n", fname);
		return false;
	}
	strcpy(FileName, fname);
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool Tokenizer::Close() {
	if (File) fclose((FILE*)File);
	else return false;

	File = 0;
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool Tokenizer::Abort(char *error) {
	printf("ERROR '%s' line %d: %s\n", FileName, LineNum, error);
	Close();
	return false;
}

////////////////////////////////////////////////////////////////////////////////

char Tokenizer::GetChar() {
	char c = char(getc((FILE*)File));
	if (c == '\n') LineNum++;
	return c;
}

////////////////////////////////////////////////////////////////////////////////

char Tokenizer::CheckChar() {
	int c = getc((FILE*)File);
	ungetc(c, (FILE*)File);
	return char(c);
}

////////////////////////////////////////////////////////////////////////////////

int Tokenizer::GetInt() {
	SkipWhitespace();
	int pos = 0;
	char temp[256];

	// Get first character ('-' or digit)
	char c = CheckChar();
	if (c == '-') {
		temp[pos++] = GetChar();
		c = CheckChar();
	}
	if (!isdigit(c)) {
		printf("ERROR: Tokenizer::GetInt()- Expecting int on line %d of '%s'\n", LineNum, FileName);
		return 0;
	}
	temp[pos++] = GetChar();

	// Get integer potion
	while (isdigit(c = CheckChar())) temp[pos++] = GetChar();

	// Finish
	temp[pos++] = '\0';
	return atoi(temp);
}

////////////////////////////////////////////////////////////////////////////////

// BUG: can't parse ".2", "f", or "F"
// Uses: [-]I[.[I]][(e|E)[+|-]I]
// Should use: [+|-](I|I.|.I|I.I)[(e|E)[+|-]I][f|F]

float Tokenizer::GetFloat() {
	SkipWhitespace();
	int pos = 0;
	char temp[256];

	// Get first character ('-' or digit)
	char c = CheckChar();
	if (c == '-') {
		temp[pos++] = GetChar();
		c = CheckChar();
	}
	if (!isdigit(c)) {
		printf("ERROR: Tokenizer::GetFloat()- Expecting float on line %d of '%s' '%c'\n", LineNum, FileName, c);
		return 0.0f;
	}
	temp[pos++] = GetChar();

	// Get integer potion of mantissa
	while (isdigit(c = CheckChar())) temp[pos++] = GetChar();

	// Get fraction component
	if (c == '.') {
		temp[pos++] = GetChar();
		while (isdigit(c = CheckChar())) temp[pos++] = GetChar();
	}

	// Get exponent
	if (c == 'e' || c == 'E') {
		temp[pos++] = GetChar();
		c = CheckChar();
		if (c == '+' || c == '-') {
			temp[pos++] = GetChar();
			c = CheckChar();
		}
		if (!isdigit(c)) {
			printf("ERROR: Tokenizer::GetFloat()- Poorly formatted float exponent on line %d of '%s'\n", LineNum, FileName);
			return 0.0f;
		}
		while (isdigit(c = CheckChar())) temp[pos++] = GetChar();
	}

	// Finish
	temp[pos++] = '\0';
	return float(atof(temp));
}

////////////////////////////////////////////////////////////////////////////////

bool Tokenizer::GetToken(char *str) {
	SkipWhitespace();

	int pos = 0;
	char c = CheckChar();
	while (c != ' ' && c != '\n' && c != '\t' && c != '\r' && !feof((FILE*)File)) {
		str[pos++] = GetChar();
		c = CheckChar();
	}
	str[pos] = '\0';
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool Tokenizer::FindToken(const char *tok) {
	int pos = 0;
	while (tok[pos] != '\0') {
		if (feof((FILE*)File)) return false;
		char c = GetChar();
		if (c == tok[pos]) pos++;
		else pos = 0;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool Tokenizer::SkipWhitespace() {
	char c = CheckChar();
	bool white = false;
	while (isspace(c)) {
		GetChar();
		c = CheckChar();
		white = true;
	}
	return white;
}

////////////////////////////////////////////////////////////////////////////////

bool Tokenizer::SkipLine() {
	char c = GetChar();
	while (c != '\n') {
		if (feof((FILE*)File)) return false;
		c = GetChar();
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool Tokenizer::Reset() {
	if (fseek((FILE*)File, 0, SEEK_SET)) return false;
	return true;
}

////////////////////////////////////////////////////////////////////////////////
Joint* Tokenizer::readSkel()
{
	Joint* J = new Joint();
	J->parent = nullptr;

	char c = this->GetChar();
	vector<char>* jname = new vector<char>();

	while (c != EOF)
	{
		if (c == 'n' && this->CheckChar() == 't')
		{
			c = this->GetChar();
			c = this->GetChar();
			while (c != '{')
			{
				jname->push_back(c);
				c = this->GetChar();
			}
		}

		if (c == '{')
		{
			this->addChild(J, jname);
			break;
		}
		c = this->GetChar();
	}

	return J;
}

void Tokenizer::addChild(Joint* parent, vector<char>* parent_name)
{
	this->addInfo(parent);
	this->addName(parent, parent_name);
	vector<char>* jname = new vector<char>();

	char c = this->GetChar();
	while (c != EOF)
	{
		if (c == 'n' && this->CheckChar() == 't')
		{
			c = this->GetChar();
			c = this->GetChar();
			while (c == ' ')
				c = this->GetChar();
			while (c != '{')
			{
				jname->push_back(c);
				c = this->GetChar();
			}
		}

		if (c == '{')
		{
			Joint* child = new Joint();
			child->parent = parent;
			parent->children.push_back(child);
			this->addChild(child, jname);
		}
		else if (c == '}')
			return;
		
		c = this->GetChar();
	}
}

void Tokenizer::addInfo(Joint * curr)
{
	int count = 0;
	while (this->CheckChar() != '{' && this->CheckChar() != '}')
	{
		char c = this->GetChar();
		this->addVal(curr, c, 'o', 'f', 0);
		this->addVal(curr, c, 'm', 'i', 1);
		this->addVal(curr, c, 'm', 'a', 2);
		this->addVal(curr, c, 't', 'x', 3);
		this->addVal(curr, c, 't', 'y', 4);
		this->addVal(curr, c, 't', 'z', 5);
		this->addVal(curr, c, 'p', 'o', 6);
	}
	curr->checkValid();
}

void Tokenizer::addVal(Joint * curr, char c, char comp1, char comp2, int ind)
{
	int count = 0;
	int limit;
	if (ind >= 3 && ind <= 5)
		limit = 2;
	else
		limit = 3;
	if (c == comp1 && this->CheckChar() == comp2)
	{
		char tmp = this->GetChar();
		while (tmp != '\n' && count < limit)
		{
			if (!isalpha(tmp))
			{
				float val = this->GetFloat();
				curr->container[ind]->push_back(val);
				count++;
			}
			tmp = this->GetChar();
		}
	}
}

void Tokenizer::addName(Joint * curr, vector<char>* jname)
{
	curr->name = jname;
}
