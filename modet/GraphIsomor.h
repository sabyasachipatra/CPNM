// Copyright © 2019 Sabyasachi Patra, IIIT Bhubaneswar, All rights reserved.
#ifndef GRAPHISOMOR_H_
#define GRAPHISOMOR_H_

#include <vector>
#include <list>
#include <utility>
#include "Graph.h"
#include "nauty/nauty.h"

#define MOSIZE 15
#define WORKSIZE MOSIZE*160

class GraphIsomor {
 private:
     static setword workspace[WORKSIZE];
     static int n, m, lab[MOSIZE], ptn[MOSIZE], orbits[MOSIZE];
     static set *gv;
     static graph g[MOSIZE*MOSIZE];
     static void findAutomorphisms(Graph *G, std::vector<char *> *vv);
     static void isomorphicExtensions();

 public:
     static void initNauty(int size);
     static void finishNauty();
     static void canonicalOrder(const char *in, char *can, int size);
     static void canonicalOrderWithMap(const char *i, char *c, int s, int* m2);
     static void symmetryConditions(Graph *g,
                                    std::list< std::pair<int, int> > *cond);
};

class GMap {
 public:
     int numf, numr;
     int *f;
     int *r;
     GMap(int nf, int nr);
     ~GMap();
     void add(int n, int v) {
         f[n] = v;
         r[v] = n;
     }
     void remove(int n) {
         r[f[n]] = -1;
         f[n] = -1;
     }
};
#endif  // GRAPHISOMOR_H_
