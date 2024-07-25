#pragma once
#include <vector>
#include <string>

extern int verbose;

extern int ncol, mrow, nveh;
	//vehicles and columns are number from 1 to ncol
	//horizontal levels are numbered from 0 to mrow, assuming no horizontal move is allowed at levels 0 and mrow
extern std::vector<int> Alpha;
extern std::vector<int> Omega;
	//Alpha and Omega will have nveh elements
int alpha(int k);
int omega(int k);
int first_i(int k);
int last_i(int k);
bool isR(int k);
bool isL(int k);
bool isS(int k);

typedef struct conflict {
public:
	int p;
	int q;
	float c;
	int before;
	int after;
	int center;
	bool isEven;
} Conflict;
extern std::vector<Conflict> C;
extern std::vector<bool> isConfictingVehicle;
int extractConflicts(void);

void createInstanceRandom(int n_col, int n_row, int n_veh);
void destinationShuffle(void);

/* CONFLICT GRAPH */
#include "Graph.h"
extern Graph CconflictG;
extern vector<bool> isRoot;
extern vector<bool> isSigmaAdj;
int createConflictCgraph(void);
inline bool isInCCPath(int v) { return ! isRoot[v]; };

