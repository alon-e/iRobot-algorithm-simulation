#ifndef _HOUSE_MAP__H_
#define _HOUSE_MAP__H_

#include <map>
#include "Position.h"
#include "Constants.h"

enum class HouseView { Map, Matrix };

class Path
{
	Position endPos;
	int distance;
	Direction nextStep;

public:
	Path(Position endPos, int distance, Direction nextStep) : endPos(endPos), distance(distance), nextStep(nextStep) {}

	Position getPosition() const { return endPos; }

	int getDistance() const { return distance; }

	Direction getNextStep() const { return nextStep; }
};

class Square {
	int row;
	int col;
	unsigned char value;

public:
	Square(int row, int col, unsigned char value) : row(row), col(col), value(value) {}

	Square(Position pos, unsigned char value) : row(pos.getRow()), col(pos.getCol()), value(value) {}

	int getRow() const { return row; }

	int getCol() const { return col; }

	char getValue() const { return value == '0' ? SPACE : value; }

	Position getPosition() const { return Position(row, col); }
};

class HouseMap
{
	map<Position, unsigned char> houseMap;
	unsigned char** houseMatrix;
	int eastBorder, westBorder, southBorder, northBorder;
	Position mapDockPos, matrixDockPos;
	int dirtLeft;
	bool isHouseFullyMapped;
	int distanceToDock;
	int distanceLastUpdate;
	HouseView view;

public:
	HouseMap()
	{
		mapDockPos = matrixDockPos = Position(0, 0);
		isHouseFullyMapped = false;
		eastBorder = westBorder = southBorder = northBorder = 0;
		dirtLeft = 0;
		view = HouseView::Map;
		setDistanceToDock(0);
	}

	~HouseMap() { DeleteMatrix(houseMatrix); }

	unsigned char operator[](const Position pos) const { return view == HouseView::Matrix ? GetFromMatrix(pos) : GetFromMap(pos); }

	void operator[](const Square square) {
		if (view == HouseView::Matrix) InsertToMatrix(square);
		else { InsertToMap(square); UpdateBorders(square.getPosition()); }
	}

	int getDistanceLastUpdate() const { return distanceLastUpdate; }

	int getDistanceToDock() const { return distanceToDock; }

	void setDistanceToDock(int distance) { distanceToDock = distance; distanceLastUpdate = 0; }

	void IncrementDistanceToDock() { if (distanceLastUpdate > 0) distanceToDock++; distanceLastUpdate++; }

	Position getDockingPos() const { return view == HouseView::Matrix ? matrixDockPos : mapDockPos; }

	HouseView getHouseView() const { return view; }

	int getEastBorder() const { return eastBorder; }

	int getWestBorder() const { return westBorder; }

	int getSouthBorder() const { return southBorder; }

	int getNorthBorder() const { return northBorder; }

	int getHeight() const { return southBorder - northBorder + 1; }

	int getWidth() const { return eastBorder - westBorder + 1; }

	void DecreaseDirt(Position pos);

	void AddToDirt(int dirt) { dirtLeft += dirt; }

	void BordersRevealed() {
		CreateMatrix();
		view = HouseView::Matrix;
	}

	void Print(unsigned char** matrix) const;

	Path FindPathToTarget(unsigned char** matrix, int seed);

	Direction FindClosestUndiscoveredReachableSquareOnBorderNextStep(Position startPos, int seed);

	Direction FindClosestUndiscoveredReachableSquareNextStep(Position startPos, int seed);

	Direction FindClosestDirtySquareNextStep(Position startPos, int seed);

	Direction ShortestDistanceNextStep(Position startPos, Position endPos, int seed);

	int FindShortestDistance(Position startPos, Position endPos);

	bool IsBordersMapped() const { return view == HouseView::Matrix; }

	bool IsDirtySquare(Position pos) const { return IsDirtySquare(operator[](pos)); }

	bool IsWall(Position pos) const { return operator[](pos) == WALL; }

	bool IsSquareAlreadyDiscovered(Position pos) const { return operator[](pos) != UNDISCOVERD_UNREACHABLE && operator[](pos) != UNDISCOVERD_REACHABLE; }

	bool IsUndiscoveredButReachableSquare(Position pos) const { return operator[](pos) == UNDISCOVERD_REACHABLE; }

	bool IsSquareAlreadyVisited(Position pos) const { return IsSquareAlreadyDiscovered(pos) && !IsWall(pos); }

	bool IsSquareReachable(Position pos) const { return IsSquareAlreadyVisited(pos) || IsUndiscoveredButReachableSquare(pos); }

	bool IsAllHouseRevelead();

	bool IsHouseClean() const { return dirtLeft == 0; }

	bool IsBordersDiscovered() const;

private:
	unsigned char GetFromMap(int row, int col) const { return GetFromMap(Position(row, col)); }

	unsigned char GetFromMap(const Position pos) const {
		if (houseMap.find(pos) == houseMap.end()) return UNDISCOVERD_UNREACHABLE;
		return houseMap.at(pos);
	}

	unsigned char GetFromMatrix(const Position pos) const { return houseMatrix[pos.getRow()][pos.getCol()]; }

	void InsertToMap(Square square) { houseMap[square.getPosition()] = square.getValue(); }

	void InsertToMatrix(Square square) { houseMatrix[square.getRow()][square.getCol()] = square.getValue(); }

	void UpdateBorders(Position pos);

	void CreateMatrix();

	void RemoveAllUndiscoverableSuqares();

	void DeleteMatrix(unsigned char** matrix) const;

	Direction GetFirstStepFromPath(unsigned char** matrix, int row, int col, unsigned char lastRoundChar) const;

	unsigned char** ClosestUndiscoveredReachableSquareOnBorderMatrix(Position startPos) const;

	unsigned char** ClosestUndiscoveredReachableSquareMatrix(Position startPos) const;

	unsigned char** DirtySquaresMatrix(Position startPos) const;

	unsigned char** FindShortestDistanceMatrix(Position startPos, Position endPos) const;

	unsigned char** BordersMatrix() const;

	unsigned char** CreateDistanceMatrix() const;

	bool IsHouseCompleted();

	bool IsSquareOnBorder(int row, int col) const { return row == 0 || row == getHeight() - 1 || col == 0 || col == getWidth() - 1; }

	bool IsBorderReachable(unsigned char** matrix, int i, int j) const;

	bool IsDirtySquare(unsigned char squareValue) const { return squareValue >= '1' && squareValue <= '9'; }
};

#endif //_HOUSE_MAP__H_

