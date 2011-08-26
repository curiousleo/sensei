// Compile with: g++ -std=c++0x -O2 -lboost_thread-mt -o sensei sensei.cpp

#include <algorithm>
#include <array>
#include <iostream>
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
	static const tiny thread_number = boost::thread::hardware_concurrency();
	std::vector<boost::thread> threads;

	// Initialize 'units' and 'peers' vectors
	init();

	// Create and start worker threads
	for (tiny thread_i = 0; thread_i != thread_number; ++thread_i) {
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
	static const Values defaults = 
			{true, true, true, true, true, true, true, true, true};
	sudoku.fill(defaults);

	std::string::const_iterator cell_it;
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
	tiny cell_i, width, max_width = 0;
	Sudoku::const_iterator cell_it;
	Values::const_iterator val_it;

	// Find padding width
	for (cell_it = sudoku.begin(); cell_it != sudoku.end(); ++cell_it) {
		width = count(*cell_it);
		if (width > max_width)
			max_width = width;
	}
	width = max_width + 1;

	// Print Sudoku
	const std::string line_sep = "\n" +
		std::string(width*3+1, '-') + "+" +
		std::string(width*3+1, '-') + "+" +
		std::string(width*3, '-') + "\n";

	for (cell_it = sudoku.begin(); cell_it != sudoku.end(); ++cell_it) {
		line = "";
		for (val_it = cell_it->begin(); val_it != cell_it->end(); ++val_it) {
			if (*val_it)
				line += std::string(1, (char)(val_it - cell_it->begin() + '1'));
		}
		sep = "";
		cell_i = cell_it - sudoku.begin();
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
	tiny min_len = 10, len, cell_i, min_cell_i, guess_i;

	for (cell_i = 0; cell_i != 81; ++cell_i) {
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
	for (guess_i = 0; guess_i != 9; ++guess_i) {
		if (!sudoku[min_cell_i][guess_i])
			continue;

		Sudoku guess_sudoku = sudoku;

		try {
			assign(guess_sudoku, min_cell_i, guess_i + 1);
			eliminate(guess_sudoku);
		} catch (std::exception) { continue; }

		if (solve(guess_sudoku)) {
			sudoku = guess_sudoku;
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
		sudoku[peers[cell_i][peer_i]][val_i] = false;
		if (!count(sudoku[peers[cell_i][peer_i]]))
			throw std::logic_error("Peer has no possibilities left");
	}
}

void eliminate(Sudoku& sudoku) {
	/* If a unit has only one possible place for a value, then put the
	 * value there. */
	bool changed, found;
	tiny found_cell[2];		// Pair of (cell index, value)

	do {
		changed = false;

		// Loop through units
		for (tiny unit_i = 0; unit_i != 27; ++unit_i) {

			// Loop through values
			for (tiny val_i = 0; val_i != 9; ++val_i) {
				found = false;

				// Loop through cells
				for (tiny cell_i = 0; cell_i != 9; ++cell_i) {

					if (sudoku[cell_i][val_i]) {
						if (!found) {
							// First occurrence of this value
							found_cell[0] = cell_i;
							found_cell[1] = val_i + 1;
							found = true;
						}
						else {
							// Value has been found before -> not unique
							found = false;
							break;
						}
					}
				} // End loop through cells

				if (found && count(sudoku[found_cell[0]]) != 1) {
					assign(sudoku, found_cell[0], found_cell[1]);
					changed = true;
				}
			} // End loop through values
		} // End loop through units
	} while (changed);
}

void init(void) {
	tiny row, sq_first, j;

	// Fill vector 'units'
	for (tiny i = 0; i != 9; ++i) {
		// 'row': position of first cell in row
		row = 9*i;
		for (j = 0; j != 9; ++j) {
			units[i][j] = row + j;
		}
	
		// 'i' is the column index
		for (j = 0; j != 9; ++j) {
			units[i+9][j] = i + j*9;
		}

		// 'sq_first': position of first (top left) cell in square
		sq_first = 18 * (tiny)(i / 3) + i * 3;
		for (j = 0; j != 9; ++j) {
			units[i+18][j] = sq_first + (j/3)*9 + (j%3);
		}
	}
	
	// Fill vector 'peers'
	// std::array<std::array<tiny, 20>, 81>::const_iterator cell_it;
	std::array<tiny, 20>::const_iterator peer_it;
	std::array<std::array<tiny, 9>, 27>::const_iterator unit_it;
	std::array<tiny, 9>::const_iterator unit_cell_it;

	tiny cell_i, peer_count;

	// Loop through cells
	for (cell_i = 0; cell_i != 81; ++cell_i) {
		peer_count = 0;

		// Loop through units
		for (unit_it = units.begin(); unit_it != units.end(); ++unit_it) {

			if (
					std::find(unit_it->begin(), unit_it->end(), cell_i)
							!= unit_it->end()) {
				// If cell is contained in a certain unit, add each cell in that
				//unit as a peer if it is not yet in the list of peers
				
				// Loop through through cells in unit
				for (
						unit_cell_it = unit_it->begin();
						unit_cell_it != unit_it->end(); ++unit_cell_it) {
					
					// If it is not yet in the list of peers and also not the
					// cell itself, add it
					if (
							std::find(
									peers[cell_i].data(), peers[cell_i].data() +
										peer_count, *unit_cell_it)
										== peers[cell_i].data() + peer_count
									&& *unit_cell_it != cell_i) {
						peers[cell_i][peer_count] = *unit_cell_it;
						++peer_count;
					}
				} // End loop throug cells in unit
			}
		} // End loop through units
	} // End loop through cells
}

bool solved(const Sudoku& sudoku) {
	// Check if all cells have length 1; no validity check
	for (tiny cell_i = 0; cell_i != 81; ++cell_i) {
		if (count(sudoku[cell_i]) != 1)
			return false;
	}

	return true;
}

tiny count(const Values& values) {
	tiny c = 0;
	for (tiny val_i = 0; val_i != 9; ++val_i) {
		if (values[val_i]) c++;
	}

	return c;
}

// vi: set tabstop=4 softtabstop=4 shiftwidth=4 smarttab noexpandtab:
