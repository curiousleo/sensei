#ifndef GUARD_sensei_h
#define GUARD_sensei_h

/* A Sudoku is a vector of sets of unsigned chars (Values).
 * The set contains the remaining possible values for the corresponding
 * cell.
 */

// Typedefs
typedef unsigned char tiny;
typedef std::array<bool, 9> Values;
typedef std::array<Values, 81> Sudoku;

// Prototypes
void init(void);
void read(Sudoku&, const std::string);
void display(const Sudoku&);
bool solve(Sudoku&);

void assign(Sudoku&, const tiny, const tiny);
void eliminate(Sudoku&);
bool solved(const Sudoku&);

void solve_worker(void);
tiny count(const Values&);

#endif // GUARD_sensei_h guard
