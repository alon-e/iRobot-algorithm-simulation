#ifndef __SIMPLEINIFILEPARSER__H_
#define __SIMPLEINIFILEPARSER__H_

#include <map>
#include <vector>

using namespace std;

class SimpleIniFileParser {
	map<string, int> parameters;

public:
	explicit SimpleIniFileParser(const string& iniPath) { this->loadFromFile(iniPath); }

	void loadFromFile(const string& iniPath);

	map<string, int> toMap() const { return parameters; }

private:
	static vector<string> split(const string &s, char delim);

	static string trim(string& str);

	void processLine(const string& line);
};

#endif //__SIMPLEINIFILEPARSER__H_