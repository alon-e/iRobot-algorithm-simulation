#include <string>
#include <fstream>
#include <iostream>
#include "House.h"
#include "Constants.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Montage.h"

House::House(string path)
{
	isValid = false;
	matrix = nullptr;
	filename = path;

	ifstream fin(path);
	if (!fin.is_open())
	{
		status = "cannot open file";
		return;
	}
	string MaxSteps, Rows, Cols;

	getline(fin, name);
	getline(fin, MaxSteps);
	getline(fin, Rows);
	getline(fin, Cols);

	try { maxSteps = stoi(MaxSteps); }
	catch (...) 
	{
		status = "line number 2 in house file shall be a positive number, found: " + MaxSteps;
		return; 
	}
	try { rows = stoi(Rows); }
	catch (...)
	{
		status = "line number 3 in house file shall be a positive number, found: " + Rows;
		return;
	}
	try { columns = stoi(Cols); }
	catch (...)
	{
		status = "line number 4 in house file shall be a positive number, found: " + Cols;
		return;
	}

	if (maxSteps <= 0)
	{
		status = "line number 2 in house file shall be a positive number, found: " + MaxSteps;
		return;
	}
	if (rows <= 0) 
	{
		status = "line number 3 in house file shall be a positive number, found: " + Rows;
		return;
	}
	if (columns <= 0)
	{
		status = "line number 4 in house file shall be a positive number, found: " + Cols;
		return;
	}

	fin.ignore(); //skip newline and go the begining of matrix
	matrix = new string[rows];

	for (auto i = 0; i < rows; ++i)
	{
		getline(fin, matrix[i]);
		if (matrix[i].length() < unsigned(columns)) ExtendRow(&matrix[i]);	
	}
	SurroundByWalls();
	SetInitials();
	isValid = getHouseStatus().compare(VALID) == 0;
}
	

House& House::operator=(const House& house)
{
	if (&house == this) return *this;

	this->filename = house.filename;
	this->name = house.name;
	this->maxSteps = house.maxSteps;
	this->rows = house.rows;
	this->columns = house.columns;
	if (this->matrix != nullptr) delete[] this->matrix;
	this->matrix = new string[this->rows];
	if (house.matrix != nullptr) for (auto i = 0; i < rows; ++i) { this->matrix[i] = house.matrix[i]; }
	this->initialDirt = house.initialDirt;
	this->Dock = house.Dock;
	this->status = house.status;
	this->isValid = house.isValid;

	return *this;
}

void House::createDirectoryIfNotExists(const string& dirPath) const
{
  string cmd = "mkdir -p " + dirPath;
  int ret = system(cmd.c_str());
  if (ret == -1)
  {
  	//assuming the folder can be created - do nothing
  }
}

void House::PrintVideo(int steps,const string& algorithmName,Position pos,Direction prevStep) const
{
	//house name - house.name
	vector<string> tiles;
    for (auto row = 0; row < rows; ++row)
    {
      for (auto col = 0; col < columns; ++col)
      {
      	string tile = "";
      	
      	if (pos == Position(row,col)) {
      		//Extend robot images to show context
      		if (pos == Dock) {
      			tile = IMG_ROBOT_DOCKING;
      		} else if (isDirtyCell(getPositionValue(pos))) {
      			//cleaning - % 2 for variation in symbols
      			if  (getPositionValue(pos) % 2)
      				tile = IMG_ROBOT_CLEANING_EVEN;
      			else
      				tile = IMG_ROBOT_CLEANING_ODD;
      			
      		} else {
      			//orientation
      			switch (prevStep) {
      				case Direction::Stay:
      					tile = IMG_ROBOT;
      					break;
    				case Direction::North:
      					tile = IMG_ROBOT_NORTH;
      					break;
    				case Direction::South:
      					tile = IMG_ROBOT_SOUTH;
      					break;
    				case Direction::East:
      					tile = IMG_ROBOT_EAST;
      					break;
    				case Direction::West:
      					tile = IMG_ROBOT_WEST;
      					break;
      			}	
      		} 
      	} else {
	        if (matrix[row][col] == SPACE)
	          tile = IMG_SPACE;
	        else
	          tile = string() + matrix[row][col];
      	}
		
		tiles.push_back(tile);

      }
    }
    string imagesDirPath = "simulations/" + algorithmName + "_" + name.substr(0,name.find_last_not_of("\n"));
    createDirectoryIfNotExists(imagesDirPath);
    string stepsString = to_string(steps);
    //cout <<"img dir: "<< imagesDirPath << endl;
    string composedImage = imagesDirPath + "/image" + string(5-stepsString.length(), '0') + stepsString + ".jpg";
    //cout << "comp: " <<composedImage << endl;
    Montage::compose(tiles, columns, rows, composedImage);
        
	
}
void House::Print(Position pos) const
{
	char temp = matrix[pos.getRow()][pos.getCol()];
	matrix[pos.getRow()][pos.getCol()] = ROBOT;

	cout << endl;
	cout << "  ";
	for (auto j = 0; j < columns; ++j)
		cout << j % 10;
	cout << endl;
	for (auto i = 0; i < rows; ++i) {
		cout << i % 10 << " ";
		for (auto j = 0; j < columns; ++j) {
			cout << matrix[i][j];
		}
		cout << endl;
	}

	matrix[pos.getRow()][pos.getCol()] = temp;
}

void House::Print() const
{
	cout << "House name: " << name << endl;
	cout << "House max steps: " << maxSteps << endl;
	cout << "House rows: " << rows << endl;
	cout << "House columns: " << columns << endl;

	cout << endl;
	cout << "  ";
	for (auto j = 0; j < columns; ++j)
		cout << j % 10;
	cout << endl;
	for (auto i = 0; i < rows; ++i) {
		cout << i % 10 << " ";
		for (auto j = 0; j < columns; ++j) {
			cout << matrix[i][j];
		}
		cout << endl;
	}
}

void House::DecreaseDirtInPosition(int i, int j) 
{
	//Dirt is the chat '1' thru '9', so reducing it acts a reducing int.
	//if the cell was '1' then it should be ' '
	matrix[i][j]--;
	if (matrix[i][j] == '0') matrix[i][j] = SPACE;
}

void House::CleanDirt(Position pos)
{
	if (isDirtyCell(getPositionValue(pos)))
		DecreaseDirtInPosition(pos.getRow(), pos.getCol());
}

void House::SetInitials() {

	int numDockingStations = initialDirt = 0;

	for (auto i = 0; i < rows; ++i) {
		for (auto j = 0; j < columns; ++j) {
			if (matrix[i][j] == DOCK) {
				if (++numDockingStations > 1)
				{
					status = TOO_MANY_DOCKS;
					return;
				}
				Dock = Position(i, j);
			}

			else if (isDirtyCell(matrix[i][j])) {
				initialDirt += (int(matrix[i][j]) - '0');
			}
			else if (matrix[i][j] != WALL)
				matrix[i][j] = SPACE;
		}
	}
	if (numDockingStations == 1)
		status = VALID;
	else
		status = MISSING_DOCK;
}

void House::SurroundByWalls()
{
	for (auto i = 0; i < rows; ++i) {
		if (i == 0 || i == rows - 1) {
			for (auto j = 0; j < columns; ++j)
				matrix[i][j] = WALL;
		}
		else matrix[i][0] = matrix[i][columns - 1] = WALL;
		matrix[i][columns] = '\0';
	}
}