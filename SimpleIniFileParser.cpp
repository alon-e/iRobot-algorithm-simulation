#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "SimpleIniFileParser.h"

using namespace std;

void SimpleIniFileParser::loadFromFile(const string& iniPath)
{
	this->parameters.clear();
	ifstream fin(iniPath.c_str());
	if (!fin.is_open())
	{
		cout << "config.ini exists in '" << iniPath << "' but cannot be opened" << endl;
		return;
	}
	string line;
	while (getline(fin, line))
	{
		this->processLine(line);
	}
}

vector<string> SimpleIniFileParser::split(const string &s, char delim) 
{
	vector<string> elems;
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

string SimpleIniFileParser::trim(string& str)
{
	str.erase(0, str.find_first_not_of(' '));       //prefixing spaces
	str.erase(str.find_last_not_of(' ') + 1);         //surfixing spaces
	return str;
}

void SimpleIniFileParser::processLine(const string& line)
{
	vector<string> tokens = split(line, '=');
	if (tokens.size() != 2) return;
	try { this->parameters[trim(tokens[0])] = stoi(tokens[1]); }
	catch (invalid_argument exception) { this->parameters[trim(tokens[0])] = -1; } //mark for invalid values
}
