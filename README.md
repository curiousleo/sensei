# The Sensei Loves to Solve Sudokus

**Sensei** is basically a multithreaded C++ implementation of Peter
Norvig's excellent [Sudoku solver](http://norvig.com/sudoku.html "Peter 
Norvig's Sudoku solver explained") (originally written in Python).

On my laptop, Sensei solves up to 1100 random Sudokus per second.

## Compilation

The code makes use of some C++0x syntactic sugar as well as the
boost::thread library, so I recommend compiling it like this:

	sensei$ g++ -std=c++0x -lboost_thread-mt -O2 -o bin/sensei src/sensei.cpp

You need g++ and the boost::thread library (`libboost-thread-dev` on
Debian).

## Usage

Sensei reads Sudokus from the command line. The easiest way to use it
is by piping, e.g. `cat test/top95.txt | ./bin/sensei`. Some Sudoku lists
(also from Peter Norvig) are provided in `/test`.

Each Sudoku is represented as a flat list of cell values, 81 chars long,
as you would read the cell values from the top left. Example:

	.94...13..............76..2.8..1.....32.........2...6.....5.4.......8..7..63.4..8

`.` stands for 'unknown value'.

## Performance

The `timefile.sh` script can be run as a benchmark: `./bin/timefile.sh
test/*`.

## Optimization

`optimize.sh` goes one step further. It compiles Sensei using different
optimization flags, outputs the binary's file size and runs the above
benchmark. On my laptop, the output is as follows:

	sensei/bin$ ./optimize.sh 
	Optimization -O0
	168K    sensei
	../test/hardest.txt: 0.067 secs (164 Hz)
	../test/msk_009.txt: 4.763 secs (212 Hz)
	../test/top95.txt: 0.961 secs (98 Hz)

	Optimization -O1
	60K     sensei
	../test/hardest.txt: 0.017 secs (647 Hz)
	../test/msk_009.txt: 0.910 secs (1110 Hz)
	../test/top95.txt: 0.190 secs (500 Hz)

	Optimization -O2
	60K     sensei
	../test/hardest.txt: 0.016 secs (687 Hz)
	../test/msk_009.txt: 0.866 secs (1167 Hz)
	../test/top95.txt: 0.187 secs (508 Hz)

	Optimization -O3
	60K     sensei
	../test/hardest.txt: 0.023 secs (478 Hz)
	../test/msk_009.txt: 0.859 secs (1176 Hz)
	../test/top95.txt: 0.186 secs (510 Hz)

--curiousleo
