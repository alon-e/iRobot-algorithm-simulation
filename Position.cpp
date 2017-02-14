#include <functional>
#include "Position.h"

Position& Position::operator=(const Position& position) {
	if (&position != this)
	{
		this->row = position.row;
		this->col = position.col;
	}
	return *this;
}

void Position::Move(Direction dir) {
	if (dir == Direction::Stay) return;

	const static function<void(Position&)> do_move[] = {
		[](Position&p) {++p.col;}, // East
		[](Position&p) {--p.col;}, // West
		[](Position&p) {++p.row;}, // South
		[](Position&p) {--p.row;}  // North
	};
	do_move[int(dir)](*this);
}
