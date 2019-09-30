// Copyright © 2019 Sabyasachi Patra, IIIT Bhubaneswar, All rights reserved.
#ifndef EXPANSIONTREE_H_
#define EXPANSIONTREE_H_

#include <vector>
#include <list>
#include <utility>
#include "Graph.h"
#include "GraphIsomor.h"
#include "SubgraphTree.h"

typedef struct {
    char *s;
    int freq;
    double avgf_rand;
    double devf_rand;
    double z_score;
} ResultType;

class ExpansionTreeNode {
 public:
     ExpansionTreeNode(int d, int v, int e);  // Create e-tree node
     ~ExpansionTreeNode();
     int depth;  // Depth of e-tree node
     int numvertex;  // Number of verices in e-tree node
     int numedge;  // Number of edges in e-tree node
     int frequency;  // number of embeddings
     char *canstr;  // node in canonical order
     // List of symmetry breaking conditions
     std::list< std::pair<int, int> > cond;
     std::list<ExpansionTreeNode *> child;  // List of child nodes
     std::list< std::pair<int, int> > edgeDiff;
     std::list<int> mapFlag;
     std::list< std::vector<int> > map;
     std::list< std::vector<int> > emb;
     void insertRootNode(int d, int v, int e, SubgraphTree *isomorSG);
     void insertConditions(std::list< std::pair<int, int> > *conditions);
     void printTree(int *sl);
     void printGraph(int n);
     void printCanonical(int numvertex, char* canstr);
     void expandTree(int size, SubgraphTree *isomorSG);
     int insertNode(int d, int n, int e, const char *s, int a, int b,
                    SubgraphTree *ig);
     void census(int size, Graph *g, SubgraphTree *sg);
     void treeCensus(int size, Graph *g, SubgraphTree *sg);
     void graphCensus(int size, Graph *g, SubgraphTree *sg);
     void basicTreeCensus(int size, Graph *g, SubgraphTree *sg);
     void printEmbeddings(int size);
     void inducedFreq(int size, Graph *g, SubgraphTree *sg, int th);
     void computeFeatureMatrix(int size, Graph *g, int th);
};

class ExpansionTree {
 private:
     ExpansionTreeNode *root;
 public:
     ExpansionTree();
     ~ExpansionTree();
     void create(int size, SubgraphTree *isomorSG);
     void census(int size, Graph *g, SubgraphTree *sg);
     void printEmbeddings(int size);
     void inducedFreq(int size, Graph *g, SubgraphTree *sg, int th);
     void computeFeatureMatrix(int size, Graph *g, int th, char *feature_file);
};
#endif  // EXPANSIONTREE_H_
