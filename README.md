# The Sensei Loves to Solve Sudokus

_Sensei_ is basically a C++ implementation of Peter Norvig's excellent
[Sudoku solver](http://norvig.com/sudoku.html "Peter Norvig's Sudoku
solver explained").

## Usage

`sensei` reads Sudokus from the command line. The easiest way to use it
is by piping: `cat` _`sudokus.txt`_ `| ./sensei`. Some Sudoku lists
(also from Peter Norvig) are provided in `/test`.

### Format

Sudokus are saved as a flat list of cell values, e.g.:

	.94...13..............76..2.8..1.....32.........2...6.....5.4.......8..7..63.4..8

`.` stands for 'unknown value'.

## Performance

The _sensei_ is *fast*. The `timefile.sh` script can be run as a
benchmark, e.g. `./bin/timefile.sh test/*`.

## Optimization

`optimize.sh` goes one step further. It compiles the _sensei_ program
using different optimization flags and runs the above benchmark. On my
laptop, the output is as follows:

	sensei/bin$ ./optimize.sh
	Optimization -O0
	124K    sensei
	../test/hardest.txt: 0.137 secs (80 Hz)
	../test/msk_009.txt: 11.113 secs (90 Hz)
	../test/top95.txt: 2.239 secs (42 Hz)

	Optimization -O1
	44K     sensei
	../test/hardest.txt: 0.027 secs (407 Hz)
	../test/msk_009.txt: 2.098 secs (481 Hz)
	../test/top95.txt: 0.434 secs (218 Hz)

	Optimization -O2
	40K     sensei
	../test/hardest.txt: 0.027 secs (407 Hz)
	../test/msk_009.txt: 2.072 secs (487 Hz)
	../test/top95.txt: 0.430 secs (220 Hz)

	Optimization -O3
	40K     sensei
	../test/hardest.txt: 0.036 secs (305 Hz)
	../test/msk_009.txt: 2.031 secs (497 Hz)
	../test/top95.txt: 0.429 secs (221 Hz)

--curiousleo
