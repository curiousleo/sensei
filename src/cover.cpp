#include "cover.hpp"

ExactCover::ExactCover() {};

ExactCover::add_row(std::array cont) {
	typedef typename std::array::size_type array_sz;

	root = &cont[0][0];
	array_sz col_i_start = 1;

	for (array_sz row_i = 0; row_i != cont.size(); ++row_i) {

		for (
				array_sz col_i = col_i_start;
				col_i != cont[row_i].size(); ++col_i) {
			;
		}

		col_i_start = 0;
	}
}
