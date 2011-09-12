#include <list>
#include <vector>

#include "cover.hpp"

ExactCover::ExactCover() {
	cur_node = NULL;
	cur_column = NULL;
}

ExactCover::ExactCover(const std::list<std::list<int> > &rows) {
	cur_node = NULL;
	cur_column = NULL;

	set_rows(rows);
}

ExactCover::~ExactCover() {
	free_memory();
}

void ExactCover::free_memory() {
	// Delete all columns from memory
	for (
			std::vector<ExactCoverColumn*>::iterator col_it = columns.begin();
			col_it != columns.end(); ++col_it) {
		delete *col_it;
	}

	// Delete all nodes from memory
	for (
			std::vector<ExactCoverNode*>::iterator node_it = nodes.begin();
			node_it != nodes.end(); ++node_it) {
		delete *node_it;
	}
}

bool ExactCover::search() {
	// If search has been called before, try to find next solution
	if (cur_node == NULL && cur_column == NULL) mode = FORWARD;
	else mode = RECOVER;

	while (true) {
		switch (mode) {
			case FORWARD:
				// Choose column deterministically
				cur_column = smallest_column();
				cover_column(cur_column);

				// Choose row nondetermilistically
				cur_node = cur_column->head.down;
				choice.push_back(cur_node);

			case ADVANCE:
				// If all rows of cur_column have been checked, go to 'BACKUP'
				if (cur_node == &(cur_column->head)) {
					mode = BACKUP; continue;
				}

				cover_row(cur_node);

				// Matrix empty?
				if (root->next == root) {
					// Solution found
					save_solution();
					return true;
				}

				mode = FORWARD; continue;

			case BACKUP:
				// Undo 'FORWARD'
				uncover_column(cur_column);

				if (choice.size() == 1) {
					mode = DONE; continue;
				}

				cur_node = choice.back();
				cur_column = cur_node->column;

			case RECOVER:
				// Undo 'ADVANCE'
				uncover_row(cur_node);

				choice.pop_back();

				cur_node = cur_node->down;
				choice.push_back(cur_node);
				
				mode = ADVANCE; continue;

			case DONE:
				// No solution found
				return false;
		}
	}
}

void ExactCover::save_solution() {
	solution.clear();

	// For each node in 'choice', save its tag in 'solution'
	for (
			std::vector<ExactCoverNode*>::const_iterator node_it =
			choice.begin(); node_it != choice.end(); ++node_it) {
		solution.push_back((*node_it)->tag);
	}
}

ExactCoverColumn *ExactCover::smallest_column() {
	int min_size = -1;
	ExactCoverColumn *min_column = NULL;

	for (
			ExactCoverColumn *column = root->next;
			column != root; column = column->next) {
		if (min_size == -1 || column->size < min_size) {
			min_size = column->size;
			min_column = column;
		}
	}

	return min_column;
}

void ExactCover::cover_row(ExactCoverNode *row) {
	// Cover each column linked to this row
	for (
			ExactCoverNode *node = row->right;
			node != row; node = node->right) {
		cover_column(node->column);
	}
}

void ExactCover::uncover_row(ExactCoverNode *row) {
	// Uncover each column linked to this row
	for (
			ExactCoverNode *node = row->right;
			node != row; node = node->right) {
		uncover_column(node->column);
	}
}

void ExactCover::cover_column(ExactCoverColumn *column) {
	// Unlink column from the column list
	column->prev->next = column->next;
	column->next->prev = column->prev;

	// Cover each row this column points to
	// Go through each row
	for (
			ExactCoverNode *row = column->head.down;
			row != &(column->head); row = row->down) {

		// Go through each node in that row
		for (
				ExactCoverNode *node = row->right;
				node != row; node = node->right) {

			// Unlink node from column
			node->up->down = node->down;
			node->down->up = node->up;

			// Decrease this node's column size
			node->column->size--;
		}
	}
}

void ExactCover::uncover_column(ExactCoverColumn *column) {
	// Re-insert column into column list
	column->prev->next = column->next->prev = column;
	
	// Uncover each row this column points to
	// Go through each row
	for (
			ExactCoverNode *row = column->head.down;
			row != &(column->head); row = row->down) {

		// Go through each node in that row
		for (
				ExactCoverNode *node = row->right;
				node != row; node = node->right) {

			// Re-insert node into column
			node->up->down = node->down->up = node;

			// Increase this node's column size
			node->column->size++;
		}
	}
}

void ExactCover::set_rows(const std::list<std::list<int> > &rows) {
	std::list<std::list<int> >::const_iterator row_it;

	// Clear columns & rows
	free_memory();
	choice.clear(); nodes.clear(); columns.clear();

	int max_column = 0;

	// Find number of columns
	for (row_it = rows.begin(); row_it != rows.end(); ++row_it) {
		for (
				std::list<int>::const_iterator node_it =
				row_it->begin(); node_it != row_it->end(); ++node_it) {
			if (*node_it > max_column) max_column = *node_it;
		}
	}

	// Initialize columns
	init_columns(max_column);
	
	// Add each row
	int row_i = 0;
	for (row_it = rows.begin(); row_it != rows.end(); ++row_it) {
		add_row((*row_it), row_i);
		++row_i;
	}
}

void ExactCover::init_columns(const int col_count) {
	// Reserve space for all the columns and the root column
	columns.reserve(col_count + 1);

	// Create root column
	root = new ExactCoverColumn;
	columns.push_back(root);
	root->index = 0;
	root->head.tag = 0;

	for (int col_i = 0; col_i != col_count; ++col_i) {
		ExactCoverColumn *column = new ExactCoverColumn;

		column->index = col_i + 1;

		// Initialize empty column
		column->size = 0;
		column->head.tag = -1; // ???

		// Initialize column head
		column->head.up = &(column->head);
		column->head.down = &(column->head);

		root->prev = column; // Can be optimized
		columns.back()->next = column;

		column->prev = columns.back();
		column->next = root;

		columns.push_back(column);
	}
}

void ExactCover::add_row(const std::list<int> &row, const int row_i) {
	ExactCoverNode *row_start = NULL;

	// Add each element (node)
	for (
			std::list<int>::const_iterator node_it = row.begin();
			node_it != row.end(); ++node_it) {
		link_node(row_i, *node_it, &row_start);
	}
}

void ExactCover::link_node(
		const int row_i, const int col_i, ExactCoverNode **row_start) {
	// Goal: Populate all node members: left, right, up, down, column, tag
	
	ExactCoverNode *node = new ExactCoverNode;
	nodes.push_back(node);

	// Populate tag and column fields
	node->tag = row_i;
	node->column = columns[col_i];

	// Set row_start to node if row is the first node in that column
	if (*row_start == NULL) *row_start = node;

	// Populate down field
	node->down = &(columns[col_i]->head);
	columns[col_i]->head.up = node;

	// Populate up field
	// Is node the only element in that column?
	if (columns[col_i]->size == 0) {
		node->up = &(columns[col_i]->head);
		columns[col_i]->head.down = node;
	} else {
		// Insert as last element in that column
		node->up = columns[col_i]->head.up;
		columns[col_i]->head.up->down = node;
	}

	// Populate left and right fields
	if (*row_start != node) {
		// Insert as last element in that row
		node->left = (*row_start)->left;
		node->right = (*row_start);

		(*row_start)->left->right = node;
		(*row_start)->left = node;
	} else {
		// Insert as first and only element in that row
		node->left = node;
		node->right = node;
	}

	columns[col_i]->size++;
}

// vi: set tabstop=4 softtabstop=4 shiftwidth=4 smarttab noexpandtab:
