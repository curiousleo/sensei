#ifndef GUARD_cover_h
#define GUARD_cover_h

// Classes & Structs

// Knuth: "data object x" or node_struct
struct ExactCoverNode {
	// Members
	ExactCoverNode *left, *right;
	ExactCoverNode *up, *down;
	ExactCoverNode *column;

	bool value;
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

	// Public prototypes
	template <class C1>
	void fill(const C1*);
	void cover(ExactCoverColumn*);
	void uncover(ExactCoverColumn*);

private:
	// Private prototypes
	template <class C2>
	void add_row(const C2*);

	// Private members
	ExactCoverNode *root, *current;
	std::vector<ExactCoverNode> nodes;
	std::vector<ExactCoverColumn> columns;
};

#endif // GUARD_cover_h guard

// vi: set tabstop=4 softtabstop=4 shiftwidth=4 smarttab noexpandtab:
