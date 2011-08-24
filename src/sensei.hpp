#ifndef GUARD_sensei_h
#define GUARD_sensei_h


/* A Sudoku is a vector of unsigned chars (Values).
 * The vector contains the remaining possible values for the corresponding
 * cell.
 */
typedef unsigned char Position;
typedef unsigned char Value;
typedef std::set<Value> Values;
typedef std::vector<Values, std::allocator<Position> > Sudoku;

void init(void);
void read(Sudoku&, const std::string);
void display(const Sudoku);
bool solve(Sudoku&);

void assign(Sudoku&, Position, Value);
void eliminate(Sudoku&);
bool solved(const Sudoku&);

#endif // GUARD_sensei_h guard
