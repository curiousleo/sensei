// Compile with: g++ -std=c++0x -O2 -lboost_thread-mt -o sensei sensei.cpp

#include <algorithm>
#include <array>
#include <iomanip>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/thread.hpp>

#include "sensei.hpp"

// 27 units, 9 cells each
std::array<std::array<tiny, 9>, 27> units;
// 81 cells, 20 peer cells each
std::array<std::array<tiny, 20>, 81> peers;

boost::mutex cin_mutex;
boost::shared_mutex cout_mutex;

int main() {
	static const int thread_number = boost::thread::hardware_concurrency();
	std::vector<boost::thread> threads;

	// Initialize 'units' and 'peers' vectors
	init();

	// Create and start worker threads
	for (int i = 0; i != thread_number; ++i) {
		threads.push_back(boost::thread(solve_worker));
	}

	// Join worker threads
	for (
			std::vector<boost::thread>::iterator thread_it = threads.begin();
			thread_it != threads.end(); ++thread_it) {
		thread_it->join();
	}

	return 0;
}

void solve_worker() {
	// Read input from pipe or terminal
	Sudoku sudoku;
	std::string s_str;

	cin_mutex.lock();
	while (std::cin >> s_str) {
		cin_mutex.unlock();

		read(sudoku, s_str);
		eliminate(sudoku);
		solve(sudoku);

		cout_mutex.lock();
		display(sudoku);
		cout_mutex.unlock();

		cin_mutex.lock();
	}
	cin_mutex.unlock();
}

void read(Sudoku& sudoku, const std::string s_str) {
	if (s_str.size() < 81)
		throw std::range_error("Sudoku string too short");

	// Populate new Sudoku with default values
	static const tiny defaults[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	sudoku = Sudoku(81, Values(defaults, defaults+9));

	tiny val;

	try {
		for (unsigned char i = 0; i != 81; ++i) {
			val = s_str[i];
			if (val == '.' || val == '0')
				// '.' or '0' designates cell with unknown value
				continue;
			else
				assign(sudoku, i, (tiny)(val - '0'));
		}
	} catch (std::exception e) {
		// Catch out of bounds (end of string reached)
		// or logic error (from assignment)
		std::cerr << "Error reading Sudoku: " << e.what() << std::endl;
	}
}

void display(const Sudoku& sudoku) {
	std::string sep, line;
	tiny cell = 0, width, max_width = 0;

	// Find padding width
	for (tiny cell_i = 0; cell_i != 81; ++cell_i) {
		width = count(sudoku[cell_i]);
		if (width > max_width)
			max_width = width;
	}
	width = max_width + 1;

	// Print Sudoku
	const std::string line_sep = "\n" +
		std::string(width*3+1, '-') + "+" +
		std::string(width*3+1, '-') + "+" +
		std::string(width*3, '-') + "\n";

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
	tiny min_cell;

	for (tiny pos = 0; pos != 81; ++pos) {
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
		Sudoku guess_sudoku(sudoku);

		try {
			assign(guess_sudoku, min_cell, (*guess_it));
			eliminate(guess_sudoku);
		} catch (std::exception) { continue; }

		if (solve(guess_sudoku)) {
			sudoku = guess_sudoku;
			return true;
		}
	}

	return false;
}

void assign(Sudoku& sudoku, tiny pos, tiny val) {
	/* If a cell has only one possible value, then eliminate that value
	 * from the cell's peers. */
	if (find(sudoku[pos].begin(), sudoku[pos].end(), val) == sudoku[pos].end())
		throw std::range_error("Assignation not possible: value not in set of possible values");
	if (val > 9)
		throw std::out_of_range("Value is not valid");

	sudoku[pos] = {val};

	for (
			std::set<tiny>::const_iterator it = peers[pos].begin();
			it != peers[pos].end(); ++it) {
		sudoku[*it].erase(val);
		if (sudoku[*it].size() == 0)
			throw std::logic_error("Peer has no possibilities left");
	}
}

void eliminate(Sudoku& sudoku) {
	/* If a unit has only one possible place for a value, then put the
	 * value there. */
	bool changed;
	Values* cell;

	do {
		changed = false;

		for (
				std::vector<std::vector<tiny> >::const_iterator unit_it = units.begin();
				unit_it != units.end(); ++unit_it) {

			for (unsigned char val = 1; val != 10; ++val) {
				std::vector<std::pair<tiny, tiny> > cells;

				for (
						std::vector<tiny>::const_iterator cell_it = unit_it->begin();
						cell_it != unit_it->end(); ++cell_it) {

					cell = &(sudoku[*cell_it]);

					if (find(cell->begin(), cell->end(), val) != cell->end())
						cells.push_back(std::pair<tiny, tiny>(*cell_it, val));
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
	tiny row, sq_first;

	// Fill vector 'units'
	for (tiny i = 0; i != 9; ++i) {
		// 'row': position of first cell in row
		row = 9*i;
		std::vector<tiny> row_unit = {
			(tiny)row, (tiny)(row+1), (tiny)(row+2),
			(tiny)(row+3), (tiny)(row+4), (tiny)(row+5),
			(tiny)(row+6), (tiny)(row+7), (tiny)(row+8)};
		units.push_back(row_unit);
	
		// 'i' is the column index
		std::vector<tiny> col_unit = {
			(tiny)i, (tiny)(i+9), (tiny)(i+18),
			(tiny)(i+27), (tiny)(i+36), (tiny)(i+45),
			(tiny)(i+54), (tiny)(i+63), (tiny)(i+72)};
		units.push_back(col_unit);

		// 'sq_first': position of first (top left) cell in square
		sq_first = 18 * (tiny)(i / 3) + i * 3;
		std::vector<tiny> sq_unit = {
			(tiny)(sq_first), (tiny)(sq_first+1), (tiny)(sq_first+2),
			(tiny)(sq_first+9), (tiny)(sq_first+10), (tiny)(sq_first+11),
			(tiny)(sq_first+18), (tiny)(sq_first+19), (tiny)(sq_first+20)};
		units.push_back(sq_unit);
	}
	
	// Fill vector 'peers'
	for (tiny pos = 0; pos != 81; ++pos) {
		std::set<tiny> peer_set;

		for (
				std::vector<std::vector<tiny> >::const_iterator unit_it = units.begin();
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
	// Check if all cells have length 1; no validity check
	for (
			Sudoku::const_iterator cell_it = sudoku.begin();
			cell_it != sudoku.end(); ++cell_it) {

		if (cell_it->size() > 1)
			return false;
	}

	return true;
}

// vi: set tabstop=4 softtabstop=4 shiftwidth=4 smarttab noexpandtab:
