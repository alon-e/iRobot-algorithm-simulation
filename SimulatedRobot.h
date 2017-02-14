#ifndef __SIMULATEDROBOT__H_
#define __SIMULATEDROBOT__H_

#include <map>
#include <sstream>
#include "House.h"
#include "Direction.h"
#include "AbstractAlgorithm.h"
#include "Battery.h"
#include "Sensor.h"
#include "Constants.h"

extern bool video;

enum class SimulationStatus { Running, Stopped, Crashed, Dead, Done};

class SimulatedRobot {
	string algorithmName;
	map<string, int> config;
	House house;
	AbstractAlgorithm* algorithm;
	Sensor sensor;
	Battery battery;
	Position pos;
	Direction prevStep;
	SimulationStatus status;
	stringstream crashDetails;
	int maxSteps;
	int stepsDid;
	int stepsLeft;
	int dirtCleaned;

public:
	SimulatedRobot(House& house, string algorithmName, AbstractAlgorithm& algorithm, const map<string, int> configurations, int maxSteps);

	~SimulatedRobot() { delete algorithm; }

	string getHouseName() const { return house.getName(); }

	string getAlgorithmName() const { return algorithmName; }

	int getNumSteps() const { return stepsDid; }

	int getDirtCleaned() const { return dirtCleaned; }

	int getDirtLeft() const { return house.getInitalDirt() - getDirtCleaned(); }

	string getCrashDetails() const { return crashDetails.str(); }

	SimulationStatus getStatus() const { return status; };

	void setStatus(SimulationStatus status) { this->status = status; }

	void InitSimulation();

	void RunStep();

	bool IsInDocking() const {
		Position dockPos = house.getDockPosition();
		return pos.getRow() == dockPos.getRow() && pos.getCol() == dockPos.getCol();
	}

	void WeHaveAWinner();

private:
	void MoveRobot(Direction dir);

	void UpdateSimulationStatus();

	void UpdateSensor();

	bool IsWall(Position pos) const { return house.getPositionValue(pos) == WALL; }
};

#endif //__SIMULATEDROBOT__H_