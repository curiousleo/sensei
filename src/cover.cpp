#include <vector>

#include "cover.hpp"

ExactCover::~ExactCover() {
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
	// Matrix empty?
	if (root->right == root && root->down == root) {
		return true;
	}

	// Choose column deterministically
	ExactCoverColumn *column = smallest_column();

	// Choose row nondetermilistically
	ExactCoverNode *row = column->head.down;
	choice.push_back(row);

	// Column empty?
	if (row == &(column->head)) {
		// Backup
	}

	cover_row(row);

	// Column vector empty?
	if (columns.front()->next == columns.front()) {
		// Solution found
	}

	search();
}

void ExactCover::cover_row(ExactCoverNode *row) {
	for (
			ExactCoverColumn *node = row->right;
			node != row; node = node->right) {
		cover_column(node->column);
	}
}

void ExactCover::uncover_row(ExactCoverNode *row) {
	for (
			ExactCoverColumn *node = row->right;
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

void ExactCover::add_rows(const std::vector<std::vector<bool> > *rows) {
	std::vector<std::vector<bool> >::size_type row_i;

	// Add columns
	init_columns(rows->size());
	
	// Add each row
	for (row_i = 0; row_i != rows->size(); ++row_i) {
		add_row(&(*rows)[row_i], row_i);
	}
}

void ExactCover::init_columns(const int col_count) {
	// Reserve space for all the columns and the root column
	columns.reserve(col_count + 1);

	// Create root column
	ExactCoverColumn *root = new ExactCoverColumn;
	root->index = 0;
	root->head.tag = 0;

	columns.push_back(root);

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

void ExactCover::add_row(const std::vector<bool> *row, const int row_i) {
	std::vector<bool>::size_type col_i;
	ExactCoverNode *row_start = NULL;

	// Add each element (node)
	for (col_i = 0; col_i != row->size(); ++col_i) {
		if ((*row)[col_i]) {
			link_node(row_i, col_i, &row_start);
		}
	}
}

void ExactCover::link_node(const int row_i, const int col_i, ExactCoverNode **row_start) {
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
