#ifndef __VALIDATIONS__H_
#define __VALIDATIONS__H_

#include <vector>
#include <map>
#include "House.h"
#include "AbstractAlgorithm.h"

typedef int calc_t(const map<string, int>& score_params);


class Validator
{
public:
	static string getShortFilename(string const& fullname);

	static bool SetConfigurationsFile(string& configDir, map<string,int>& configurations);

	static calc_t* SetScoreFormula(string& scoreFormulaDir, void*& dlScore);

	static bool SetAlgorithms(string algorithmsDir, vector<void*>& dlibs, void*& dlScore, vector<string>& algorithmErrors);

	static bool SetHouses(string housesDir, vector<House>& houses, vector<string>& houseErrors, vector<void*>& dlibs, void*& dlScore);

	static int SetNumThreads(string threadCount);

private:
	static bool EndsWith(string const& fullString, string const& ending);

	static bool IsValidConfigFile(string configPath, map<basic_string<char>, int> configurations);

	static vector<string> GetFilesFromDir(string& dir, const string& suffix);
};

#endif //__VALIDATIONS__H_
