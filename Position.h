#ifndef __POSITION__H_
#define __POSITION__H_

#include <iostream>
#include "Direction.h"

using namespace std;

class Position {
	int row, col;

public:
	Position() { Position(-1, -1); }

	Position(int x, int y) : row(x), col(y) {}

	Position(const Position& position) { this->row = position.getRow(); this->col = position.getCol(); }
	
	Position& operator=(const Position& position);

	bool operator==(const Position& position) const { return this->row == position.getRow() && this->col == position.getCol(); }

	Position operator+(const Direction& dir) const {
		Position pos(*this);
		pos.Move(dir);
		return pos;
	}

	Position& operator+(const Position& position) {
		this->row = this->row + position.getRow();
		this->col = this->col + position.getCol();
		return *this;
	}

	int getRow() const { return row; }

	int getCol() const { return col; }

	void Print(ostream& out = cout) const { out << "(" << row << "," << col << ")"; }

	void Move(Direction dir);

	friend ostream& operator<<(ostream& out, const Position& p) { p.Print(out); return out; }

	friend bool operator<(const Position& p1, const Position& p2) {
		if (p1.getRow() == p2.getRow()) return p1.getCol() < p2.getCol();
		return p1.getRow() < p2.getRow();
	}

};

#endif //__POSITION__H_