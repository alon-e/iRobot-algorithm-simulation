#ifndef __SIMULATEDHOUSE__H_
#define __SIMULATEDHOUSE__H_

#include <list>
#include "House.h"
#include "AbstractAlgorithm.h"
#include "SimulatedRobot.h"
#include "Validator.h"

enum class HouseCompetitionStatus { Running, Finished, Invalid };

struct Competition
{
	map<SimulatedRobot*, int> algorithmsPositions;
	int currentPositon;
	int maxPosition;
	int winnerNumSteps;
	HouseCompetitionStatus status;
};

class SimulatedHouse
{
	House house;
	list<string> validRobots;
	list<SimulatedRobot*> activeRobots;
	list<SimulatedRobot*> inactiveRobots;
	int simulationSteps;
	Competition competition;

public:
	explicit SimulatedHouse(House house) { this->house = house; }

	~SimulatedHouse() { for (SimulatedRobot* robot : inactiveRobots) delete robot; }

	House getHouse() const { return house; }

	list<string> getAlgorithms() const { return validRobots; }

	list<SimulatedRobot*> getInactiveRobots() const { return inactiveRobots; }

	bool IsValidHouse() const { return house.isValidHouse(); }

	void InitSimulatedHouse(map<string, AbstractAlgorithm*>& algorithms, map<string, int>& configurations);

	void StepAllAlgorithms();

	int ComputeAlgorithmScore(string algorithmName, calc_t*& scoreFunc);

	HouseCompetitionStatus getCompetitionStatus() const { return competition.status; }

	void PrintHouseCompetitionScore(calc_t* scoreFunction);

private:
	void UpdateCompetitionStatus();

	static bool NotRunning(SimulatedRobot*& robot) { return robot->getStatus() != SimulationStatus::Running; }

	int getAlgorithmPositionInCompetition(SimulatedRobot* robot) {
		return robot->getStatus() == SimulationStatus::Done ? min(competition.algorithmsPositions[robot], 4) : 10;
	}

	int getActualAlgorithmPositionInCompetition(SimulatedRobot* robot) {
		return robot->getStatus() == SimulationStatus::Done ? competition.algorithmsPositions[robot] : 1 + competition.maxPosition;	
	}

	string HasFinishedSuccessfully(SimulatedRobot* robot) const {
		return robot->getStatus() == SimulationStatus::Done ? "Yes!" : "No!";
	}
};

#endif //__SIMULATEDHOUSE__H_