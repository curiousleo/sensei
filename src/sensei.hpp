#ifndef GUARD_sensei_h
#define GUARD_sensei_h

/* A Sudoku is a vector of sets of unsigned chars (Values).
 * The set contains the remaining possible values for the corresponding
 * cell.
 */

// Typedefs
typedef unsigned char Position;
typedef unsigned char Value;
typedef std::set<Value> Values;
typedef std::vector<Values, std::allocator<Position> > Sudoku;

// Prototypes
void init(void);
void read(Sudoku&, const std::string);
void display(const Sudoku&);
bool solve(Sudoku&);

void assign(Sudoku&, Position, Value);
void eliminate(Sudoku&);
bool solved(const Sudoku&);

void solve_worker(void);

#endif // GUARD_sensei_h guard
