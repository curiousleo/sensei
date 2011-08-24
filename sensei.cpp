#include <algorithm>
#include <iomanip>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

// #include <boost/foreach.hpp>
// #include <boost/thread.hpp>

#include "sensei.hpp"

std::vector<std::vector<Position> > units;
std::vector<std::set<Position> > peers;

int main() {
	Sudoku sudoku;
	std::string s_str;
	unsigned int i = 0;

	init();

	// Read input from pipe or terminal
	while (std::cin >> s_str) {
		// std::cout << ++i;
		sudoku = read(s_str);
		eliminate(sudoku);
		solve(sudoku);
		// std::cout << std::endl;
	}
	// std::cout << std::endl;
	return 0;
}

Sudoku read(const std::string s_str) {
	if (s_str.size() < 81)
		throw std::range_error("Sudoku string too short");

	// Populate new Sudoku with default values
	static const Value defaults[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	Sudoku sudoku = Sudoku(81, Values(defaults, defaults+9));

	Value val;

	try {
		for (unsigned char i = 0; i != 81; ++i) {
			val = s_str[i];
			if (val == '.' || val == '0')
				// '.' or '0' designates cell with unknown value
				continue;
			else
				assign(sudoku, i, (Value)(val - '0'));
		}
	} catch (std::exception e) {
		// Catch out of bounds (end of string reached)
		// or logic error (from assignment)
		std::cerr << "Error reading Sudoku: " << e.what() << std::endl;
	}

	return sudoku;
}

void display(const Sudoku sudoku) {
	std::vector<unsigned char> lengths;
	std::string sep, line;
	unsigned char cell;

	// Find padding width
	for (
			Sudoku::const_iterator cell_it = sudoku.begin();
			cell_it != sudoku.end(); ++cell_it)
		lengths.push_back(cell_it->size());
	const unsigned char width = (*max_element(lengths.begin(), lengths.end())) + 1;

	// Print Sudoku
	const std::string line_sep = "\n" +
		std::string(width*3+1, '-') + "+" +
		std::string(width*3+1, '-') + "+" +
		std::string(width*3, '-') + "\n";

	cell = 0;
	for (
			Sudoku::const_iterator cell_it = sudoku.begin();
			cell_it != sudoku.end(); ++cell_it) {
		line = "";
		for (
				Values::const_iterator value_it = cell_it->begin();
				value_it != cell_it->end(); ++value_it) {
			line += std::string(1, (*value_it) + '0');
		}
		sep = "";
		if (cell % 3 == 2)
			sep = " |";
		if (cell % 9 == 8)
			sep = "\n";
		if (cell % 27 == 26)
			sep = line_sep;
		if (cell == 80)
			sep = "\n\n";
		std::cout << std::setw(width) << line << sep;
		++cell;
	}
}

bool solve(Sudoku& sudoku) {
	// Check if Sudoku is already solved
	if (solved(sudoku))
		return true;

	// Find cell with minimum possibilities > 1 so we can take a guess
	unsigned char min_len = 10, len;
	Position min_cell;

	for (Position pos = 0; pos != 81; ++pos) {
		len = sudoku[pos].size();
		// Minimum length > 1 for cell is 2
		if (len == 2) {
			min_cell = pos;
			break;
		}
		if (len < min_len && len > 1) {
			min_cell = pos;
			min_len = len;
		}
	}

	// Guess values for remaining possibilities
	for (
			Values::const_iterator guess_it = sudoku[min_cell].begin();
			guess_it != sudoku[min_cell].end(); ++guess_it) {
		// std::cout << ".";
		Sudoku guess_sudoku = Sudoku(sudoku);
		try {
			assign(guess_sudoku, min_cell, (*guess_it));
			eliminate(guess_sudoku);
		} catch (std::exception e) {
			// std::cerr << e.what();
			continue;
		}
		if (solve(guess_sudoku)) {
			sudoku = guess_sudoku;
			return true;
		}
	}

	return false;
}

void assign(Sudoku& sudoku, Position pos, Value val) {
    /* If a cell has only one possible value, then eliminate that value
     * from the cell's peers.
	 */
	if (find(sudoku[pos].begin(), sudoku[pos].end(), val) == sudoku[pos].end())
		throw std::range_error("Assignation not possible: value not in set of possible values");
	if (val > 9)
		throw std::out_of_range("Value is not valid");

	sudoku[pos] = {val};

	for (
			std::set<Position>::const_iterator it = peers[pos].begin();
			it != peers[pos].end(); ++it) {
		sudoku[*it].erase(val);
		if (sudoku[*it].size() == 0)
			throw std::logic_error("Peer has no possibilities left");
	}
}

void eliminate(Sudoku& sudoku) {
    /* If a unit has only one possible place for a value, then put the
     * value there.
	 */
	bool changed;

	do {
		changed = false;

		for (
				std::vector<std::vector<Position> >::const_iterator unit_it = units.begin();
				unit_it != units.end(); ++unit_it) {

			for (unsigned char val = 1; val != 10; ++val) {
				std::vector<std::pair<Position, Value> > cells;

				for (
						std::vector<Position>::const_iterator cell_it = unit_it->begin();
						cell_it != unit_it->end(); ++cell_it) {

					Values* cell = &(sudoku[*cell_it]);

					if (find(cell->begin(), cell->end(), val) != cell->end())
						cells.push_back(std::pair<Position, Value>(*cell_it, val));
				}

				if (cells.size() == 1 && sudoku[cells.begin()->first].size() != 1) {
					assign(sudoku, cells.begin()->first, cells.begin()->second);
					changed = true;
				}
			}
		}
	} while (changed);
}

void init(void) {
	Position row, sq_first;

	// Fill vector 'units'
	for (Position i = 0; i != 9; ++i) {
		// 'row': position of first cell in row
		row = 9*i;
		std::vector<Position> row_unit = {
			(Position)row, (Position)(row+1), (Position)(row+2),
			(Position)(row+3), (Position)(row+4), (Position)(row+5),
			(Position)(row+6), (Position)(row+7), (Position)(row+8)};
		units.push_back(row_unit);
	
		// 'i' is the column index
		std::vector<Position> col_unit = {
			(Position)i, (Position)(i+9), (Position)(i+18),
			(Position)(i+27), (Position)(i+36), (Position)(i+45),
			(Position)(i+54), (Position)(i+63), (Position)(i+72)};
		units.push_back(col_unit);

		// Position of first (top left) cell in square
		sq_first = 18 * (Position)(i / 3) + i * 3;
		std::vector<Position> sq_unit = {
			(Position)(sq_first), (Position)(sq_first+1), (Position)(sq_first+2),
			(Position)(sq_first+9), (Position)(sq_first+10), (Position)(sq_first+11),
			(Position)(sq_first+18), (Position)(sq_first+19), (Position)(sq_first+20)};
		units.push_back(sq_unit);
	}
	
	// Fill vector 'peers'
	for (Position pos = 0; pos != 81; ++pos) {
		// Go through all cell positions
		std::set<Position> peer_set;

		for (
				std::vector<std::vector<Position> >::const_iterator unit_it = units.begin();
				unit_it != units.end(); ++unit_it) {

			if (find(unit_it->begin(), unit_it->end(), pos) != unit_it->end()) {
				peer_set.insert(unit_it->begin(), unit_it->end());
			}
		}
		peer_set.erase(pos); // pos is not a peer of itself
		peers.push_back(peer_set);
	}
}

bool solved(const Sudoku& sudoku) {
	// Check if all cells have length 1
	for (
			Sudoku::const_iterator cell_it = sudoku.begin();
			cell_it != sudoku.end(); ++cell_it) {

		if (cell_it->size() > 1)
			return false;
	}

	return true;
}

// :vi: set tabstop=4 softtabstop=4 shiftwidth=4 smarttab noexpandtab :
