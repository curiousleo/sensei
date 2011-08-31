#include <vector>

#include "cover.hpp"

ExactCover::ExactCover() {};

void ExactCover::add_rows(const std::vector<std::vector<bool> > *rows) {
	int row_id = 0;
	std::vector<std::vector<bool> >::const_iterator row_it;
	
	// Add each row
	for (row_it = rows->begin(); row_it != rows->end(); ++row_it) {
		add_row(&(*row_it), row_id);
		++row_id;
	}
}

void ExactCover::add_row(const std::vector<bool> *row, int row_id) {
	std::vector<bool>::const_iterator col_it;

	// Add each element
	for (col_it = row->begin(); col_it != row->end(); ++col_it) {
		;
	}
}

// vi: set tabstop=4 softtabstop=4 shiftwidth=4 smarttab noexpandtab:
