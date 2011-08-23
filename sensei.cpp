#include <vector>
#include <set>
#include <iostream>
#include <algorithm>

// #include <boost/thread.hpp>
// #include <boost/foreach.hpp>

#include "sensei.hpp"

std::vector<std::vector<Position> > units;
std::vector<std::set<Position> > peers;

int main() {
	init();
	return 0;
}

void init(void) {
	// Fill vector 'units': rows
	for (Position row = 0; row <= 81; row += 9) {
		// 'row': position of first cell in row
		std::vector<Position> row_pos = {
			(Position)row, (Position)(row+1), (Position)(row+2),
			(Position)(row+3), (Position)(row+4), (Position)(row+5),
			(Position)(row+6), (Position)(row+7), (Position)(row+8)};

		units.push_back(row_pos);
	}
	
	// Fill vector 'units': columns
	for (Position col = 0; col != 9; ++col) {
		// 'col' is the column index
		std::vector<Position> col_pos = {
			(Position)col, (Position)(col+9), (Position)(col+18),
			(Position)(col+27), (Position)(col+36), (Position)(col+45),
			(Position)(col+54), (Position)(col+63), (Position)(col+72)};

		units.push_back(col_pos);
	}

	// Fill vector 'units': 3x3 squares
	for (Position sq = 0; sq != 9; ++sq) {
		// Position of first (top left) cell in square
		Position sq_first = 18 * (Position)(sq / 3) + sq * 3;
		std::vector<Position> sq_pos = {
			(Position)(sq_first), (Position)(sq_first+1), (Position)(sq_first+2),
			(Position)(sq_first+9), (Position)(sq_first+10), (Position)(sq_first+11),
			(Position)(sq_first+18), (Position)(sq_first+19), (Position)(sq_first+20)};

		// units.insert(std::set<Position>(sq_pos.begin(), sq_pos.end()));
		units.push_back(sq_pos);
	}
	
	// Fill vector 'peers'
	for (Position pos = 0; pos != 81; ++pos) {
		// Go through all cell positions
		std::set<Position> peer_set;
		for (
				std::vector<std::vector<Position> >::const_iterator it2 = units.begin();
				it2 != units.end(); ++it2) {

			if (find(it2->begin(), it2->end(), pos) != it2->end()) {
				peer_set.insert(it2->begin(), it2->end());
			}
		}
		peer_set.erase(pos); // pos is not a peer of pos
		peers.push_back(peer_set);
	}
}

bool solved(Sudoku sudoku) {
	for (
			Sudoku::const_iterator it = sudoku.begin();
			it != sudoku.end(); ++it) {

		Values count; // c accumulates the total bits set in v
		Values value = (*it);
		
		for (count = 0; value; value >>= 1) {
			count += value & 1;
		}
		if (count != 1)
			return false;
	}
	return true;
}

// :vi: set tabstop=4 softtabstop=4 shiftwidth=4 smarttab noexpandtab :
