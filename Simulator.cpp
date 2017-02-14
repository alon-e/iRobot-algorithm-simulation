#include <sstream>
#include <numeric>
#include <iomanip>
#include <boost/filesystem.hpp>
#include "Simulator.h"
#include "AlgorithmRegistrar.h"
#ifdef LINUX
#include <dlfcn.h> 
#include <thread>
#include <atomic>
#include "MakeUnique.h"
#include "Encoder.h"
#endif



int main(int argc, char** argv)
{
	string configDir = boost::filesystem::current_path().string();
	string scoreFormulaDir;
	string housesDir = boost::filesystem::current_path().string();
	string algorithmsDir = boost::filesystem::current_path().string();
	string threadString;

	for (int i = 1; i < argc - 1; i++) {
		//Won't search flag in last char*
		string arg(argv[i]);

		//search for -config to get path
		if (arg.compare("-config") == 0) configDir = complete(boost::filesystem::path(argv[i + 1])).string();
		
		//search for -score_formula to get path
		if (arg.compare("-score_formula") == 0) scoreFormulaDir = complete(boost::filesystem::path(argv[i + 1])).string();

		//search for -algorithm_path - directory to searh for .so files
		if (arg.compare("-algorithm_path") == 0) algorithmsDir = complete(boost::filesystem::path(argv[i + 1])).string();

		//search for -house_path - directory to searh for .house files
		if (arg.compare("-house_path") == 0) housesDir = complete(boost::filesystem::path(argv[i + 1])).string();

		if (arg.compare("-threads") == 0) threadString = argv[i + 1];

		if (arg.compare("-video") == 0) video = true;
		
	}
	//if last argument is video:
	if (string(argv[argc-1]).compare("-video") == 0) video = true;

	map<string, int> configurations;
	if (!Validator::SetConfigurationsFile(configDir, configurations)) return EXIT_FAILURE;

	string scoreError;
	void* dlScore = nullptr;
	calc_t* scoreFunc = Validator::SetScoreFormula(scoreFormulaDir, dlScore);
	scoreFunc = scoreFunc == nullptr ? calc_score : scoreFunc;
	
	vector<void*> dlibs;
	vector<string> algorithtmErrors;
	if (!Validator::SetAlgorithms(algorithmsDir, dlibs, dlScore, algorithtmErrors)) return EXIT_FAILURE;

	vector<House> houses;
	vector<string> houseErrors;
	if (!Validator::SetHouses(housesDir, houses, houseErrors, dlibs, dlScore)) return EXIT_FAILURE;
	
	//Validate thread count - default to THREAD_COUNT
	int threadCount = Validator::SetNumThreads(threadString);
	Simulator simulator(configurations, dlibs, dlScore, houses, threadCount);
	simulator.RunSimulationsThreadWrapper();
	//simulator.RunSimulations();
	simulator.PrintScores(scoreFunc, scoreError);

	if (video) { simulator.Encode(); }
	
	simulator.PrintErrors(algorithtmErrors, houseErrors, scoreError);
	
	AlgorithmRegistrar::getInstance().RemoveAlgorithms();
	return EXIT_SUCCESS;
}

Simulator::Simulator(map<string, int>& config, vector<void*>& dlibs, void*& dlScore, vector<House>& houses, int threadCount) {
	this->dlibs = dlibs;
	this->dlScore = dlScore;
	this->numThreads = threadCount;

	InitSimulatedHouses(config, houses);
}

Simulator::~Simulator()
{
	for (SimulatedHouse* simulation : inactiveHouses) delete simulation;
#ifdef LINUX 
	for (auto dl : dlibs) dlclose(dl);
	if (dlScore) dlclose(dlScore);
#endif
}

void Simulator::InitSimulatedHouses(map<string, int>& configurations, vector<House>& houses)
{
    AlgorithmRegistrar& registrar = AlgorithmRegistrar::getInstance();

	for (House house : houses)
	{
		map<string,AbstractAlgorithm*> algorithms;

		auto algorithmsList = registrar.getNewAlgorithmInstances();
		auto& algorithmsNames = registrar.getAlgorithmNames();
		auto pName = algorithmsNames.begin();
		
		for(auto& algorithm: algorithmsList) 
		{
			string algoName = *(pName++);
			algorithms[algoName] = algorithm.release();
		}

		activeHouses.push_back(new SimulatedHouse(house));
		activeHouses.back()->InitSimulatedHouse(algorithms, configurations);
		validHouses.push_front(house);
	}
}
void Simulator::RunSimulationsThreadWrapper()
{
	//Needed 'cus each thread need a 'run' function - using RunSimulations

	//init threads
	vector<unique_ptr<thread>> threads(numThreads);


	for(auto& thread_ptr : threads) {
		//execute threads
		thread_ptr = make_unique<thread>(&Simulator::RunSimulations, this); // create and run the thread
	}

	//wait for all threads
	for(auto& thread_ptr : threads) {
		thread_ptr->join();
	}
	//moved here to keep activeHouse consistent while treads run.
	activeHouses.remove_if(NotRunning);


}

void Simulator::RunSimulations()
{
	for(size_t index = activeHousesIndex++; index < activeHouses.size(); index = activeHousesIndex++) {

		//Work around for house list - can't point to activeHouses[index]
		list<SimulatedHouse*>::iterator simulation_itr = activeHouses.begin();
		if (activeHouses.size() > index)
		    simulation_itr = std::next(activeHouses.begin(), index);

		SimulatedHouse* simulation = (*simulation_itr);

		//each thread should process whole house competition.
		while(simulation->getCompetitionStatus() != HouseCompetitionStatus::Finished){
			if (simulation->getCompetitionStatus() == HouseCompetitionStatus::Running)
				simulation->StepAllAlgorithms();

			if (simulation->getCompetitionStatus() == HouseCompetitionStatus::Finished)
				inactiveHouses.push_front(simulation);
		}
	}
}

void Simulator::Encode() {
	//To Encode images into video:
	for (SimulatedHouse* simulation : inactiveHouses) {
		for (SimulatedRobot* simulationRobot : simulation->getInactiveRobots()) {
			string alogrithmName = simulationRobot->getAlgorithmName();
			string houseName = simulationRobot->getHouseName().substr(0,(simulationRobot->getHouseName()).find_last_not_of("\n"));
			
			string simulationDir =  "simulations/" + alogrithmName + "_" + houseName + "/";
			string imagesExpression = simulationDir + "image%5d.jpg";
			Encoder::encode(imagesExpression, alogrithmName + "_" + houseName + ".mpg");
		}
	}
	//rm simulations/
	string rmCmd = "rm -rf simulations/";
	int ret = system(rmCmd.c_str());
	if (ret == -1)
	{
	  	//assuming the folder can be deleted - do nothing
	}

}
void Simulator::PrintScores(calc_t* scoreFunction, string& scoreError)
{
//	for (SimulatedHouse* simulation : inactiveHouses) simulation->PrintHouseCompetitionScore(scoreFunction);
	
	stringstream seperator, algorithmName, houseName, avg;
	int seperatorLength = 14 + 11 * (validHouses.size() + 1) + 1;

	for (int i = 0; i < seperatorLength; i++) seperator << "-";
	
	cout << seperator.str() << endl;
	cout.width(14);
	cout << left << "|" << right << "|";
	
	for (House house : validHouses)
	{
		string houseFullName = Validator::getShortFilename(house.getFilename());
		string houseNoSuffix = houseFullName.substr(0, houseFullName.find(".house")) + SPACER;
		houseNoSuffix.resize(9);

		cout.width(10);
		cout << left << houseNoSuffix << right << "|";
	}
	cout.width(10);
	cout << left << "AVG" << right << "|" << endl;
	cout << seperator.str() << endl;

	multimap<double, string> lines = CreateScoresMatrix(scoreFunction, scoreError);

	for (auto it = lines.rbegin(); it != lines.rend(); ++it)
	{
		cout << it->second << endl;
		cout << seperator.str() << endl;
	}
}

multimap<double, string> Simulator::CreateScoresMatrix(calc_t* scoreFunction, string& scoreError)
{
	list<string> algorithms = inactiveHouses.front()->getAlgorithms();
	multimap<double, string> lines;

	for (string algorithmName : algorithms)
	{
		stringstream line;

		line.width(14);
		line << left << "|" + algorithmName << right << "|";
		double avg = GetAllScoresAndComputeAvg(algorithmName, scoreFunction, line, scoreError);
		lines.insert(pair<double, string>(avg, line.str()));
	}
	return lines;
}

double Simulator::GetAllScoresAndComputeAvg(string& algorithmName, calc_t* scoreFunction, stringstream& scoresString, string& scoreError)
{
	vector<double> scores;

	for (House house : validHouses)
	{
		for (SimulatedHouse* simulation : inactiveHouses)
		{
			if (house.getFilename().compare(simulation->getHouse().getFilename()) != 0) continue;

			scoresString.width(10);
			int simulationScore = simulation->ComputeAlgorithmScore(algorithmName, scoreFunction);
			scores.push_back(simulationScore);
			scoresString << right << simulationScore << "|";
			if (simulationScore == -1) scoreError = BAD_SCORE;
		}
	}

	double average = accumulate(scores.begin(), scores.end(), 0.0) / scores.size();
	scoresString.width(10);
	scoresString << right << setprecision(2) << fixed << average << "|";

	return average;
}

void Simulator::PrintErrors(vector<string>& algorithmErrors, vector<string>& houseErrors, string& scoreError) const
{
	if (!algorithmErrors.empty() || !houseErrors.empty())
	{
		cout << endl;
		cout << "Errors:" << endl;

		for (string error : houseErrors) cout << error << endl;
		for (string error : algorithmErrors) cout << error << endl;
	}

	for (SimulatedHouse* simulation : inactiveHouses)
	{
		for (SimulatedRobot* robot : simulation->getInactiveRobots())
		{
			cout << robot->getCrashDetails();
		}
	}

	if (!scoreError.empty()) cout << scoreError << endl;
}
