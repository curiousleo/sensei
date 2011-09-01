#ifndef GUARD_cover_h
#define GUARD_cover_h

// Classes & Structs
struct ExactCoverNode;
struct ExactCoverColumn;
struct ExactCover;

// Knuth: "data object x" or node_struct
struct ExactCoverNode {
	// Members
	ExactCoverNode *left, *right;
	ExactCoverNode *up, *down;
	ExactCoverColumn *column;

	int tag;
};

// Knuth: "column object" or col_struct
struct ExactCoverColumn {
	// Members
	ExactCoverNode head;
    ExactCoverColumn *prev, *next;

	std::vector<ExactCoverNode>::size_type size, index;
};

class ExactCover {
public:
	// Constructors
	ExactCover();
	ExactCover(const std::vector<std::vector<bool> > *rows);

	// Destructor
	~ExactCover();

	// Public prototypes
	void add_rows(const std::vector<std::vector<bool> > *rows);
	bool search();

	// Public members
	std::vector<int> solution;

private:
	// Private prototypes
	void cover_row(ExactCoverNode*);
	void cover_column(ExactCoverColumn*);
	void uncover_row(ExactCoverNode*);
	void uncover_column(ExactCoverColumn*);

	void add_row(const std::vector<bool> *row, const int row_id);
	void init_columns(const int col_count);
	void link_node(
			const int row_i, const int col_i, ExactCoverNode **row_start);
	ExactCoverColumn *smallest_column();
	void save_solution(void);

	// Private members
	ExactCoverColumn *root;

	ExactCoverColumn *cur_column;
	ExactCoverNode *cur_row;

	std::vector<ExactCoverNode*> choice;
	std::vector<ExactCoverNode*> nodes;
	std::vector<ExactCoverColumn*> columns;
};

// Enumerations
enum SearchMode {FORWARD, ADVANCE, BACKUP, RECOVER, DONE};

#endif // GUARD_cover_h guard

// vi: set tabstop=4 softtabstop=4 shiftwidth=4 smarttab noexpandtab:
