# The Sensei Loves to Solve Sudokus

**Sensei** is basically a multithreaded C++ implementation of Peter
Norvig's excellent [Sudoku solver](http://norvig.com/sudoku.html "Peter 
Norvig's Sudoku solver explained") (originally written in Python).

On my laptop, Sensei solves up to 1400 hard (17 clues) Sudokus per second.

## Compilation

The code makes use of some C++0x syntactic sugar as well as the
boost::thread library, so I recommend compiling it like this:

	sensei$ g++ -std=c++0x -lboost_thread-mt -O3 -o bin/sensei src/sensei.cpp

You need g++ version 4.4 and the boost::thread library (`libboost-thread-dev`
Debian package).

## Usage

Sensei reads Sudokus from the command line. The easiest way to use it
is by piping, or using the standard input.

Each Sudoku is represented as a flat list of cell values, 81 chars long,
as you would read them from the top left, row by row. Example:

	.94...13..............76..2.8..1.....32.........2...6.....5.4.......8..7..63.4..8

`.` or `0` stands for 'unknown value'.

## Performance

The `timefile.sh` script can be run as a benchmark: `./timefile.sh
../test/*`. I have included a list of Sudokus, `sudoku17.txt`; credits for
this file go to [Gordon
Royle](http://mapleta.maths.uwa.edu.au/~gordon/sudokumin.php "Gordon Royle's
'Minimum Sudoku' website") and the University of Western Australia.

## Optimization

`optimize.sh` goes one step further. It compiles Sensei using different
optimization flags, outputs the binary's file size and runs the above
benchmark. On my laptop, the output is as follows:

	sensei/bin$ ./optimize.sh 
	Optimization -O0
	112K    sensei
	../test/sudoku17.txt: 163.099 secs (3.318 ms/puzzle, 301 Hz)

	Optimization -O1
	52K     sensei
	../test/sudoku17.txt: 35.089 secs (.713 ms/puzzle, 1400 Hz)

	Optimization -O2
	52K     sensei
	../test/sudoku17.txt: 34.411 secs (.700 ms/puzzle, 1428 Hz)

	Optimization -O3
	52K     sensei
	../test/sudoku17.txt: 33.582 secs (.683 ms/puzzle, 1463 Hz)

--curiousleo
