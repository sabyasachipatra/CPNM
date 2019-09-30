// Copyright © 2019 Sabyasachi Patra, IIIT Bhubaneswar, All rights reserved.
#ifndef DET_H_
#define DET_H_

#include <vector>
#include <list>
#include <utility>
#include "Graph.h"
#include "GraphIsomor.h"
#include "SubgraphTree.h"

class DETNode {
 public:
     DETNode(int d, int v, int e);  // Create e-tree node with depth 'd'
     ~DETNode();
     int depth;  // Depth of e-tree node
     int numvertex;  // Number of verices in e-tree node
     int numedge;  // Number of edges in e-tree node
     int frequency;  // number of embeddings
     int disjointFrequency;  // number of isjoint embeddings
     char *canstr;  // node in canonical order
     // List of symmetry breaking conditions
     std::list< std::pair<int, int> > cond;
     std::list<DETNode *> child;  // List of child nodes
     std::list< std::pair<int, int> > edgeDiff;
     std::list<int> mapFlag;
     std::list< std::vector<int> > map;
     std::list< std::vector<int> > emb;
     std::list< std::vector<int> > disjointEmb;
     void insertRootNode(int d, int v, int e, SubgraphTree *isomorSG);
     void insertConditions(std::list< std::pair<int, int> > *conditions);
     void printTree(int *sl);
     void printGraph(int n);
     void printCanonical(int numvertex, char* canstr);
     int insertNode(int d, int n, int e, const char *s, int a, int b,
                    SubgraphTree *isomorSG);
     void census(int s, Graph *g, SubgraphTree *sg, SubgraphTree *ig, int th);
     void treeCensus(int s, Graph *g, SubgraphTree *sg,
                     SubgraphTree *ig, int th);
     void graphCensus(int s, Graph *g, SubgraphTree *sg,
                      SubgraphTree *ig, int th);
     void basicTreeCensus(int size, Graph *g);
     void printEmbeddings(int size);
     void inducedFreq(int size, Graph *g, SubgraphTree *dsg, int th);
     void mis(int size, Graph *g, SubgraphTree *dsg);
     bool hasOverlap(int size, int **embArray, int a, int b, char *canstr);
     void buildMinHeap(int hs, int *H, int *hiArray, int *niArray);
     void minHeapify(int hs, int *H, int i, int *hiArray, int *niArray);
     void decreaseKey(int *H, int i, int *hiArray, int *niArray);
     int deleteHeap(int *hs, int *H, int index, int *hiArray, int *niArray);
};

class DET {
 private:
     DETNode *root;
 public:
     DET();
     ~DET();
     void census(int s, Graph *g, SubgraphTree *sg, SubgraphTree *ig, int th);
     void printEmbeddings(int size);
     void inducedFreq(int size, Graph *g, SubgraphTree *sg, int th);
};
#endif  // DET_H_
