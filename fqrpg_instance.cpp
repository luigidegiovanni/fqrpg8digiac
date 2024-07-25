#include "fqrpg_instance.h"

#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;

int ncol, mrow, nveh;
vector<int> Alpha;
vector<int> Omega;
inline int alpha(int k) { return Alpha[k]; }
inline int omega(int k) { return Omega[k]; }
inline int first_i(int k) {
	return (alpha(k) < omega(k)) ? alpha(k) : omega(k);
}
inline int last_i(int k) {
	return (alpha(k) > omega(k)) ? alpha(k) : omega(k);
}
bool isR(int k) { return (alpha(k) < omega(k)); }
bool isL(int k) { return (alpha(k) > omega(k)); }
bool isS(int k) { return (alpha(k) == omega(k)); }


vector<Conflict> C;
vector<bool> isConfictingVehicle;

int extractConflicts(void) {
  isConfictingVehicle.clear(); 
  isConfictingVehicle.resize(nveh, false);
  C.clear();
	
	for (int p = 0; p < nveh; ++p) {
		if (omega(p) == alpha(p)) continue; // p in S or no vehicle starts from colmun p
		if (omega(p) <= alpha(p)) continue; // p not in R
		for (int q = 0; q < nveh; ++q) {
			if (omega(q) == alpha(q)) continue; // q in S or no vehicle starts from colmun q
			if (omega(q) >= alpha(q)) continue; // q not in L
			if (alpha(p) > alpha(q)) continue;
			float c = (float(alpha(p)) + float(alpha(q))) / 2.0;
			float epsilon = 0.01;
			int before = ceil(c - 1 - epsilon);
			int after = floor(c + 1 + epsilon);
			int center = int(c + epsilon);
			bool isEven = (abs(c - center) <= epsilon);
			if (omega(p) >= c && omega(q) <= c) {
				C.push_back({ p,q,c,before,after,center,isEven });
				isConfictingVehicle[p] = isConfictingVehicle[q] = true;
			}
		}
	}
	return C.size();
}


void  destinationShuffle(void) {
  for (int v = 0; v < nveh; ++v) Omega.push_back(Alpha[v]);
  random_shuffle(Omega.begin(), Omega.end());
}


void createInstanceRandom(int n_col, int n_row, int n_veh) {
  Alpha.clear();
  Omega.clear();
  ncol = n_col;
  mrow = n_row;
  nveh = n_veh;
  for (int v = 0; v < nveh; ++v) Alpha.push_back(v + 1);
  destinationShuffle();

  if (verbose >= 2) {
	for (int k = 0; k < Alpha.size(); ++k) {
	  std::cout << "veh " << k << ") from " << Alpha[k] << " to " << Omega[k] << std::endl;
	}
	cout << ncol << " columns, " << mrow << " rows, " << nveh << " vehicles" << endl;
  }

}



/* CONFLICT GRAPH */
Graph CconflictG;
vector<bool> isRoot;
vector<bool> isSigmaAdj;

int createConflictCgraph(void) {

	CconflictG.clear();
	isRoot.clear();
	isRoot.resize(ncol, true); 
	isSigmaAdj.clear();
	isSigmaAdj.resize(ncol, false);
	
	int nC = extractConflicts();
	for (int k = 0; k < nC; ++k) {
		if (verbose >=2) cout << "L: " << alpha(C[k].p) << "[" << omega(C[k].p) << "] "
			<< " R: " << alpha(C[k].q) << "[" << omega(C[k].q) << "] "
			<< " c: " << C[k].c
			<< " b: " << C[k].before << " a: " << C[k].after << " center: " << C[k].center
			<< " Type: " << (C[k].isEven ? "Even (node)" : "Odd (arc)")
			<< (C[k].isEven && (omega(C[k].p) == C[k].c || omega(C[k].q) == C[k].c) ? " Type C (Terminal)" : " Type B")
			<< endl;
		if (C[k].isEven) {
		  if (omega(C[k].p) == C[k].c) { // p(q), q --> p (p over q)
			CconflictG.addEdge(C[k].q, C[k].p);
			isRoot[C[k].p] = false;
			isSigmaAdj[C[k].q] = (abs(omega(C[k].q) - omega(C[k].p)) == 1);
		  }
		  else if (omega(C[k].q) == C[k].c) { // q(p), p --> q (q over p)
			CconflictG.addEdge(C[k].p, C[k].q);
			isRoot[C[k].q] = false;
			isSigmaAdj[C[k].p] = (abs(omega(C[k].p) - omega(C[k].q)) == 1);
		  }
		}
	}

	return 0;
}
