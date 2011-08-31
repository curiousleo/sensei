#ifndef GUARD_cover_h
#define GUARD_cover_h

// Classes & Structs

// Knuth: "data object x" or node_struct
struct ExactCoverNode {
	// Members
	ExactCoverNode *left, *right;
	ExactCoverNode *up, *down;
	ExactCoverNode *column;
};

// Knuth: "column object" or col_struct
struct ExactCoverColumn: ExactCoverNode {
	// Members
	ExactCoverNode head;
    ExactCoverColumn *prev, *next;

    int size, index;
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
	void cover(ExactCoverColumn*);
	void uncover(ExactCoverColumn*);

	void add_row(const std::vector<bool> *row, int row_id);
	void link_node(const std::vector<bool> *row, int row_id);

	// Private members
	ExactCoverNode *root, *current;

	std::vector<ExactCoverNode*> choice;
	std::vector<ExactCoverNode*> nodes;
	std::vector<ExactCoverColumn*> columns;
};

#endif // GUARD_cover_h guard

// vi: set tabstop=4 softtabstop=4 shiftwidth=4 smarttab noexpandtab:
