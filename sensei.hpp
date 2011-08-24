#ifndef GUARD_sensei_h
#define GUARD_sensei_h


#define SUDOKU1 "...6....445....2......893..97......3.63........4..27..6.9.5.............5....3.61"

/* A Sudoku is a vector of unsigned chars (Values).
 * The vector contains the remaining possible values for the corresponding
 * cell.
 */
typedef unsigned char Position;
typedef unsigned char Value;
typedef std::set<Value> Values;
typedef std::vector<Values, std::allocator<Position> > Sudoku;

void init(void);
Sudoku read(std::string);
void display(Sudoku);
void solve(Sudoku&);

void assign(Sudoku&, Position, Value);
void eliminate(Sudoku&);

#endif // GUARD_sensei_h guard
