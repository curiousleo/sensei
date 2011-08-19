from copy import copy, deepcopy
import logging
from time import clock
from multiprocessing import Pool

S1_STR = '...6....445....2......893..97......3.63........4..27..6.9.5.............5....3.61'

ROWS = 'ABCDEFGHI'
COLS = '123456789'
VALUES = set(range(1, 10))

cross = lambda A, B: [a+b for a in A for b in B]
CELLS  = cross(ROWS, COLS)
UNITS = ([cross(ROWS, c) for c in COLS]
          + [cross(r, COLS) for r in ROWS]
          + [cross(rs, cs) for rs in ('ABC','DEF','GHI') for cs in ('123','456','789')])
unitdict = dict((s, [u for u in UNITS if s in u]) for s in CELLS)
PEERS = dict((s, set(sum(unitdict[s],[]))-set([s])) for s in CELLS)

class SudokuContradictionError(Exception):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr(self.value)

def solved(sudoku):
    return all(len(value[1]) == 1 for value in sudoku.items())

def display(sudoku):
    width = 1 + max(len(sudoku[c]) for c in CELLS)
    line = '+'.join(['-'*(width*3)]*3)

    out = ''

    for row in ROWS:
        out += ''.join(''.join(str(i) for i in sudoku[row+col]).center(width)+('|' if col in '36' else '') for col in COLS) + '\n'
        if row in 'CF': out += line + '\n'

    print(out)

def assign(sudoku, cell, value):
    '''(1) If a cell has only one possible value, then eliminate that value
    from the cell's peers.'''
    assert(type(value) is int)
    if value in sudoku[cell]:
        sudoku[cell] = set([value])
        logging.debug('Assigned {0} to {1}'.format(value, cell))
    
        for peer in PEERS[cell]:
            sudoku[peer].difference_update(set([value]))
            if len(sudoku[peer]) is 0:
                raise SudokuContradictionError('Peer {0} reduced to zero'.format(peer))

        return True
    else:
        raise ValueError('Cell {0} [{1}] does not contain {2}'.format(cell,
            sudoku[cell], value))

def eliminate(sudoku):
    '''(2) If a unit has only one possible place for a value, then put the
    value there.'''
    changed = False
    for unit in UNITS:
        for value in VALUES:
            cells = [cell for cell in unit if value in sudoku[cell]]
            if len(cells) is 1:
                if len(sudoku[cells[0]]) is not 1:
                    assign(sudoku, cells[0], value)
                    changed = True
    if changed: eliminate(sudoku)

def solve(sudoku):
    if solved(sudoku): return sudoku

    try:
        n, cell = min((len(sudoku[cell]), cell) for cell in sudoku if len(sudoku[cell]) > 1)
    except ValueError:
        display(sudoku)
        raise RuntimeError
    for guess in sudoku[cell]:
        logging.debug('Guessed {0} for cell {1}'.format(guess, cell))
        guess_sudoku = deepcopy(sudoku)
        try:
            assign(guess_sudoku, cell, guess)
            eliminate(guess_sudoku)
        except SudokuContradictionError as error:
            logging.debug('Contradiction found: {0}'.format(error))
            continue
        solution = solve(guess_sudoku)
        if solution: return solution
        else: continue

    return False

def sudoku_from_str(sudoku_str):
    assert(len(sudoku_str) == 81)

    sudoku = dict(zip(CELLS, (copy(VALUES) for i in range(81))))

    values = dict(zip(CELLS, sudoku_str))
    values = dict((k, int(v)) for k, v in values.items() if v in '123456789')

    for k, v in values.items(): assign(sudoku, k, v)

    return sudoku

def sudoku_from_input():
    print('Please type in your sudoku.')
    print('Use a dot for unknown cells, <Enter> at the end of a row.')
    sudoku_str = ''.join(input().ljust(9, '.') for i in range(9))
    return sudoku_from_str()

def sudokus_from_file(filename):
    with open(file=filename, mode='r') as sudoku_file:
        for line in sudoku_file:
            line = line.strip()
            if len(line) is 81: yield sudoku_from_str(line)

def solve_file(filename):
    print('Solving {} (using multiprocessing) ...'.format(filename))
    pool = Pool()
    results = pool.map(solve_worker, sudokus_from_file(filename))
    total = len(results)
    solved = len([result for result in results if result[0]])
    time_taken = [t for (r, t) in results]
    print('Solved {} of {} sudokus (avg {:.2f} secs ({:.0f} Hz), min {:.2f} secs, max {:.2f} secs, total {:.2f})' \
            .format(solved, total, sum(time_taken)/total, total/sum(time_taken),
                min(time_taken), max(time_taken), sum(time_taken)))

def solve_worker(sudoku):
    start = clock()
    result = solve(sudoku)
    time_taken = clock() - start
    return(bool(result), time_taken)

if __name__ == '__main__':
    pass
