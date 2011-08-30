#ifndef GUARD_cover_h
#define GUARD_cover_h

// Classes & Structs

// Knuth: "data object x"
struct ExactCoverNode {
	// Members
	ExactCoverNode *left, *right;
	ExactCoverNode *up, *down;
	ExactCoverNode *column;

	bool value;
};

// Knuth: "column object"
struct ExactCoverColumn: ExactCoverNode {
	// Members
    ExactCoverColumn *prev, *next;

    int size;
	std::string name;
};

class ExactCover {
public:
	// Constructors
	ExactCover();

	// Member prototypes
	template <class C>
	void add_row(const C*);
	void cover(ExactCoverColumn*);
	void uncover(ExactCoverColumn*);

private:
	// Members
	ExactCoverNode *root, *current;
};

#endif // GUARD_cover_h guard

// vi: set tabstop=4 softtabstop=4 shiftwidth=4 smarttab noexpandtab:
