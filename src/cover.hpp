#ifndef GUARD_cover_h
#define GUARD_cover_h

// Enumerations
enum SearchMode {FORWARD, ADVANCE, BACKUP, RECOVER, DONE};

// Classes & Structs
struct ExactCoverNode;
struct ExactCoverColumn;
class ExactCover;

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
	ExactCover(const std::list<std::list<int> > rows);

	// Destructor
	~ExactCover();

	// Public prototypes
	void set_rows(const std::list<std::list<int> > rows);
	bool search();

	// Public members
	std::vector<int> solution;

private:
	// Private prototypes
	void cover_row(ExactCoverNode*);
	void cover_column(ExactCoverColumn*);
	void uncover_row(ExactCoverNode*);
	void uncover_column(ExactCoverColumn*);

	void add_row(const std::list<int> row, const int row_id);
	void init_columns(const int col_count);
	void link_node(const int row_i, const int col_i, ExactCoverNode **row_start);
	ExactCoverColumn *smallest_column();
	void save_solution(void);
	void free_memory();

	// Private members
	ExactCoverColumn *root, *cur_column;
	ExactCoverNode *cur_node;

	std::vector<ExactCoverNode*> choice;
	std::vector<ExactCoverNode*> nodes;
	std::vector<ExactCoverColumn*> columns;

	SearchMode mode;
};

#endif // GUARD_cover_h guard

// vi: set tabstop=4 softtabstop=4 shiftwidth=4 smarttab noexpandtab:
