#ifndef GUARD_sensei_h
#define GUARD_sensei_h

/* A Sudoku is a vector of sets of unsigned chars (Values).
 * The set contains the remaining possible values for the corresponding
 * cell.
 */

// Typedefs
typedef unsigned char tiny;
typedef std::set<tiny> Values;
typedef std::array<Values, 81> Sudoku;

// Prototypes
void init(void);
void read(Sudoku&, const std::string);
void display(const Sudoku&);
bool solve(Sudoku&);

void solve_worker(void);

#endif // GUARD_sensei_h guard
