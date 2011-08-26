#ifndef GUARD_sensei_h
#define GUARD_sensei_h

/* A Sudoku is a vector of sets of unsigned chars (Values).
 * The set contains the remaining possible values for the corresponding
 * cell.
 */

// Typedefs
typedef unsigned char tiny;
typedef bool Values[9];
typedef Values Sudoku[81];

// Prototypes
void init(void);
void read(Sudoku&, const std::string);
void display(const Sudoku&);
bool solve(Sudoku&);

void assign(Sudoku&, const tiny, const tiny);
void eliminate(Sudoku&);
bool solved(const Sudoku&);

void solve_worker(void);
tiny count(const Sudoku&);

#endif // GUARD_sensei_h guard
