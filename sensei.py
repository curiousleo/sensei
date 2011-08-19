from copy import deepcopy

S1_STR = '.94...13..............76..2.8..1.....32.........2...6.....5.4.......8..7..63.4..8'

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
        
    def __getitem__(self, key):
        return self._sudoku_data[ROWS.index(key[0]) * 9 + COLS.index(key[1])]
            
    def __setitem__(self, key, value):
        if type(value) is int:
            value = set([value])
        elif type(value) is list or type(value) is tuple:
            value = set(value)

        self._sudoku_data[ROWS.index(key[0]) * 9 + COLS.index(key[1])] = value

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

    def assign(self, key, value):
        '''(1) If a cell has only one possible value, then eliminate that value
        from the cell's peers.'''
        assert(type(value) is int)
        if value in self[key]:
            self[key] = value
        
            for peer in PEERS[key]:
                self[peer].difference_update(set([value]))
                if len(self[peer]) is 0: return False

            return True
        return False

    def eliminate(self):
        '''(2) If a unit has only one possible place for a value, then put the
        value there.'''
        changed = False
        for unit in UNITS:
            for value in VALUES:
                cells = [cell for cell in unit if value in self[cell]]
                if len(cells) is 1:
                    if len(self[cells[0]]) is not 1:
                        self.assign(cells[0], value)
                        changed = True
        if changed: self.eliminate()

def solve(sudoku):
    sudoku.eliminate()
    if not sudoku.solved:
        n, cell = min((len(sudoku[cell]), cell) for cell in CELLS if
                len(sudoku[cell]) > 1)
        guess_sudoku = deepcopy(sudoku)
        for guess in guess_sudoku[cell]:
            print('Guessed {0} for cell {1}'.format(guess, cell), end=' ')
            if not guess_sudoku.assign(cell, guess):
                print('... didn\'t work.')
                continue
            print('... worked.')
            return solve(guess_sudoku)
    else: return sudoku

def sudoku_from_str(sudoku_str):
    assert(len(sudoku_str) == 81)

    sudoku = Sudoku()

    values = dict(zip(CELLS, sudoku_str))
    values = dict((k, int(v)) for k, v in values.items() if v in '123456789')

    for k, v in values.items():
        sudoku.assign(k, v)

    return sudoku

if __name__ == '__main__':
    pass

# : vi: set expandtab softtabstop=4 tabstop=4 :
