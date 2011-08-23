#include <vector>
#include <iostream>
// #include <algorithm>

// #include <boost/thread.hpp>
// #include <boost/foreach.hpp>

#include "sensei.hpp"

PositionVector units;
PositionVector peers;

int main() {
	init();
	return 0;
}

void init(void) {
	// Fill vector 'units'
	// Fill vector 'peers'
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

// :vi: set tabstop=4 noexpandtab :
