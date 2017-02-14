#include "301645974_C_.h"
#include "Constants.h"

_301645974_C::_301645974_C()
{
	simulationSteps = INT16_MAX;
	currentPos = Position(0, 0);
	suggestedStep = Direction::Stay;
	seed = 2005;
}

void _301645974_C::setConfiguration(map<string, int> config)
{
	battery = Battery(config["BatteryCapacity"], config["BatteryConsumptionRate"], config["BatteryRechargeRate"]);
	state = AlgorithmState::MappingBorders;
}

Direction _301645974_C::step(Direction lastStep)
{
	Direction move = Direction::Stay;
	prevStep = lastStep;
	UpdateAlgorithm();

	if (prevStep != suggestedStep) move = Flip(prevStep);

	else if (state == AlgorithmState::MappingBorders) move = MapBorders();

	else if (state == AlgorithmState::MappingHouse) move = MapHouse();

	else if (state == AlgorithmState::Cleaning) move = Clean();

	else if (state == AlgorithmState::GoingBackToDock) move = GoBackToDock();

	else if (state == AlgorithmState::Charging) move = Direction::Stay;

	suggestedStep = move;
	return move;
}

void _301645974_C::UpdateAlgorithm()
{
	UpdatePosition();
	UpdateBattery();
	UpdateHouse();
	UpdateSteps();
	UpdateState();
}

void _301645974_C::UpdateHouse()
{
	house.IncrementDistanceToDock();

	if (house.IsSquareAlreadyDiscovered(currentPos))
	{
		if (house.IsDirtySquare(currentPos)) house.DecreaseDirt(currentPos);
		return;
	}

	SensorInformation squareInfo = sensor->sense();
	unsigned char squareValue = IsInDocking() ? DOCK : squareInfo.dirtLevel + '0';

	house[Square(currentPos, squareValue)];
	house.AddToDirt(squareInfo.dirtLevel);

	for (int i = 0; i < 4; i++)
	{
		Position adjacentPos = currentPos + Direction(i);

		if (house.IsSquareAlreadyDiscovered(adjacentPos)) continue;

		unsigned char value = squareInfo.isWall[i] ? WALL : UNDISCOVERD_REACHABLE;
		house[Square(adjacentPos, value)];
	}

	if (house.getHouseView() != HouseView::Matrix && house.IsBordersDiscovered())
		MoveToMatrixView();
}

void _301645974_C::UpdateState()
{
	AlgorithmState lastState = state;

	//robot in docking
	if ((IsInDocking() && !battery.IsFullyCharged()) || (IsInDocking() && stepsLeft == 1))
	{
		//last step for simulation - stay in docking
		if (stepsLeft == 1)
			state = AlgorithmState::Charging;

		//start touring the house only if battery is fully charged
		else if (!battery.IsFullyCharged())
			if (lastState == AlgorithmState::GoingBackToDock || lastState == AlgorithmState::Charging)
				state = AlgorithmState::Charging;
	}

	//battery running low or robot is about to finish
	else if (IsRobotNeedToRecharge() || IsRobotFinishedCleaning())
		state = AlgorithmState::GoingBackToDock;

	//house is fully revealed - tour the house and clean it
	else if (house.IsAllHouseRevelead())
		state = AlgorithmState::Cleaning;

	//found borders - map the inside of the house
	else if (house.getHouseView() == HouseView::Matrix)
		state = AlgorithmState::MappingHouse;

	//map borders of the house
	else
		state = AlgorithmState::MappingBorders;
//
//	if (lastState != state)
//		cout << endl << endl << "State Changed from " << lastState << " to " << state << endl << endl;
}

Direction _301645974_C::Flip(Direction dir) const
{
	if (dir == Direction::Stay) return dir;
	int flip = int(dir) % 2 == 0 ? 1 : -1;
	return Direction(int(dir) + flip);
}

bool _301645974_C::IsRobotNeedToRecharge()
{
	if (house.getDistanceLastUpdate() > 0)
		if (house.getDistanceLastUpdate() == house.getHeight() * house.getWidth() / DISTANCE_FACTOR)
			house.setDistanceToDock(house.FindShortestDistance(currentPos, house.getDockingPos()));

	return stepsLeft <= house.getDistanceToDock() + 2;
}


#include "MakeUnique.h"
REGISTER_ALGORITHM (_301645974_C)
