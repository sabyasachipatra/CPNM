// Copyright © 2019 Sabyasachi Patra, IIIT Bhubaneswar, All rights reserved.
#include "Graph.h"

Graph::Graph() {
    features.clear();
    initGraph();
}

Graph::~Graph() {
    deleteGraph();
}

void Graph::initGraph() {
    numNodes = 0;
    numEdges = 0;
    adjMatrix = NULL;
    numNeighbours = NULL;
    neighbours = NULL;
    neighboursArray = NULL;
}

void Graph::deleteGraph() {
    int i;
    if (adjMatrix != NULL) {
        for (i=0; i < numNodes; i++)
            if (adjMatrix[i] != NULL)
                delete[] adjMatrix[i];
        delete[] adjMatrix;
    }
    if (numNeighbours != NULL)
        delete[] numNeighbours;
    if (neighbours != NULL)
        delete[] neighbours;
    if (neighboursArray != NULL) {
        for (i=0; i < numNodes; i++)
            if (neighboursArray[i] != NULL)
                delete[] neighboursArray[i];
        delete[] neighboursArray;
    }
}

void Graph::createGraph(int n) {
    int i, j;
    numNodes = n;
    deleteGraph();
    adjMatrix = new bool*[n];
    for (i=0; i < n; i++)
        adjMatrix[i] = new bool[n];
    neighbours = new std::vector<int>[n];
    numNeighbours = new int[n];
    numEdges = 0;
    for (i=0; i < numNodes; i++) {
        numNeighbours[i] = 0;
        neighbours[i].clear();
        for (j=0; j < numNodes; j++)
            adjMatrix[i][j] = false;
    }
}

void Graph::addEdge(int a, int b) {
    if (adjMatrix[a][b])
        return;
    adjMatrix[a][b] = true;
    adjMatrix[b][a] = true;
    numEdges++;
    neighbours[a].push_back(b);
    numNeighbours[a]++;
    neighbours[b].push_back(a);
    numNeighbours[b]++;
}

void Graph::rmEdge(int a, int b) {
    int i, s;
    if (!adjMatrix[a][b])
        return;
    adjMatrix[a][b] = false;
    adjMatrix[b][a] = false;
    numEdges--;
    // delete b from nighbours[a]
    s = neighbours[a].size();
    for (i=0; i < s; i++)
        if (neighbours[a][i] == b)
            break;
    if (i < s)
        neighbours[a].erase(neighbours[a].begin()+i);
    numNeighbours[a]--;
    // delete a from nighbours[b]
    s = neighbours[b].size();
    for (i=0; i < s; i++)
        if (neighbours[b][i] == a)
            break;
    if (i < s)
        neighbours[b].erase(neighbours[b].begin()+i);
    numNeighbours[b]--;
}

void Graph::sortNeighbours() {
    int i;
    for (i=0; i < numNodes; i++)
        sort(neighbours[i].begin(), neighbours[i].begin()+neighbours[i].size());
}

void Graph::sortNeighboursArray() {
  int i;
  for (i=0; i < numNodes; i++)
    qsort(neighboursArray[i], numNeighbours[i], sizeof(int), Graph::compareInt);
}

int Graph::compareInt(const void *a, const void *b) {
    return (*((int *)b)) - (*((int *)a));
    // return (*reinterpret_cast<int *>(b)) - (*reinterpret_cast<int *>(a));
}

void Graph::makeNeighboursArray() {
    int i, j;
    std::vector<int>:: iterator ii;
    neighboursArray = new int*[numNodes];
    for (i=0; i < numNodes; i++) {
        neighboursArray[i] = new int[neighbours[i].size()];
        for (ii=neighbours[i].begin(), j=0; ii != neighbours[i].end();
             ++ii, j++)
            neighboursArray[i][j] = *ii;
    }
}

void Graph::makeNeighboursVector() {
    int i, j;
    std::vector<int>:: iterator ii;
    for (i=0; i < numNodes; i++)
        for (j=0; j < numNeighbours[i]; j++)
            neighbours[i].push_back(neighboursArray[i][j]);
}

void Graph::readGraphFile(Graph *g, const char *s) {
    FILE *f = fopen(s, "r");
    if (!f) {
        std::cout << "invalid graph file" << std::endl;
        exit(1);
    }
    int i, a, b, c, size, max;
    std::vector<int> va, vb;
    max = 1;
    size = 0;
    while (fscanf(f, "%d %d %d", &a, &b, &c) == 3) {
        va.push_back(a);
        vb.push_back(b);
        if (a > max) max = a;
        if (b > max) max = b;
        size++;
    }
    fclose(f);
    max++;
    g->createGraph(max);
    for (i=0; i < size; i++) {
        if (va[i] == vb[i]) {
            printf("Self-Loop on %d ignored\n", va[i]);
            continue;  // discard self loops!
        }
        if (g->hasEdge(va[i], vb[i])) {
            printf("Repeated connection: %d %d\n", va[i], vb[i]);
        } else {
            g->addEdge(va[i], vb[i]);
        }
    }
    va.clear();
    vb.clear();
}

void Graph::strToGraph(Graph *g, const char *s, int size) {
    int i, j;
    g->createGraph(size);
    for (i=0; i < size; i++)
        for (j=0; j < size; j++)
            if (s[i*size+j] == '1')
                g->addEdge(i, j);
}

// Randomize 'g' network with number of exchanges per edge and number of tries
// attempts per edge
void Graph::
randomGraph(Graph *g, int num_exchanges, int num_tries, int *nswap) {
    int i, j, k, l, edges, nodes = g->numberNodes();
    int a = 0, b = 0, c = 0, d = 0, n;
    std::vector<int> *u, *v;
    for (n=0; n < num_exchanges; n++) {
        for (i=0; i < nodes; i++) {
            u = g->getNeighbours(i);
            edges = u->size();
            a = i;
            for (j=0; j < edges; j++) {
                b = (*u)[j];
                for (k=0; k < num_tries; k++) {
                    c = getRandom(0, nodes-1);
                    if (a == c || b == c || g->hasEdge(c, b)) continue;
                    l = g->numberNeighbours(c);
                    if (l == 0) continue;
                    v = g->getNeighbours(c);
                    d = getRandom(1, l);
                    d = (*v)[d-1];
                    if (a == d || b == d || g->hasEdge(a, d)) continue;
                    break;
                }
                if (k < num_tries) {  // Found an edge to swap!
                    g->rmEdge(a, b);  g->rmEdge(c, d);
                    g->addEdge(a, d); g->addEdge(c, b);
                    *nswap = *nswap+1;
                    // cout << *nswap << endl;
                }
            }
        }
    }
}
// Pseudo-Random number between 'a' and 'b'
int Graph::getRandom(int a, int b) {
    srand(time(0));
    double aux = rand() / ((double)RAND_MAX);
    // double aux = rand_r() / static_cast<double>(RAND_MAX);
    return a + aux*(b-a+1);
}
