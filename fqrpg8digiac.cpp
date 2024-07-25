#include "Graph.h"
#include "fqrpg_instance.h"

#include <iostream>

int visitConflictCgraph(void);
bool checkAssumption01(int& coutMissed, int& countCollisions);
bool checkAssumption02(int& coutMissed, int& countCollisions);

///////////////////////////////
// position (col,row) is encoded as col * 10 + row
inline int positionEncode(int col, int row) {
  return col * 10 + row;
}
map<int, vector<int>> route8; 

// function protptypes
int getCollisionsC(int i_veh);
int checkNodeCollision(int p, int q);
int buildRouteSdot1(int v); 
int buildRouteSdot2 (int v);
int buildRouteSdot3 (int i);
int buildRoute (int v);


using namespace std;

int status;
int verbose;

int main(int argc, char const* argv[]) {
	std::string flinp = "__random__";
	srand(0);//srand(1111110);

	try {
		verbose = 0;
  		std::string flinp = "__random__";
  		srand(0);
	
		int nsamples = 1e06;
		vector<int> ncc = {10, 25, 50, 75, 100, 125, 150, 175, 200, 225, 250, 300, 400, 500, 1000 };
		for (int ic = 0; ic < ncc.size(); ++ic) {
	  		int count1 = 0;
	  		int count2 = 0;
	  		int count3 = 0;
	  		int count4 = 0;
	  		bool isA01, isA02;
	  		int max01 = -1;
	  		int max02 = -1;
	  		int max04 = -1;
	  		int totCC = 0;
	  		int totCB = 0;
	  		for (int i = 0; i < nsamples; ++i, destinationShuffle()) {
				if (i % 10000 == 0) { cerr << "."; }
		
				createInstanceRandom(ncc[ic], 8, ncc[ic]);
				createConflictCgraph();
				// clear route8
				map<int, vector<int>>::iterator nit;
				for (nit = route8.begin(); nit != route8.end(); ++nit) {
					nit->second.clear();
				}
				route8.clear();

				int countMiss01 = 0;
				int countColl01 = 0;
				int countMiss02 = 0;
				int countColl02 = 0;
				if (isA01 = checkAssumption01(countMiss01,countColl01)) {
					//cerr << "\n(" << i << ")\n";
					++count1;
				} else if (countMiss01 > max01) max01 = countMiss01;
				if (isA02=checkAssumption02(countMiss02,countColl02)) {
					++count2;
				} else if (countMiss02 > max02) max02 = countMiss02;
				if (isA01 && isA02) {
					++count3;
				}
				if (countColl01 > 0 || countColl02 > 0) {
					++count4;
					int nCollC = (countColl01 > 0 ? countColl01 : 0);
					int nCollB =(countColl02 > 0 ? countColl02 : 0);
					int nColl = nCollB + nCollC;
					if (nColl > max04) max04 = nColl;
					totCC += nCollC;
					totCB += nCollB;
				}
				if (verbose >= 1) {
					for (int v = 0; v < ncol; ++v) {
						if (CconflictG.adj[v].size() <= 1) continue;
						cout << "vcol " << alpha(v) << ": has " << CconflictG.adj[v].size() << " succ; "
							<< " is SigA " << isSigmaAdj[v] << "; "
							<< " is Root " << isRoot[v] << "; "
							<< endl;
					}
					//char towait; cin >> towait;
				}
	  		}
	  		cout << '\n' << ncc[ic] << ": assumption one ok in " << count1 << " instances over " << nsamples << " (" << float(count1)/nsamples*100 << "%) max " << max01 << endl;
	  		cout << '\n' << ncc[ic] << ": assumption two ok in " << count2 << " instances over " << nsamples << " (" << float(count2)/nsamples*100 << "%) max " << max02 << endl;
	  		cout << '\n' << ncc[ic] << ": assumption all ok in " << count3 << " instances over " << nsamples << " (" << float(count3)/nsamples*100 << "%)" << endl;
	  		cout << '\n' << ncc[ic] << ": collisions        in " << count4 << " instances over " << nsamples << " (" << float(count4)/nsamples*100 << "%) totC " << totCC << " totB " << totCB << " max " << max04 << endl;
		}

		status = 0;
	}
	catch (std::exception& e)
	{
		std::cout << ">>>EXCEPTION: " << e.what() << "(" << flinp << ")";
		status = -1;
  	}

	if (verbose >= 1) {
		cout << "\nalgo digiacetal8 version 0";
		//_getch();
  	}
  	return status;
}

int visitConflictCgraph(void) {
  CconflictG.visited.clear(); //arborescence ==> once is enough 
  for (int v = 0; v < nveh; ++v) {
	cout << "ROOT: " << alpha(v) << " " << isRoot[v] << " (sad " << isSigmaAdj[v] << ")" << endl;
	if (isRoot[v])
	  CconflictG.DFS(v);
  }
  
  return 0;
}

bool checkAssumption01(int& countMissed, int& countCollisions ) {
  for (int v = 0; v < nveh; ++v)
	if (isInCCPath(v) && CconflictG.adj[v].size() >= 2)
	  if (countMissed < 0) 
		return false; 
	  else {
		++countMissed;
		if (countCollisions >=0) countCollisions += getCollisionsC(v);
	  }
  return (countMissed <= 0);
}

bool checkAssumption02(int& countMissed, int& countCollisions) {
  for (int k = 0; k < C.size(); ++k) {
	bool isBconfl = C[k].isEven && (omega(C[k].p) != C[k].c && omega(C[k].q) != C[k].c);
	if (!isBconfl) continue;
	int vp = C[k].p;
	int vq = C[k].q;
	if (!isInCCPath(vp)) continue; //not in a C-conflict path
	if (!isInCCPath(vq)) continue; //not in a C-conflict path
	if (CconflictG.adj[vp].size() == 0) continue; // first in a C-conflict path
	if (CconflictG.adj[vq].size() == 0) continue; // first in a C-conflict path
	if (isSigmaAdj[vp]) continue;
	if (isSigmaAdj[vq]) continue;
	if (countMissed < 0) 
	  return false; 
	else {
	  ++countMissed;
	  if (countCollisions >=0) countCollisions += checkNodeCollision(vp,vq);
	}
  }
  return (countMissed <= 0);
}

int getCollisionsC(int i_veh) {
  //i_veh is in a C conflict path and has two or more v_veh s.t. v_veh(i_veh)
  
  int numColl = 0;
  list<int>::iterator v; 
  for (v = CconflictG.adj[i_veh].begin(); v != CconflictG.adj[i_veh].end(); ++v) {
	int v_veh = *v;
	numColl += checkNodeCollision(i_veh,v_veh);
  }

  return numColl;
}

void printRoute(int v) {
  for (int t = 0; t < route8[v].size(); ++t) {
	cerr << "\t" << route8[v][t];
  }
}

int checkNodeCollision(int p, int q) {
  //cerr << "\nchecking " << alpha(p) << " vs " << alpha(q) << endl;
  int tp = buildRoute(p); //cerr << endl; printRoute(p); cerr << endl;
  int tq = buildRoute(q); //cerr << endl; printRoute(q); cerr << endl;
  for (int t = 0; t < tp && t < tq; ++t) {
	if (route8[p][t] == route8[q][t]) return 1;
  }
  return 0;
}

int buildRoute(int i) {
  //in these experiments, i is in a C-conflict path
  if (!isInCCPath(i)) {
	cerr << "Unexpected vehicle case!" << endl;
	char c; cin >> c;
	return -1;
  }
  if (route8.count(i) > 0) return route8[i].size();
  else if (CconflictG.adj[i].size() == 0) return buildRouteSdot1(i);
  else if (isSigmaAdj[i]) return buildRouteSdot2(i);
  else return buildRouteSdot3(i);
}

int buildRouteSdot1(int i) {
  //cerr << " " << alpha(i) << ":X.1 ";
  if (route8.count(i) == 0) {
	int c = alpha(i), r = 1;
	route8[i].push_back(positionEncode(c, r));
	if (isR(i)) {   //4.1
	  for (++r; r <= 7; ++r)
		route8[i].push_back(positionEncode(c, r));
	  --r;
	  for (++c; c <= omega(i); ++c)
		route8[i].push_back(positionEncode(c, r));
	  --c;
	  route8[i].push_back(positionEncode(c, ++r));
	}
	if (isL(i)) {   //5.1
	  route8[i].push_back(positionEncode(c, ++r));
	  for (++r; r <= 6; ++r)
		route8[i].push_back(positionEncode(c, r));
	  --r;
	  for (--c; c >= omega(i); --c)
		route8[i].push_back(positionEncode(c, r));
	  ++c;
	  for (++r; r <= 8; ++r)
		route8[i].push_back(positionEncode(c, r));
	}
  }
  return route8[i].size();
}

int buildRouteSdot2 (int i) {
//cerr << " " << alpha(i) << ":X.2 "; 
  if ( route8.count(i) == 0 ) {
		int c = alpha(i), r = 1;
		route8[i].push_back(positionEncode(c,r));
		if (isR(i)) {   //4.2
			for ( ++c ; c <= omega(i)-1; ++c ) 
				route8[i].push_back(positionEncode(c,r));
			--c;
			for ( ++r ;  r <= 5; ++r ) 
				route8[i].push_back(positionEncode(c,r));
			--r;
			route8[i].push_back(positionEncode(++c,r));
			for (  ++r ;  r <= 8; ++r ) 
				route8[i].push_back(positionEncode(c,r));
		}
		if (isL(i)) {   //5.2
			route8[i].push_back(positionEncode(c,++r));
			for ( --c; c >= omega(i)+1; --c ) 
				route8[i].push_back(positionEncode(c,r));
			++c;
			for ( ++r ;  r <= 4; ++r ) 
				route8[i].push_back(positionEncode(c,r));
			--r;
			route8[i].push_back(positionEncode(--c,r));
			for (  ++r ;  r <= 8; ++r ) 
				route8[i].push_back(positionEncode(c,r));
		} 
	} 
	return route8[i].size();
}

int buildRouteSdot3 (int i) {
	//cerr << " " << alpha(i) << ":X.3 ";
	if ( route8.count(i) == 0 ) {
		int v = rand() % CconflictG.adj[i].size();
		list<int>::iterator it = next(CconflictG.adj[i].begin(), v);
		v = *it;
		int c = alpha(i), r = 1;
		route8[i].push_back(positionEncode(c,r));
		if (isR(i)) {   //4.3
			for ( ++c ; c <= omega(v); ++c ) 
				route8[i].push_back(positionEncode(c,r));
			--c;
			for ( ++r ;  r <= 3; ++r ) 
				route8[i].push_back(positionEncode(c,r));
			--r;
			route8[i].push_back(positionEncode(++c,r));
			for ( ++r ;  r <= 5; ++r ) 
				route8[i].push_back(positionEncode(c,r));
			--r;
			for ( ++c ; c <= omega(i); ++c ) 
				route8[i].push_back(positionEncode(c,r));
			--c;
			for (  ++r ;  r <= 8; ++r ) 
				route8[i].push_back(positionEncode(c,r));
		}
		if (isL(i)) {   //5.3
			route8[i].push_back(positionEncode(c,++r));
			for ( --c; c >= omega(v); --c ) 
				route8[i].push_back(positionEncode(c,r));
			++c;
			for ( ++r ;  r <= 4; ++r ) 
				route8[i].push_back(positionEncode(c,r));
			--r;
			for ( --c; c >= omega(i)+1; --c ) 
				route8[i].push_back(positionEncode(c,r));
			++c;
			for ( ++r ;  r <= 6; ++r ) 
				route8[i].push_back(positionEncode(c,r));
			--r;
			route8[i].push_back(positionEncode(--c,r));
			for (  ++r ;  r <= 8; ++r ) 
				route8[i].push_back(positionEncode(c,r));
		} 
	} 
	return route8[i].size();
}

