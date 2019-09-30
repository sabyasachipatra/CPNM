// Copyright © 2019 Sabyasachi Patra, IIIT Bhubaneswar, All rights reserved.
#ifndef GRAPH_H_
#define GRAPH_H_

#include <stdio.h>
#include <vector>
#include <list>
#include <map>
#include <iostream>
#include <algorithm>

class Graph {
 private:
     int numNodes;
     int numEdges;
     int *numNeighbours;
     bool **adjMatrix;
     int  **neighboursArray;
     std::vector<int> *neighbours;
     void initGraph();
     void deleteGraph();

 public:
     Graph();
     ~Graph();
     int numberNodes() {
         return numNodes;
     }
     int numberEdges() {
         return numEdges;
     }
     bool **adjacencyMatrix() {
         return adjMatrix;
     }
     void createGraph(int n);
     void addEdge(int a, int b);
     void rmEdge(int a, int b);
     bool hasEdge(int a, int b) {
         return adjMatrix[a][b];
     }
     bool isConnected(int a, int b) {
         return adjMatrix[a][b] || adjMatrix[b][a];
     }
     int numberNeighbours(int a) {
         return numNeighbours[a];
     }
     int *numberNeighbours() {
         return numNeighbours;
     }
     void sortNeighbours();
     void sortNeighboursArray();
     void makeNeighboursArray();
     void makeNeighboursVector();
     std::vector<int> *getNeighbours(int a) {
         return &neighbours[a];
     }
     int **getNeighboursArray() {
         return neighboursArray;
     }
     int *getNeighboursArray(int a) {
         return neighboursArray[a];
     }
     static int compareInt(const void *a, const void *b);
     static void readGraphFile(Graph *g, const char *s);
     static void readFeatureFile(Graph *g, float **FMat, int nf, const char *s);
     static int computeNumFeature(Graph *g, const char *s);
     static void strToGraph(Graph *g, const char *s, int size);
};
#endif  // GRAPH_H_
