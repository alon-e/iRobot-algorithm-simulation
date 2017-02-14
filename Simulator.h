#ifndef __SIMULATOR__H_
#define __SIMULATOR__H_

#include <vector>
#include "SimulatedHouse.h"
#include "Validator.h"
#ifndef LINUX
#include <vld.h>
#endif

#ifdef LINUX
#include <atomic>
#endif

using namespace std;

extern "C" int calc_score(const map<string, int>& score_params);
bool video = false;


class Simulator
{
	list<SimulatedHouse*> activeHouses;
	list<SimulatedHouse*> inactiveHouses;
	list<House> validHouses;
	vector<void*> dlibs;
	void* dlScore;

	int numThreads;
	atomic_size_t activeHousesIndex{0};


public:
	Simulator(map<string, int>& config, vector<void*>& dlibs, void*& dlScore, vector<House>& houses, int threadCount);

	~Simulator();

	void InitSimulatedHouses(map<string, int>& config, vector<House>& houses);

	void RunSimulations();

	void RunSimulationsThreadWrapper();

	void Encode();

	void PrintScores(calc_t* scoreFunction, string& scoreError);

	void PrintErrors(vector<string>& algorithmErrors, vector<string>& houseErrors, string& scoreError) const;
	
private:
	static bool NotRunning(SimulatedHouse*& simulation) { return simulation->getCompetitionStatus() != HouseCompetitionStatus::Running; }

	multimap<double, string> CreateScoresMatrix(calc_t* scoreFunction, string& scoreError);
	
	double GetAllScoresAndComputeAvg(string& algorithmName, calc_t* scoreFunction, stringstream& scoresString, string& scoreError);
};

#endif //__SIMULATOR__H_