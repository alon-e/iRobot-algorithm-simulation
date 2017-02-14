#include <set>
#include <sstream>
#include <boost/filesystem.hpp>
// #include <boost/foreach.hpp> 
#include "Validator.h"
#include "SimpleIniFileParser.h"
#include "AlgorithmRegistrar.h"
#include "Constants.h"

#ifdef LINUX
#include <dlfcn.h>
#endif

std::string Validator::getShortFilename(string const& fullname)
{
	boost::filesystem::path p(fullname);
	return p.filename().string();
}

bool Validator::SetConfigurationsFile(string& configDir, map<string,int>& configurations)
{
	vector<string> configFiles = GetFilesFromDir(configDir, "config.ini");
	if (configFiles.size() == 0)
	{
		cout << USAGE_MSG << endl;
		cout << "cannot find config.ini file in " << configDir << endl;
		return false;
	}

	string configPath = configFiles.back();
	SimpleIniFileParser iniParser(configPath);
	configurations = iniParser.toMap();

	return IsValidConfigFile(configPath, configurations);
}

calc_t* Validator::SetScoreFormula(string& scoreFormulaDir, void*& dlScore)
{
	if (scoreFormulaDir.empty()) return nullptr;
#ifdef LINUX
	vector<string> scoreFormulaFiles = GetFilesFromDir(scoreFormulaDir, "score_formula.so");
	if (scoreFormulaFiles.size() == 0)
	{
		cout << USAGE_MSG << endl;
		cout << "cannot find score_formula.so file in " << scoreFormulaDir << endl;
		return nullptr;
	}

	dlScore = dlopen(scoreFormulaFiles.front().c_str(), RTLD_NOW);
	if (dlScore == nullptr)
	{
		cout << "score_formula.so exists in " << scoreFormulaDir << " but cannot be opened or is not a valid .so" << endl;
		return nullptr;
	}
	dlerror();
	calc_t* calcScore = *reinterpret_cast<calc_t**>((dlsym(dlScore, "calc_score")));
	const char* dlsym_error = dlerror();
	if (dlsym_error) 
	{
		cout << INVALID_SCORE_FORMULA << endl;
		return nullptr;
	}
	return calcScore;
#endif
	return nullptr;
}

bool Validator::SetAlgorithms(string algorithmsDir, vector<void*>& dlibs, void*& dlScore, vector<string>& algorithmErrors)
{
#ifdef LINUX
	bool foundValidAlgorithm = false;
	unsigned int numAlgorithms = 0;
	vector<string> algorithmsFiles = GetFilesFromDir(algorithmsDir, "_.so");

	if (algorithmsFiles.size() == 0)
	{
		cout << USAGE_MSG << endl;
		cout << "cannot find algorithm files in " << algorithmsDir << endl;
		if (dlScore) dlclose(dlScore);
		return false;
	}
	void* dlib;
	AlgorithmRegistrar& registrar = AlgorithmRegistrar::getInstance();

	for (string algorithmFile : algorithmsFiles)
	{
		string algorithmName = getShortFilename(algorithmFile);
		string algorithmNameNoSuffix = algorithmName.substr(0, algorithmName.find_last_not_of("so"));

		int result = registrar.LoadAlgorithm(algorithmFile, algorithmNameNoSuffix, dlib);
		
		if(result != AlgorithmRegistrar::ALGORITHM_REGISTERED_SUCCESSFULY) 
		{
			if (result == AlgorithmRegistrar::FILE_CANNOT_BE_LOADED) 
			{
				algorithmErrors.push_back(algorithmName + ": " + INVALID_SO);
				continue;
			}

			if (result == AlgorithmRegistrar::NO_ALGORITHM_REGISTERED) 
			{
				algorithmErrors.push_back(algorithmName + ": " + NO_REG_SO);
				continue;
			}
		}	
		dlibs.push_back(dlib);

		numAlgorithms++;
		foundValidAlgorithm = true;
	}

	if (!foundValidAlgorithm)
	{
		cout << "All algorithm files in target folder '" << algorithmsDir << "' cannot be opened or are invalid:" << endl;
		for (string error : algorithmErrors) cout << error << endl;
		for (auto dl : dlibs) dlclose(dl);
		if (dlScore) dlclose(dlScore);
		return false;
	}
#endif
	return true;
}

bool Validator::SetHouses(string housesDir, vector<House>& houses, vector<string>& houseErrors, vector<void*>& dlibs, void*& dlScore)
{
	bool foundValidHouse = false;
	vector<string> housesFiles = GetFilesFromDir(housesDir, ".house");

	if (housesFiles.size() == 0)
	{
		cout << USAGE_MSG<< endl;
		cout << "cannot find house files in " << housesDir << endl;
#ifdef LINUX
		for (auto dl : dlibs) dlclose(dl);
		if (dlScore) dlclose(dlScore);
#endif
		return false;
	}

	for (string houseFile : housesFiles)
	{
		House house(houseFile);
		if (house.isValidHouse())
		{
			foundValidHouse = true;
			houses.insert(houses.begin(), house);
		}
		else
			houseErrors.push_back(getShortFilename(house.getFilename()) + ": " + house.getHouseStatus());
	}

	if (!foundValidHouse)
	{
		cout << "All house files in target folder '" << housesDir << "' cannot be opened or are invalid:" << endl;
		for (string error : houseErrors) cout << error << endl;
#ifdef LINUX
		for (auto dl : dlibs) dlclose(dl);
		if (dlScore) dlclose(dlScore);
#endif
		return false;
	}
	return true;
}

 int Validator::SetNumThreads(string threadStr)
 {
	try 
	{ 
		int numThreads = stoi(threadStr);
		return numThreads <= 0 ? DEFAULT_NUM_THREADS : numThreads;
	}
	catch (...) { return DEFAULT_NUM_THREADS; };
 }

bool Validator::IsValidConfigFile(string configPath, map<string, int> configurations)
{
	int numMissingParameters = 0, numBadParameters = 0;
	stringstream missingParameters, badParameters;
	set<string> expectedParameters = { "MaxStepsAfterWinner", "BatteryCapacity", "BatteryConsumptionRate", "BatteryRechargeRate" };

	for (string parameter : expectedParameters)
	{
		//parameter not found in config file
		if (configurations.find(parameter) == configurations.end())
		{
			numMissingParameters == 0 ? missingParameters << parameter : missingParameters << ", " << parameter;
			numMissingParameters++;
			continue;
		}

		//parameter found - check if valid value
		if (configurations[parameter] < 0)
		{
			numBadParameters == 0 ? badParameters << parameter : badParameters << ", " << parameter;
			numBadParameters++;
		}
	}

	if (numMissingParameters > 0) cout << "config.ini missing " << numMissingParameters <<
		" parameter(s): " << missingParameters.str() << endl;

	if (numBadParameters > 0) cout << "config.ini having bad values for " << numBadParameters <<
		" parameter(s): " << badParameters.str() << endl;

	return numMissingParameters == 0 && numBadParameters == 0;
}

vector<string> Validator::GetFilesFromDir(string& dir, const string& suffix)
{
	vector<string> files;

	boost::filesystem::path folder(dir);
	folder = complete(folder);

	if (!is_directory(folder)) return files;
	
	dir = boost::filesystem::canonical(folder).string();
	
	vector<boost::filesystem::path> filesPath;
	copy(boost::filesystem::directory_iterator(folder), boost::filesystem::directory_iterator(), back_inserter(filesPath));
	sort(filesPath.begin(), filesPath.end());

	for (boost::filesystem::path path : filesPath)
	{
		if (is_directory(path)) continue; //eliminate directories

		string fileName = path.string();

		if (EndsWith(fileName, suffix))
			files.push_back(fileName);
	}
	return files;
}

// vector<string> Validator::GetFilesFromDir(string& dir, const string& suffix)
// {
// 	vector<string> files;

// 	boost::filesystem::path folder(dir);
// 	folder = complete(folder);

// 	if (!is_directory(folder)) return files;
	
// 	// dir = boost::filesystem::canonical(folder).string();
	
// 	// vector<boost::filesystem::path> filesPath;
// 	// copy(boost::filesystem::directory_iterator(folder), boost::filesystem::directory_iterator(), back_inserter(filesPath));
// 	// sort(filesPath.begin(), filesPath.end());

// 	boost::filesystem::directory_iterator it(folder), eod;

// 	BOOST_FOREACH(boost::filesystem::path file, std::make_pair(it, eod))   
// 	{ 
// 		if (is_directory(file)) continue; //eliminate directories

// 		string fileName = file.string();

// 		if (EndsWith(fileName, suffix))
// 			files.push_back(fileName);
// 	}

// 	// for (boost::filesystem::path path : boost::filesystem::directory_iterator(folder))
// 	// {
// 	// 	if (is_directory(path)) continue; //eliminate directories

// 	// 	string fileName = path.string();

// 	// 	if (EndsWith(fileName, suffix))
// 	// 		files.push_back(fileName);
// 	// }
// 	return files;
// }

bool Validator::EndsWith(string const &fullString, string const &ending) 
{
	if (fullString.length() < ending.length()) return false;

	return (fullString.compare(fullString.length() - ending.length(), ending.length(), ending) == 0);
}

