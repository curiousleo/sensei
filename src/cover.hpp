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

template <class C>
class ExactCover {
public:
	// Constructors
	ExactCover();
	ExactCover(const C*);

	// Destructor
	~ExactCover();

	// Public prototypes
	void add_rows(const C*);
	bool search();

	// Public members
	std::vector<int> solution;

private:
	// Private prototypes
	void add_row(const C*);
	void cover(ExactCoverColumn*);
	void uncover(ExactCoverColumn*);

	// Private members
	ExactCoverNode *root, *current;

	std::vector<ExactCoverNode*> choice;
	std::vector<ExactCoverNode*> nodes;
	std::vector<ExactCoverColumn*> columns;
};

#endif // GUARD_cover_h guard

// vi: set tabstop=4 softtabstop=4 shiftwidth=4 smarttab noexpandtab:
