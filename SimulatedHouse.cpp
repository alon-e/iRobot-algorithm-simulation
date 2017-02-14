#include <algorithm>
#include "SimulatedHouse.h"
#include "Validator.h"

void SimulatedHouse::InitSimulatedHouse(map<string, AbstractAlgorithm*>& algorithms, map<string, int>& configurations)
{
	if (house.isValidHouse() == false)
	{
		competition.status = HouseCompetitionStatus::Invalid;
		return;
	}
	competition.winnerNumSteps = simulationSteps = 0;
	competition.currentPositon = 1;
	competition.status = HouseCompetitionStatus::Running;

	for (auto algorithmPair : algorithms)
	{
		string algorithmName = algorithmPair.first;
		AbstractAlgorithm* algorithm = algorithmPair.second;

		activeRobots.push_back(new SimulatedRobot(house, algorithmName, *algorithm, configurations, house.getMaxSteps()));
		activeRobots.back()->InitSimulation();
		validRobots.push_back(algorithmName);
	}
}

void SimulatedHouse::StepAllAlgorithms()
{
	for (SimulatedRobot* robot : activeRobots)
	{
		robot->RunStep();
	}
	simulationSteps++;
	UpdateCompetitionStatus();
}

void SimulatedHouse::UpdateCompetitionStatus()
{
	bool isWinningRound = false;

	for (SimulatedRobot* robot : activeRobots)
	{
		//algorithm simulation finished
		if (robot->getStatus() != SimulationStatus::Running) inactiveRobots.push_back(robot);
		
		//robot cleaned house successfully
		if (robot->getStatus() == SimulationStatus::Done)
		{
			isWinningRound = true;
			if (competition.algorithmsPositions.empty())
			{	
				//first robot to finish - winner!	
				competition.winnerNumSteps = simulationSteps;

				//alert all other algorithms
				for (SimulatedRobot* algorithm : activeRobots)
				{
					if (algorithm->getStatus() == SimulationStatus::Running)
						algorithm->WeHaveAWinner();
				}
			}
			//set algorithm's position in competition
			competition.algorithmsPositions[robot] = competition.currentPositon;
		}
	}
	activeRobots.remove_if(NotRunning);
	if (isWinningRound) competition.currentPositon++;
	competition.maxPosition = competition.currentPositon;

	if (activeRobots.empty())
	{	//if winnerNumSteps = 0 => all algorithms failed
		competition.winnerNumSteps = competition.winnerNumSteps == 0 ? simulationSteps : competition.winnerNumSteps; 
		competition.status = HouseCompetitionStatus::Finished;
	}
}

//depricated - used for debug
void SimulatedHouse::PrintHouseCompetitionScore(calc_t* scoreFunction)
{
	cout << "House filename: " << Validator::getShortFilename(house.getFilenameNoSuffix()) << endl;
	cout << "House simulation Steps: " << simulationSteps << endl;

	for (SimulatedRobot* simulation : inactiveRobots)
	{
		string docking = simulation->IsInDocking() ? "Yes" : "No";
		int robotSteps = simulation->getStatus() == SimulationStatus::Dead ? simulationSteps : simulation->getNumSteps();

		cout << "Algorithm Name " << simulation->getAlgorithmName() << ":" << endl;
		cout << "\tHas Finished Successfully: " << HasFinishedSuccessfully(simulation) << endl;
		cout << "\tHas Returned To Docking Station: " << docking << endl;
		cout << "\tPosition In Competition: " << getAlgorithmPositionInCompetition(simulation) << endl;
		cout << "\tDirt Cleaned: " << simulation->getDirtCleaned() << endl;
		cout << "\tDirt Left: " << simulation->getDirtLeft() << endl;
		cout << "\tNum Robot Steps: " << robotSteps << endl;

		cout << "\tTotal Score :" << ComputeAlgorithmScore(simulation->getAlgorithmName(), scoreFunction) << endl << endl;
	}
}

int SimulatedHouse::ComputeAlgorithmScore(string algorithmName, calc_t*& scoreFunc)
{
	map<string, int> score_params;
	SimulatedRobot* robot = inactiveRobots.front();

	for (SimulatedRobot* algorithm : inactiveRobots)
	{
		if (algorithm->getAlgorithmName().compare(algorithmName) == 0)
		{
			robot = algorithm;
			break;
		}
	}
	if (robot->getStatus() == SimulationStatus::Crashed) return 0;

	score_params["actual_position_in_competition"] = getActualAlgorithmPositionInCompetition(robot);
	score_params["winner_num_steps"] = competition.winnerNumSteps;
	score_params["this_num_steps"] = robot->getStatus() == SimulationStatus::Dead ? simulationSteps : robot->getNumSteps();
	score_params["sum_dirt_in_house"] = house.getInitalDirt();
	score_params["dirt_collected"] = robot->getDirtCleaned();
	score_params["is_back_in_docking"] = robot->IsInDocking() ? 1 : 0;

	return scoreFunc(score_params);	
}