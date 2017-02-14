#include <algorithm>
#include "SimulatedRobot.h"

SimulatedRobot::SimulatedRobot(House& house, string algorithmName, AbstractAlgorithm& algorithm, const map<string, int> configurations, int maxSteps) {
	this->algorithmName = algorithmName;
	this->house = house;
	this->algorithm = &algorithm;
	this->config = configurations;
	this->maxSteps = maxSteps;
	battery = Battery(config["BatteryCapacity"], config["BatteryConsumptionRate"], config["BatteryRechargeRate"]);
}

void SimulatedRobot::InitSimulation() {
	pos = house.getDockPosition();
	stepsDid = dirtCleaned = 0;
	stepsLeft = maxSteps;
	algorithm->setSensor(sensor);
	algorithm->setConfiguration(config);
	prevStep = Direction::Stay;
	status = SimulationStatus::Running;
}

void SimulatedRobot::RunStep() {
	UpdateSensor();
	prevStep = algorithm->step(prevStep);
	MoveRobot(prevStep);
	UpdateSimulationStatus();
}

void SimulatedRobot::MoveRobot(Direction dir) {
	pos.Move(dir);
	battery.UpdateBatteryLife(IsInDocking());
	stepsDid++;
	stepsLeft--;
}

void SimulatedRobot::UpdateSimulationStatus() {

	//simulation finished - crashed into a wall
	if (IsWall(pos)) {
		status = SimulationStatus::Crashed;
		crashDetails << "Algorithm " << algorithmName << " when running on House " << house.getFilenameNoSuffix() << " went on a wall in step " << stepsDid << endl;
		return;
	}

	//simulation finished - battery is dead
	if (battery.IsBatteryDead()) {
		status = SimulationStatus::Dead;
		return;
	}

	//simulation finished - success
	if (dirtCleaned == house.getInitalDirt() && IsInDocking())
	{
		status = SimulationStatus::Done;
		return;
	}

	//simulation finished - out of steps
	if (stepsLeft == 0)
	{
		status = SimulationStatus::Stopped;
//		house.Print();
		return;
	}

	//passed on dirty floor and cleaned it
	if (house.isDirtyCell(house.getPositionValue(pos))) {
		house.CleanDirt(pos);
		dirtCleaned++;
	}

	//let the algorithm know it has few steps left
	if (stepsLeft == config["MaxStepsAfterWinner"]) algorithm->aboutToFinish(stepsLeft);
	
	//Finally, if -video is set, print house to file:
	if (video) { house.PrintVideo(stepsDid, algorithmName, pos, prevStep); }
}

void SimulatedRobot::UpdateSensor()
{
	bool isWall[4];

	char posValue = house.getPositionValue(pos);
	int dirtLevel = house.isDirtyCell(posValue) ? posValue - '0' : 0;

	for (int i = 0; i < 4; i++)
	{
		int dir = i % 2 == 0 ? 1 : -1;
		int row = i < 2 ? pos.getRow() : pos.getRow() + dir;
		int col = i >= 2 ? pos.getCol() : pos.getCol() + dir;

		isWall[i] = IsWall(Position(row, col));
	}
	sensor.UpdateSensor(dirtLevel, isWall);
}

void SimulatedRobot::WeHaveAWinner()
{
	stepsLeft = min(stepsLeft, config["MaxStepsAfterWinner"]);
	if (stepsLeft == config["MaxStepsAfterWinner"]) algorithm->aboutToFinish(stepsLeft);
}