from copy import deepcopy
from logging import debug

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

class Sudoku(object):
    def __init__(self):
        self._sudoku_data = [deepcopy(VALUES) for i in range(81)]
        
    def __getitem__(self, cell):
        return self._sudoku_data[ROWS.index(cell[0]) * 9 + COLS.index(cell[1])]
            
    def __setitem__(self, cell, value):
        if type(value) is int:
            value = set([value])
        elif type(value) is list or type(value) is tuple:
            value = set(value)

        self._sudoku_data[ROWS.index(cell[0]) * 9 + COLS.index(cell[1])] = value

    def __repr__(self):
        width = 1 + max(len(self[c]) for c in CELLS)
        line = '+'.join(['-'*(width*3)]*3)

        out = ''

        for row in ROWS:
            out += ''.join(''.join(str(i) for i in self[row+col]).center(width)+('|' if col in '36' else '') for col in COLS) + '\n'
            if row in 'CF': out += line + '\n'

        return out

    @property
    def solved(self):
        return all(len(self[cell]) is 1 for cell in CELLS)

    def assign(self, cell, value):
        '''(1) If a cell has only one possible value, then eliminate that value
        from the cell's peers.'''
        assert(type(value) is int)
        if value in self[cell]:
            self[cell] = value
            debug('Assigned {0} to {1}'.format(value, cell))
        
            for peer in PEERS[cell]:
                # possibilities = len(self[peer])
                self[peer].difference_update(set([value]))
                if len(self[peer]) is 0:
                    raise SudokuContradictionError('Peer {0} reduced to zero'.format(peer))
                # elif len(self[peer]) is 1 and possibilities is not 1:
                    # self.assign(peer, next(iter(self[peer])))

            return True
        else:
            raise ValueError('Cell {0} [{1}] does not contain {2}'.format(cell,
                self[cell], value))

    def eliminate(self):
        '''(2) If a unit has only one possible place for a value, then put the
        value there.'''
        changed = False
        for unit in UNITS:
            for value in VALUES:
                cells = [cell for cell in unit if value in self[cell]]
                if len(cells) is 1:
                    if len(self[cells[0]]) is not 1:
                        # if not self.assign(cells[0], value): return False
                        self.assign(cells[0], value)
                        changed = True
        if changed: self.eliminate()

class SudokuContradictionError(Exception):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr(self.value)

def solve(sudoku):
    if sudoku.solved: return sudoku

    n, cell = min((len(sudoku[cell]), cell) for cell in CELLS if
        len(sudoku[cell]) > 1)
    for guess in sudoku[cell]:
        debug('Guessed {0} for cell {1}'.format(guess, cell))
        guess_sudoku = deepcopy(sudoku)
        try:
            guess_sudoku.assign(cell, guess)
            guess_sudoku.eliminate()
        except SudokuContradictionError as error:
            debug('Contradiction found: {0}'.format(error))
            continue
        solution = solve(guess_sudoku)
        if solution: return solution
        else: continue

    return False

def sudoku_from_str(sudoku_str):
    assert(len(sudoku_str) == 81)

    sudoku = Sudoku()

    values = dict(zip(CELLS, sudoku_str))
    values = dict((k, int(v)) for k, v in values.items() if v in '123456789')

    for k, v in values.items():
        if not sudoku.assign(k, v):
            return False

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
    import time
    time_taken = []
    unsolved = 0
    total = 0
    print('Solving {} ...'.format(filename))
    for sudoku in sudokus_from_file(filename):
        start = time.clock()
        if solve(sudoku) is False: unsolved += 1
        time_taken += [time.clock() - start]
        total += 1
    print('Solved {} of {} sudokus (avg {:.2f} secs ({:.0f} Hz), min {:.2f} secs, max {:.2f} secs)' \
            .format(total-unsolved, total, sum(time_taken)/total,
                total/sum(time_taken), min(time_taken), max(time_taken)))

if __name__ == '__main__':
    pass
