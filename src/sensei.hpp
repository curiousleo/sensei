#ifndef GUARD_sensei_h
#define GUARD_sensei_h

/* A Sudoku is a vector of sets of unsigned chars (Values).
 * The set contains the remaining possible values for the corresponding
 * cell.
 */

// Definitions
#define N 9
#define NN N*N

// Macros
#define BOX(cell_i)						((cell_i) % 27) * 3 + ((cell_i) % 9) / 3
#define ROW(cell_i, val_i)				       N * ((val_i)) + (cell_i)

#define ROW_COND(cell_i, val_i) 		NN +   N * ((val_i)) + ((cell_i) / 9)
#define COLUMN_COND(cell_i, val_i)	2*NN + N * ((val_i)) + ((cell_i) % 9)
#define BOX_COND(cell_i, val_i) 		3*NN + N * ((val_i)) + BOX(cell_i)

// Typedefs
typedef std::list<std::list<int> > Sudoku;
typedef std::vector<int> Solution;

// Prototypes
void init(void);
void read(Sudoku&, const std::string);
void display(const Solution&);
Solution solve(const Sudoku&);

void assign(Sudoku&, const int, const int);
void solve_worker(void);

#endif // GUARD_sensei_h guard
