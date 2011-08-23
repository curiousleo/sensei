#ifndef GUARD_sensei_h
#define GUARD_sensei_h

/* A Sudoku is a vector of unsigned chars (Values).
 * The vector contains the remaining possible values for the corresponding
 * cell.
 */
typedef unsigned char Position;
typedef unsigned char Values;
typedef std::vector<Values, std::allocator<Position>> Sudoku;

void init(void);

#endif // GUARD_sensei_h guard
