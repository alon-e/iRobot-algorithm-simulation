#include <vector>
#include "HouseMap.h"
#ifndef LINUX
#include <windows.h>
#endif

void HouseMap::DecreaseDirt(Position pos)
{
	if (!IsDirtySquare(pos)) return;

	unsigned char value = operator[](pos) - 1;
	value = value == '0' ? SPACE : value;
	operator[](Square(pos, value));
	AddToDirt(-1);
}

void HouseMap::RemoveAllUndiscoverableSuqares()
{
	for (int i = 0; i < getHeight(); i++) {
		for (int j = 0; j < getWidth(); j++) {
			if (houseMatrix[i][j] == UNDISCOVERD_UNREACHABLE)
				houseMatrix[i][j] = WALL;
		}
	}
}

void HouseMap::UpdateBorders(Position pos)
{
	eastBorder = pos.getCol() > eastBorder ? pos.getCol() : eastBorder;
	westBorder = pos.getCol() < westBorder ? pos.getCol() : westBorder;
	southBorder = pos.getRow() > southBorder ? pos.getRow() : southBorder;
	northBorder = pos.getRow() < northBorder ? pos.getRow() : northBorder;
}

void HouseMap::Print(unsigned char** matrix) const
{
#ifndef LINUX
	Sleep(10);
	system("cls");
#endif

	cout << endl << endl;
	cout << "  ";
	for (auto j = 0; j < getWidth(); ++j)
		cout << j % 10;
	cout << endl;
	for (auto i = 0; i < getHeight(); ++i) {
		cout << i % 10 << " ";
		for (auto j = 0; j < getWidth(); ++j) {
			if (matrix[i][j] >= 128)
				cout << char((matrix[i][j] - 128) % 10 + '0');
			else
				cout << matrix[i][j];
		}
		cout << endl;
	}
}

Path HouseMap::FindPathToTarget(unsigned char** matrix, int seed)
{
	vector<Position> destinations;
	Position endPos;
	int round = 0;
	unsigned char lastRoundChar, thisRoundChar = 0;

	while (destinations.empty())
	{
		round++;
		thisRoundChar = round % 128 + 128;
		if (round == 1) lastRoundChar = ROBOT;
		else lastRoundChar = thisRoundChar == 128 ? 255 : thisRoundChar - 1;

		for (int i = 0; i < getHeight(); i++)
		{
			for (int j = 0; j < getWidth(); j++)
			{
				if (matrix[i][j] == lastRoundChar)
				{
					if (Position(i + northBorder, j + westBorder) == getDockingPos()) 
						setDistanceToDock(round - 1);

					for (int k = i - 1; k <= i + 1; k += 2)
					{
						if (k < 0 || k == getHeight()) continue;

						if (matrix[k][j] == TARGET)
						{
							endPos = Position(k + northBorder, j + westBorder);
							if (endPos == getDockingPos())
								setDistanceToDock(round);

							destinations.push_back(endPos);
						}

						if (matrix[k][j] == SPACE || matrix[k][j] == UNDISCOVERD_REACHABLE || matrix[k][j] == DOCK)
							matrix[k][j] = thisRoundChar;
					}

					for (int k = j - 1; k <= j + 1; k += 2)
					{
						if (k < 0 || k == getWidth()) continue;

						if (matrix[i][k] == TARGET)
						{
							endPos = Position(i + northBorder, k + westBorder);
							if (endPos == getDockingPos())
								setDistanceToDock(round);

							destinations.push_back(endPos);
						}

						if (matrix[i][k] == SPACE || matrix[i][k] == UNDISCOVERD_REACHABLE || matrix[i][k] == DOCK)
							matrix[i][k] = thisRoundChar;
					}
				}
			}
		}
	}
	endPos = destinations.at(seed % destinations.size());
	Direction step = GetFirstStepFromPath(matrix, endPos.getRow() - northBorder, endPos.getCol() - westBorder, thisRoundChar);
	DeleteMatrix(matrix);
	return Path(endPos, round, step);
}

Direction HouseMap::FindClosestUndiscoveredReachableSquareOnBorderNextStep(Position startPos, int seed)
{
	unsigned char** matrix = ClosestUndiscoveredReachableSquareOnBorderMatrix(startPos);
	return FindPathToTarget(matrix, seed).getNextStep();
}

Direction HouseMap::FindClosestUndiscoveredReachableSquareNextStep(Position startPos, int seed)
{
	unsigned char** matrix = ClosestUndiscoveredReachableSquareMatrix(startPos);
	return FindPathToTarget(matrix, seed).getNextStep();
}

Direction HouseMap::FindClosestDirtySquareNextStep(Position startPos, int seed)
{
	if (IsDirtySquare(startPos)) return Direction::Stay;

	unsigned char** matrix = DirtySquaresMatrix(startPos);
	return FindPathToTarget(matrix, seed).getNextStep();
}

Direction HouseMap::ShortestDistanceNextStep(Position startPos, Position endPos, int seed)
{
	if (startPos == endPos) return Direction::Stay;

	unsigned char** matrix = FindShortestDistanceMatrix(startPos, endPos);
	return FindPathToTarget(matrix, seed).getNextStep();
}

int HouseMap::FindShortestDistance(Position startPos, Position endPos)
{
	if (startPos == endPos) return 0;

	unsigned char** matrix = FindShortestDistanceMatrix(startPos, endPos);
	return FindPathToTarget(matrix, 0).getDistance();
}

Direction HouseMap::GetFirstStepFromPath(unsigned char** matrix, int row, int col, unsigned char lastRoundChar) const
{
	unsigned char roundChar = lastRoundChar == 128 ? 255 : lastRoundChar - 1;

	while (true)
	{
		for (int k = row - 1; k <= row + 1; k += 2)
		{
			if (k < 0 || k == getHeight()) continue;

			if (matrix[k][col] == ROBOT)
				return k < row ? Direction::South : Direction::North;

			if (matrix[k][col] == roundChar)
			{
				row = k;
				roundChar = roundChar == 128 ? 255 : roundChar - 1;
			}
		}

		for (int k = col - 1; k <= col + 1; k += 2)
		{
			if (k < 0 || k == getWidth()) continue;

			if (matrix[row][k] == ROBOT)
				return k < col ? Direction::East : Direction::West;

			if (matrix[row][k] == roundChar)
			{
				col = k;
				roundChar = roundChar == 128 ? 255 : roundChar - 1;
			}
		}
	}
}

unsigned char** HouseMap::ClosestUndiscoveredReachableSquareOnBorderMatrix(Position startPos) const
{
	unsigned char** matrix = CreateDistanceMatrix();

	for (auto i = 0; i < getHeight(); i++)
		for (auto j = 0; j < getWidth(); j++)
		{
			if (matrix[i][j] == UNDISCOVERD_UNREACHABLE)
			{
				if (IsSquareOnBorder(i, j)) matrix[i][j] = TARGET;
				else matrix[i][j] = WALL;
			}

			if (matrix[i][j] == UNDISCOVERD_REACHABLE && IsSquareOnBorder(i, j))
				matrix[i][j] = TARGET;
		}

	matrix[startPos.getRow() - northBorder][startPos.getCol() - westBorder] = ROBOT;
	return matrix;
}

unsigned char** HouseMap::ClosestUndiscoveredReachableSquareMatrix(Position startPos) const
{
	unsigned char** matrix = CreateDistanceMatrix();

	for (auto i = 0; i < getHeight(); i++)
		for (auto j = 0; j < getWidth(); j++)
		{
			if (matrix[i][j] == UNDISCOVERD_UNREACHABLE) matrix[i][j] = WALL;
			if (matrix[i][j] == UNDISCOVERD_REACHABLE) matrix[i][j] = TARGET;
		}

	matrix[startPos.getRow() - northBorder][startPos.getCol() - westBorder] = ROBOT;
	return matrix;
}

unsigned char** HouseMap::DirtySquaresMatrix(Position startPos) const
{
	unsigned char** matrix = CreateDistanceMatrix();

	for (auto i = 0; i < getHeight(); i++)
		for (auto j = 0; j < getWidth(); j++)
		{
			if (matrix[i][j] == UNDISCOVERD_UNREACHABLE) matrix[i][j] = WALL;
			if (IsDirtySquare(Position(i + northBorder, j + westBorder))) matrix[i][j] = TARGET;
		}

	matrix[startPos.getRow() - northBorder][startPos.getCol() - westBorder] = ROBOT;
	return matrix;
}

unsigned char** HouseMap::FindShortestDistanceMatrix(Position startPos, Position endPos) const
{
	unsigned char** matrix = CreateDistanceMatrix();

	for (auto i = 0; i < getHeight(); i++)
		for (auto j = 0; j < getWidth(); j++)
			if (matrix[i][j] == UNDISCOVERD_UNREACHABLE) matrix[i][j] = WALL;

	matrix[startPos.getRow() - northBorder][startPos.getCol() - westBorder] = ROBOT;
	matrix[endPos.getRow() - northBorder][endPos.getCol() - westBorder] = TARGET;
	return matrix;
}

unsigned char** HouseMap::BordersMatrix() const
{
	unsigned char** matrix = CreateDistanceMatrix();

	for (int i = 0; i < getHeight(); i++)
	{
		for (int j = 0; j < getWidth(); j++)
		{
			if (IsSquareOnBorder(i, j))
			{
				if (matrix[i][j] == UNDISCOVERD_UNREACHABLE) matrix[i][j] = ROBOT;
			}

			else
			{
				if (matrix[i][j] == UNDISCOVERD_UNREACHABLE) matrix[i][j] = SPACE;
				else if (matrix[i][j] != WALL) matrix[i][j] = TARGET;
			}
		}
	}
	return matrix;
}

unsigned char** HouseMap::CreateDistanceMatrix() const
{
	unsigned char** matrix = new unsigned char*[getHeight()];

	for (int i = 0; i < getHeight(); i++)
	{
		matrix[i] = new unsigned char[getWidth()];
		for (int j = 0; j < getWidth(); j++)
		{
			matrix[i][j] = operator[](Position(i + northBorder, j + westBorder));
			matrix[i][j] = IsDirtySquare(Position(i + northBorder, j + westBorder)) ? SPACE : matrix[i][j];
		}
	}
	return matrix;
}

void HouseMap::CreateMatrix()
{
	houseMatrix = new unsigned char*[getHeight()];

	for (int i = 0; i < getHeight(); i++)
	{
		houseMatrix[i] = new unsigned char[getWidth()];

		for (int j = 0; j < getWidth(); j++)
		{
			if (i == 0 || i == getHeight()) houseMatrix[i][j] = WALL;
			if (j == 0 || j == getWidth()) houseMatrix[i][j] = WALL;

			houseMatrix[i][j] = operator[](Position(i + northBorder, j + westBorder));
			if (houseMatrix[i][j] == DOCK)
				matrixDockPos = Position(i, j);
		}
	}
	eastBorder -= westBorder;
	southBorder -= northBorder;
	westBorder = northBorder = 0;
}

void HouseMap::DeleteMatrix(unsigned char** matrix) const
{
	for (int i = 0; i < getHeight(); i++)
		delete[] matrix[i];

	delete[] matrix;
}

bool HouseMap::IsHouseCompleted()
{
	for (auto i = 0; i < getHeight(); i++)
		for (auto j = 0; j < getWidth(); j++) 
		{
			if (operator[](Position(i + northBorder, j + westBorder)) == UNDISCOVERD_REACHABLE)
				return false;
		}
	RemoveAllUndiscoverableSuqares();
	return true;
}

bool HouseMap::IsAllHouseRevelead()
{
	if (view == HouseView::Map) return false;
	if (!isHouseFullyMapped) isHouseFullyMapped = IsHouseCompleted();
	return isHouseFullyMapped;
}

bool HouseMap::IsBordersDiscovered() const
{
	unsigned char** matrix = BordersMatrix();

	for (auto i = 0; i < getHeight(); i++)
	{
		if (matrix[i][0] == UNDISCOVERD_REACHABLE || matrix[i][getWidth() - 1] == UNDISCOVERD_REACHABLE)
		{
			DeleteMatrix(matrix);
			return false;
		}

		if (matrix[i][0] == ROBOT && IsBorderReachable(matrix, i, 0))
		{
			DeleteMatrix(matrix);
			return false;
		}

		if (matrix[i][getWidth() - 1] == ROBOT && IsBorderReachable(matrix, i, getWidth() - 1))
		{
			DeleteMatrix(matrix);
			return false;
		}
	}

	for (auto j = 0; j < getWidth(); j++)
	{
		if (matrix[0][j] == UNDISCOVERD_REACHABLE || matrix[getHeight() - 1][j] == UNDISCOVERD_REACHABLE)
		{
			DeleteMatrix(matrix);
			return false;
		}

		if (matrix[0][j] == ROBOT && IsBorderReachable(matrix, 0, j))
		{
			DeleteMatrix(matrix);
			return false;
		}

		if (matrix[getHeight() - 1][j] == ROBOT && IsBorderReachable(matrix, getHeight() - 1, j))
		{
			DeleteMatrix(matrix);
			return false;
		}
	}

	DeleteMatrix(matrix);
	return true;
}

bool HouseMap::IsBorderReachable(unsigned char** matrix, int i, int j) const
{
	for (int k = i - 1; k <= i + 1; k += 2)
	{
		if (k < 0 || k == getHeight()) continue;

		if (matrix[k][j] == TARGET) return true;
	}

	for (int k = j - 1; k <= j + 1; k += 2)
	{
		if (k < 0 || k == getWidth()) continue;

		if (matrix[i][k] == TARGET) return true;
	}
	return false;
}
