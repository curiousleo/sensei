// Compile with: g++ -std=c++0x -O2 -lboost_thread-mt -o sensei sensei.cpp

#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/thread.hpp>

#include "sensei.hpp"

// 27 units, 9 cells each
tiny[27][9] units;
// 81 cells, 20 peer cells each
tiny[81][20] peers;

boost::mutex cin_mutex;
boost::shared_mutex cout_mutex;

int main() {
	static const tiny thread_number = boost::thread::hardware_concurrency();
	boost::thread threads[thread_number];

	// Initialize 'units' and 'peers' vectors
	init();

	// Create and start worker threads
	for (tiny thread_i = 0; thread_i != thread_number; ++thread_i) {
		threads[thread_i] = boost::thread(solve_worker);
	}

	// Join worker threads
	for (tiny thread_i = 0; thread_i != thread_number; ++thread_i) {
		threads[thread_i].join();
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
	for (tiny cell_i = 0; cell_i != 81; ++cell_i) {
		for (tiny val_i = 0; val_i != 9; ++val_i) {
			sudoku[cell_i][val_i] = true;
		}
	}

	Value val;

	try {
		for (tiny cell_i = 0; cell_i != 81; ++cell_i) {
			val = s_str[cell_i];
			if (val == '.' || val == '0')
				// '.' or '0' designates cell with unknown value
				continue;
			else
				assign(sudoku, cell_i, (Value)(val - '0'));
		}
	} catch (std::exception e) {
		// Catch out of bounds (end of string reached)
		// or logic error (from assignment)
		std::cerr << "Error reading Sudoku: " << e.what() << std::endl;
	}
}

void display(const Sudoku& sudoku) {
	std::string sep, line;
	tiny width = 0, max_width;

	// Find padding width
	for (tiny cell_i = 0; cell_i != 81; ++cell_i) {
		width = count(sudoku[cell_i]);
		if (width > max_width)
			max_width = width;
	}
	width = (max_width + 1) * 3;

	// Print Sudoku
	const std::string line_sep = "\n" +
		std::string(width+1, '-') + "+" +
		std::string(width+1, '-') + "+" +
		std::string(width, '-') + "\n";

	for (tiny cell_i = 0; cell_i != 81; ++cell_i) {
		line = "";
		for (tiny val_i = 0; val_i != 9; ++val_i) {
			if (sudoku[cell_i][val_i])
				line += std::string(1, val_i + '1');
		}
		sep = "";
		if (cell_i % 3 == 2)
			sep = " |";
		if (cell_i % 9 == 8)
			sep = "\n";
		if (cell_i % 27 == 26)
			sep = line_sep;
		if (cell_i == 80)
			sep = "\n\n";
		std::cout << std::setw(width) << line << sep;
	}
}

bool solve(Sudoku& sudoku) {
	// Check if Sudoku is already solved
	if (solved(sudoku))
		return true;

	// Find cell with minimum possibilities > 1 so we can take a guess
	tiny min_len = 10, len, min_cell_i;

	for (tiny cell_i = 0; cell_i != 81; ++cell_i) {
		len = count(sudoku[cell_i]);
		// Minimum length > 1 for cell is 2
		if (len == 2) {
			min_cell_i = cell_i;
			break;
		}
		if (len < min_len && len > 1) {
			min_cell_i = cell_i;
			min_len = len;
		}
	}

	// Guess values for remaining possibilities
	for (tiny guess_i = 0; guess_i != 9; ++guess_i) {
		if (!sudoku[min_cell_i][guess_i])
			continue;

		// Copy sudoku to guess_sudoku
		Sudoku guess_sudoku;
		for (tiny _cell_i = 0; _cell_i != 81; ++_cell_i) {
			for (tiny _val_i = 0; _val_i != 9; ++_val_i) {
				guess_sudoku[_cell_i][_val_i] = sudoku[_cell_i][_val_i];
			}
		}

		try {
			assign(guess_sudoku, min_cell_i, guess_i + 1);
			eliminate(guess_sudoku);
		} catch (std::exception) { continue; }

		if (solve(guess_sudoku)) {
			// Solution found; copy guess_sudoku back to sudoku
			for (tiny _cell_i = 0; _cell_i != 81; ++_cell_i) {
				for (tiny _val_i = 0; _val_i != 9; ++_val_i) {
					sudoku[_cell_i][_val_i] = guess_sudoku[_cell_i][_val_i];
				}
			}

			return true;
		}
	}

	return false;
}

void assign(Sudoku& sudoku, const tiny cell_i, const tiny val) {
	/* If a cell has only one possible value, then eliminate that value
	 * from the cell's peers. */
	const tiny val_i = val - 1;

	if (!sudoku[cell_i][val_i])
		throw std::range_error("Assignation not possible: value not in set of possible values");
	if (val_i > 9)
		throw std::out_of_range("Value is not valid");

	for (tiny _val_i = 0; _val_i != 9; ++_val_i) {
		if (_val_i == val_i)
			sudoku[cell_i][_val_i] = true;
		else
			sudoku[cell_i][_val_i] = false;
	}

	for (tiny peer_i = 0; peer_i != 20; ++peer_i) {
		sudoku[peers[peer_i]][val_i] = false;
		if (!count(sudoku[peers[peer_i]]))
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
				std::vector<std::vector<Position> >::const_iterator unit_it = units.begin();
				unit_it != units.end(); ++unit_it) {

			for (unsigned char val = 1; val != 10; ++val) {
				std::vector<std::pair<Position, Value> > cells;

				for (
						std::vector<Position>::const_iterator cell_it = unit_it->begin();
						cell_it != unit_it->end(); ++cell_it) {

					cell = &(sudoku[*cell_it]);

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

		// 'sq_first': position of first (top left) cell in square
		sq_first = 18 * (Position)(i / 3) + i * 3;
		std::vector<Position> sq_unit = {
			(Position)(sq_first), (Position)(sq_first+1), (Position)(sq_first+2),
			(Position)(sq_first+9), (Position)(sq_first+10), (Position)(sq_first+11),
			(Position)(sq_first+18), (Position)(sq_first+19), (Position)(sq_first+20)};
		units.push_back(sq_unit);
	}
	
	// Fill vector 'peers'
	for (Position pos = 0; pos != 81; ++pos) {
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
