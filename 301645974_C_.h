
#ifndef __301645974_C___H_
#define __301645974_C___H_

#include <vector>
#include <algorithm>
#include "AbstractAlgorithm.h"
#include "Direction.h"
#include "Battery.h"
#include "Position.h"
#include "HouseMap.h"
#include "AlgorithmRegistration.h"

enum class AlgorithmState { MappingBorders, MappingHouse, Cleaning, GoingBackToDock, Charging };

inline ostream& operator<<(ostream& out, AlgorithmState state) {
	static const char* states[] = { "MappingBorders", "MappingHouse", "Cleaning", "GoingBackToDock", "Charging" };
	return out << states[int(state)];
}

class _301645974_C : public AbstractAlgorithm {
	HouseMap house;
	Position currentPos;
	Battery battery;
	const AbstractSensor* sensor;
	Direction prevStep;
	Direction suggestedStep;
	int stepsLeft;
	int simulationSteps;
	AlgorithmState state;
	int seed;

public:
	_301645974_C();

	virtual void setSensor(const AbstractSensor& sensor) override { this->sensor = &sensor; }

	virtual void setConfiguration(map<string, int> config) override;

	virtual Direction step(Direction lastStep) override;

	virtual void aboutToFinish(int stepsTillFinishing) override { simulationSteps = stepsTillFinishing; }

private:
	unsigned int RandomSeed() {
		seed = 8253729 * seed + 2396403;
		return seed % 32768;
	}

	void UpdateAlgorithm();

	void UpdatePosition() { currentPos.Move(prevStep); }

	void UpdateBattery() { battery.UpdateBatteryLife(IsInDocking()); }

	void UpdateHouse();

	void UpdateSteps() {
		simulationSteps--;
		stepsLeft = min(battery.getBatteryLife() / battery.getconsumptionRate(), simulationSteps);
	}

	void UpdateState();

	void MoveToMatrixView() {
		house.BordersRevealed();
		currentPos = currentPos + house.getDockingPos();
	}

	Direction Flip(Direction dir) const;

	Direction MapBorders() { return house.FindClosestUndiscoveredReachableSquareOnBorderNextStep(currentPos, RandomSeed()); }

	Direction MapHouse() { return house.FindClosestUndiscoveredReachableSquareNextStep(currentPos, RandomSeed()); }

	Direction Clean() { return house.FindClosestDirtySquareNextStep(currentPos, RandomSeed()); }

	Direction GoBackToDock() { return house.ShortestDistanceNextStep(currentPos, house.getDockingPos(), RandomSeed()); }

	bool IsInDocking() const { return currentPos == house.getDockingPos(); }

	bool IsRobotNeedToRecharge();

	bool IsRobotFinishedCleaning() { return house.IsAllHouseRevelead() && house.IsHouseClean(); }
};

#endif //__301645974_C___H_
