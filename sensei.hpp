#ifndef GUARD_sensei_h
#define GUARD_sensei_h

/* A Sudoku is a vector of unsigned chars (Values).
 * The vector contains the remaining possible values for the corresponding
 * square.
 */
typedef unsigned char Position;
typedef unsigned char Values;
typedef std::vector<Values> Sudoku;

/* Vectors of positions */
typedef std::vector<std::vector<Position> > PositionVector;

void init(void);

#endif // GUARD_sensei_h guard
