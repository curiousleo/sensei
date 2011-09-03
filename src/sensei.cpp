#include <algorithm>
#include <array>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/thread.hpp>

#include "cover.hpp"
#include "sensei.hpp"

Sudoku default_sudoku;

boost::mutex cin_mutex;
boost::shared_mutex cout_mutex;

int main() {
	static const int thread_number = boost::thread::hardware_concurrency();
	std::vector<boost::thread> threads;

	// Initialize 'sudoku_matrix'
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
	Sudoku sudoku(default_sudoku);
	Solution solution;
	std::string s_str;

	cin_mutex.lock();
	while (std::cin >> s_str) {
		cin_mutex.unlock();

		read(sudoku, s_str);
		solution = solve(sudoku);

		cout_mutex.lock();
		display(solution);
		cout_mutex.unlock();

		cin_mutex.lock();
	}
	cin_mutex.unlock();
}

void read(Sudoku& sudoku, const std::string s_str) {
	if (s_str.size() < 81)
		throw std::range_error("Sudoku string too short");

	int val;

	try {
		for (int cell_i = 0; cell_i != 81; ++cell_i) {
			val = s_str[cell_i];
			if (val == '.' || val == '0')
				// '.' or '0' designates cell with unknown value
				continue;
			else
				assign(sudoku, cell_i, (int)(val - '1'));
		}
	} catch (std::exception e) {
		// Catch out of bounds (end of string reached)
		// or logic error (from assignment)
		std::cerr << "Error reading Sudoku: " << e.what() << std::endl;
	}
}

void assign(Sudoku& sudoku, const int cell_i, const int val_i) {
	// Erase corresponding 'symbol@position' row
	Sudoku::iterator row_it = sudoku.begin();
	advance(row_it, ROW(cell_i, val_i));
	sudoku.erase(row_it);

	// Erase all condition columns whose conditions are fulfilled
	for (row_it = sudoku.begin(); row_it != sudoku.end(); ++row_it) {
		// Location condition
		row_it->remove(cell_i);
		// Row condition
		row_it->remove(ROW_COND(cell_i, val_i));
		// Column condition
		row_it->remove(COLUMN_COND(cell_i, val_i));
		// Box condition
		row_it->remove(BOX_COND(cell_i, val_i));
	}
}

void display(Solution& solution) {
	std::string sep, line;
	int cell = 0, width, max_width = 0;

	sort(solution.begin(), solution.end());

	// Print Solution
	const std::string line_sep = "\n" +
		std::string(4, '-') + "+" +
		std::string(4, '-') + "+" +
		std::string(3, '-') + "\n";

	for (
			Solution::const_iterator tag_it = solution.begin();
			tag_it != solution.end(); ++tag_it) {
		line = std::string(1, (*tag_it) % 9 + 1);
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

Solution solve(const Sudoku& sudoku) {
	ExactCover cover(&sudoku);
	cover.search();
	return cover.solution;
}

void init(void) {
	;
}

// vi: set tabstop=4 softtabstop=4 shiftwidth=4 smarttab noexpandtab:
