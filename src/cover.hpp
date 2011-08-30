#ifndef GUARD_cover_h
#define GUARD_cover_h

// Classes & Structs
struct ExactCoverNode {
	// Members
	ExactCoverNode *left, *right;
	ExactCoverNode *up, *down;
	ExactCoverNode *column;
};

struct ExactCoverColumn {
	// Members
	ExactCoverNode head;
    ExactCoverColumn *prev, *next;

    int length;
    int index;
};

class ExactCover {
public:
	// Constructors
	ExactCover();

	// Member prototypes
	template <class C>		// Container used to construct row
	void add_row(const C*);
	void cover(ExactCoverColumn*);
	void uncover(ExactCoverColumn*);

private:
	// Members
	ExactCoverNode *root, *current;
};

#endif // GUARD_cover_h guard

// vi: set tabstop=4 softtabstop=4 shiftwidth=4 smarttab noexpandtab:
