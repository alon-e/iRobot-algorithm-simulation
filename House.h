#ifndef __HOUSE__H_
#define __HOUSE__H_

#include "Position.h"

class House
{
	string filename;
	string name;
	int maxSteps;
	int rows;
	int columns;
	string* matrix;
	int initialDirt;
	Position Dock;
	string status;
	bool isValid;

public:

	explicit House(string path);

	House() : matrix(nullptr), Dock(-1,-1) {};

	House(const House& house) { this->matrix = nullptr; operator=(house); }

	House& operator=(const House& house);
	void Print(Position pos) const;

	~House() { if (matrix != nullptr) delete[] matrix; matrix = nullptr; }

	void Print() const;
	
	void createDirectoryIfNotExists(const string& dirPath) const;

	void PrintVideo(int steps,const string& algorithmName, Position pos,Direction prevStep) const;

	string getFilename() const { return filename; }

	string getFilenameNoSuffix() const { return filename.substr(0, filename.find_last_not_of("house")); }

	string getName() const { return name; }

	string getHouseStatus() const { return status; }

	int getInitalDirt() const { return initialDirt; }

	Position getDockPosition() const { return Dock; }

	int getMaxSteps() const { return maxSteps; }

	char getPositionValue(Position pos) const { return matrix[pos.getRow()][pos.getCol()]; }

	void CleanDirt(Position pos);

	bool isDirtyCell(char c) const { return c >= '1' && c <= '9'; }

	bool isValidHouse() const { return isValid; }

private:
	void SetInitials();

	void DecreaseDirtInPosition(int i, int j);

	void ExtendRow(string* row) const { for (int i = row->length(); i < columns; i++) row->append(" "); }

	void SurroundByWalls();
};

#endif //__HOUSE__H_