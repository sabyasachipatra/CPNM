// Copyright © 2019 Sabyasachi Patra, IIIT Bhubaneswar, All rights reserved.
#ifndef SUBGRAPHTREE_H_
#define SUBGRAPHTREE_H_

#include <stdio.h>
#include <string>
#include <map>
#include <iostream>
#include "Graph.h"
#include "GraphIsomor.h"

class SubgraphTreeNode {
 public:
     SubgraphTreeNode();
     ~SubgraphTreeNode();
     int frequency;
     SubgraphTreeNode *zero, *one;
     void zeroFrequency();
     void incrementString(int pos, char *s);
     void setString(int pos, char *s, int f);
     int checkString(int pos, char *s);
     void addString(int pos, char *s, int f);
     void showFrequency(int pos, char *s);
     void populateMap(std::map<std::string, int> *m,
                      int size, int pos, char *s);
     int countSubgraphs();
     double countEmbeddings();
};

class SubgraphTree {
 public:
     SubgraphTreeNode *root;
     SubgraphTree();
     ~SubgraphTree();
     void zeroFrequency();
     void incrementString(char *s);
     void setString(char *s, int f);
     int checkString(char *s);
     void addString(char *s, int f);
     void showFrequency(int maxsize);
     void populateMap(std::map<std::string, int> *m, int maxsize);
     int countSubgraphs();
     double countEmbeddings();
};
#endif  // SUBGRAPHTREE_H_

