#include <algorithm>
#include <array>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/thread.hpp>

#include "sensei.hpp"
#include "cover.hpp"

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
	static const std::set<tiny> defaults = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	sudoku.fill(defaults);

	tiny val;

	try {
		for (tiny cell_i = 0; cell_i != 81; ++cell_i) {
			val = s_str[cell_i];
			if (val == '.' || val == '0')
				// '.' or '0' designates cell with unknown value
				continue;
			else
				assign(sudoku, cell_i, (tiny)(val - '0'));
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
	for (
			Sudoku::const_iterator cell_it = sudoku.begin();
			cell_it != sudoku.end(); ++cell_it) {
		width = cell_it->size();
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
	;
}

void init(void) {
	;
}

// vi: set tabstop=4 softtabstop=4 shiftwidth=4 smarttab noexpandtab:
