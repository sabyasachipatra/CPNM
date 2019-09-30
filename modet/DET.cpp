// Copyright © 2019 Sabyasachi Patra, IIIT Bhubaneswar, All rights reserved.
#include "DET.h"

#define MOSIZE 15  // Maximum motif size
#define EMBSIZE 10000  // Maximum number of embeddings

DETNode::DETNode(int d, int v, int e) {
    depth = d;
    numvertex = v;
    numedge = e;
    frequency = 0;
    disjointFrequency = 0;
    canstr = NULL;
    cond.clear();
    child.clear();
    emb.clear();
    disjointEmb.clear();
}

DETNode::~DETNode() {
    if (canstr != NULL)
        delete[] canstr;
}

DET::DET() {
    root = new DETNode(0, 0, 0);
}

DET::~DET() {
    delete root;
}

void DETNode::insertRootNode(int d, int v, int e, SubgraphTree *isomorSG) {
    char *str = "001001110";  // root node
    Graph *sg = new Graph();
    Graph::strToGraph(sg, str, v);
    sg->makeNeighboursArray();
    std::list< std::pair<int, int> > *conditions =
    new std::list< std::pair<int, int> >;
    GraphIsomor::symmetryConditions(sg, conditions);
    DETNode *et = new DETNode(d, v, e);
    et->canstr = new char[strlen(str)+1];
    snprintf(et->canstr, strlen(str)+1, "%s\n", str);
    et->insertConditions(conditions);
    child.push_back(et);
    isomorSG->setString(str, 0);
    delete conditions;
    delete sg;
}

int DETNode::insertNode(int d, int n, int e, const char *s, int a, int b,
                        SubgraphTree *isomorSG) {
    // char str[n*n+1];
    char str[MOSIZE*MOSIZE+1];
    int i;
    // int map2[n];
    int map2[MOSIZE];
    GraphIsomor::canonicalOrderWithMap(s, str, n, map2);
    int mflag = 0;
    for (i=0; i < n; i++)
        if (map2[i] != i) mflag=1;
    std::list<DETNode *>::iterator ii;
    std::vector<int>::iterator kk;
    std::list< std::vector<int> >::iterator mm;
    for (ii=child.begin(); ii != child.end(); ii++) {
        if (!strcmp(str, (*ii)->canstr)) {
            std::pair<int, int> p;
            p.first = a;
            p.second = b;
            (*ii)->edgeDiff.push_back(p);
            if (mflag == 1)
                (*ii)->mapFlag.push_back(1);
            else
                (*ii)->mapFlag.push_back(0);
            std::vector<int> mapTo;
            for (i=0; i < n; i++)
                mapTo.push_back(map2[i]);
            (*ii)->map.push_back(mapTo);
            return 0;
        }
    }
    if (isomorSG->checkString(str) == 1) return 0;
    isomorSG->setString(str, 0);
    Graph *sg = new Graph();
    Graph::strToGraph(sg, str, n);
    sg->makeNeighboursArray();
    std::list< std::pair<int, int> > *conditions =
    new std::list< std::pair<int, int> >;
    GraphIsomor::symmetryConditions(sg, conditions);
    DETNode *et = new DETNode(d, n, e);
    et->canstr = new char[strlen(str)+1];
    snprintf(et->canstr, strlen(str)+1, "%s\n", str);
    et->insertConditions(conditions);
    std::pair<int, int> p;
    p.first = a;
    p.second = b;
    et->edgeDiff.push_back(p);
    if (mflag == 1)
        et->mapFlag.push_back(1);
    else
        et->mapFlag.push_back(0);
    std::vector<int> mapTo;
    for (i=0; i < n; i++)
        mapTo.push_back(map2[i]);
    et->map.push_back(mapTo);
    child.push_back(et);
    delete conditions;
    delete sg;
    return 1;
}

// Insert Symmetry conditions pertaining to this node
void DETNode::insertConditions(std::list< std::pair<int, int> > *conditions) {
    if (!conditions->size()) return;
    std::list< std::pair<int, int> >::iterator jj, kk;
    std::pair<int, int> p;
    for (jj=conditions->begin(); jj != conditions->end(); jj++)
        for (kk=conditions->begin(); kk != conditions->end(); kk++)
            if (jj->second == kk->first) {
                p.first  = jj->first;
                p.second = kk->second;
                conditions->remove(p);
            }
    for (jj=conditions->begin(); jj != conditions->end(); jj++)
        cond.push_back(*jj);
}

void DETNode::printTree(int *sl) {
    int maxedge, v;

    std::list<DETNode *>::iterator ii;
    for (ii=child.begin(); ii != child.end(); ii++) {
        *sl = *sl+1;
        (*ii)->printGraph(*sl);
        v = (*ii)->numvertex;
        maxedge = (v * (v-1))/2;
        if ((*ii)->numedge <= maxedge)
            (*ii)->printTree(sl);
    }
}

void DETNode::printGraph(int n) {
    printf("%d : ", n);
    printCanonical(numvertex, canstr);
}

void DETNode::printCanonical(int n, char* str) {
    int i = 0;
    while (*str != '\0') {
        printf("%c", *str);
        str++;
        i++;
        if (i == n) {
            i = 0;
            printf("  ");
        }
    }
    printf("\n");
}

void DET::printEmbeddings(int size) {
    root->printEmbeddings(size);
}

void DETNode::printEmbeddings(int size) {
    std::list<DETNode *>::iterator ii;
    for (ii=child.begin(); ii != child.end(); ii++) {
        if ((*ii)->frequency <= 0) continue;
        if ((*ii)->numvertex == size) {
            std::cout << std::endl << (*ii)->canstr << " : ";
            std::cout << (*ii)->disjointFrequency << std::endl;
            std::list< std::vector<int> >::iterator jj;
            for (jj=(*ii)->disjointEmb.begin();
                 jj != (*ii)->disjointEmb.end(); jj++) {
                std::vector<int>::iterator kk;
                for (kk=jj->begin(); kk != jj->end(); kk++)
                    std::cout << *kk << "  ";
                std::cout << std::endl;
            }
        }
        int maxedge = ( (*ii)->numvertex * ((*ii)->numvertex-1) )/2;
        if ((*ii)->frequency > 0 && (*ii)->numedge <= maxedge)
            (*ii)->printEmbeddings(size);
    }
}

void DET::census(int size, Graph *g, SubgraphTree *dsg,
                 SubgraphTree *isomorSG, int th) {
    root->insertRootNode(1, 3, 2, isomorSG);
    root->census(size, g, dsg, isomorSG, th);
}

void DETNode::census(int size, Graph *g, SubgraphTree *dsg,
                     SubgraphTree *isomorSG, int th) {
    int n = g->numberNodes();
    std::list<DETNode *>::iterator ii;
    for (ii=child.begin(); ii != child.end(); ii++) {
        (*ii)->basicTreeCensus(size, g);
        // cout << "orig_frequency= " << (*ii)->frequency << endl;
        (*ii)->mis(3, g, dsg);
        // cout << "disjoint_frequency= " << (*ii)->disjointFrequency << endl;
        if (size == 3 && (*ii)->disjointFrequency >= th)
            dsg->setString((*ii)->canstr, (*ii)->disjointFrequency);
        if ((*ii)->disjointFrequency >= th) {
            if ((*ii)->numvertex < size)
                (*ii)->treeCensus(size, g, dsg, isomorSG, th);
            else
                (*ii)->graphCensus(size, g, dsg, isomorSG, th);
        }
    }
}

void DETNode::basicTreeCensus(int size, Graph *g) {
    int i, j, k;
    bool **adjMat = g->adjacencyMatrix();
    int **fastnei = g->getNeighboursArray();
    int *numnei = g->numberNeighbours();
    for (i=0; i < g->numberNodes(); i++) {
        for (j=0; j< numnei[i]; j++) {
            for (k=j+1; k < numnei[i]; k++) {
                std::vector<int> em;
                if (fastnei[i][j] < fastnei[i][k]) {
                    em.push_back(fastnei[i][j]);
                    em.push_back(fastnei[i][k]);
                } else {
                    em.push_back(fastnei[i][k]);
                    em.push_back(fastnei[i][j]);
                }
                em.push_back(i);
                frequency++;
                emb.push_back(em);
            }
        }
    }
}

void DETNode::treeCensus(int size, Graph *g, SubgraphTree *dsg,
                         SubgraphTree *isomorSG, int th) {
    int i, j, k, l, a, b, flag;
    int numv;
    std::list<DETNode *>::iterator ii;
    std::list< std::vector<int> >::iterator jj;
    std::vector<int>::iterator kk;
    std::list< std::pair<int, int> >::iterator ll;
    std::list< std::pair<int, int> >::iterator dd;
    std::list< std::vector<int> >::iterator mm;
    std::list<int>::iterator ff;
    int n = g->numberNodes();
    bool **adjMat = g->adjacencyMatrix();
    int **fastnei = g->getNeighboursArray();
    int *numnei = g->numberNeighbours();

    int n1, ret;
    char *origstr, *str, *newstr;
    if (frequency == 0) return;

    // Expand by adding a vertex
    origstr = new char[numvertex*numvertex+1];
    snprintf(origstr, strlen(canstr)+1, "%s\n", canstr);
    n1 = numvertex + 1;
    str = new char[n1*n1+1];
    // add 0 at the end of each row
    for (i=0; i < numvertex; i++) {
        for (j=0; j < numvertex; j++)
            str[i*n1+j] = origstr[i*numvertex+j];
        str[i*n1+j] = '0';
    }
    // add a 0 row
    for (j=0; j < n1; j++)
        str[i*n1+j] = '0';
    str[i*n1+j] = '\0';
    delete[] origstr;
    newstr = new char[n1*n1+1];
    flag = 0;
    for (i=0; i < numvertex; i++) {
        snprintf(newstr, strlen(str)+1, "%s\n", str);
        newstr[i*n1+numvertex]='1';
        newstr[n1*(n1-1)+i]='1';
        ret = insertNode(depth+1, numvertex+1, numedge+1, newstr, i,
                         numvertex, isomorSG);
        if (ret) flag = 1;
    }
    delete[] str;
    delete[] newstr;
    if (flag == 0) return;

    for (ii=child.begin(); ii != child.end(); ii++) {
        numv = (*ii)->numvertex;
        // for each child traverse embeddings of parent node
         for (jj=disjointEmb.begin(); jj != disjointEmb.end(); jj++) {
            // int emp[numv];  // emp: parent embedding
            int emp[MOSIZE];  // emp: parent embedding
            for (kk=jj->begin(), l=0; kk != jj->end(); kk++, l++)
                emp[l] = *kk;
            for (dd=(*ii)->edgeDiff.begin(), ff=(*ii)->mapFlag.begin(),
                 mm=(*ii)->map.begin(); dd != (*ii)->edgeDiff.end();
                 dd++, ff++, mm++) {
                a = (*dd).first;
                for (i=0; i < numnei[emp[a]]; i++) {
                    b = fastnei[emp[a]][i];
                    // exclude neighbor which is part of emp
                    for (j=0; j < numv-1; j++)
                        if (b == emp[j]) break;
                    if (j != numv-1) continue;

                    emp[numv-1] = b;
                    if (*ff == 0) {
                        std::vector<int> em;
                        for (j=0; j < numv; j++)
                            em.push_back(emp[j]);
                        (*ii)->emb.push_back(em);
                        (*ii)->frequency++;
                    } else {
                        // int map2[numv];
                        int map2[MOSIZE];
                        for (kk=mm->begin(), j=0; kk != mm->end(); kk++, j++) {
                            map2[j] = *kk;
                        }
                        std::vector<int> em;
                        for (j=0; j < numv; j++)
                            em.push_back(emp[map2[j]]);
                        (*ii)->emb.push_back(em);
                        (*ii)->frequency++;
                    }
                }
            }
        }
        if ((*ii)->frequency <= 0) continue;
        // cout << "orig_frequency= " << (*ii)->frequency << endl;
        (*ii)->mis((*ii)->numvertex, g, dsg);
        // cout << "disjoint_frequency= " << (*ii)->disjointFrequency << endl;
        if ((*ii)->disjointFrequency >= th && numv == size)
            dsg->setString((*ii)->canstr, (*ii)->disjointFrequency);
        if ((*ii)->disjointFrequency >= th) {
            if (numv < size)
                (*ii)->treeCensus(size, g, dsg, isomorSG, th);
            else
                (*ii)->graphCensus(size, g, dsg, isomorSG, th);
        }
    }
}

void DETNode::graphCensus(int size, Graph *g, SubgraphTree *dsg,
                          SubgraphTree *isomorSG, int th) {
    int i, j, k, l, a, b, flag;
    int numv, maxedge;
    std::list<DETNode *>::iterator ii;
    std::list< std::vector<int> >::iterator jj;
    std::vector<int>::iterator kk;
    std::list< std::pair<int, int> >::iterator ll;
    std::list< std::pair<int, int> >::iterator dd;
    std::list< std::vector<int> >::iterator mm;
    std::list<int>::iterator ff;
    int n = g->numberNodes();
    bool **adjM = g->adjacencyMatrix();

    int ret;
    char *origstr, *str, *newstr;

    if (frequency == 0) return;
    // Expand by adding an edge
    str = new char[numvertex*numvertex+1];
    newstr = new char[numvertex*numvertex+1];
    snprintf(str, strlen(canstr)+1, "%s\n", canstr);
    flag = 0;
    for (i=1; i < numvertex; i++) {
        for (j=0; j < i; j++) {
            if (str[i*numvertex+j] == '0') {
                snprintf(newstr, strlen(str)+1, "%s\n", str);
                newstr[i*numvertex+j] = '1';
                newstr[j*numvertex+i] = '1';
                ret = insertNode(depth+1, numvertex, numedge+1,
                                 newstr, i, j, isomorSG);
                if (ret) flag=1;
            }
        }
    }
    delete[] str;
    delete[] newstr;
    if (flag == 0) return;
    for (ii=child.begin(); ii != child.end(); ii++) {
        numv = (*ii)->numvertex;
        maxedge = (numv * (numv-1))/2;
         for (jj=disjointEmb.begin(); jj != disjointEmb.end(); jj++) {
            // int emp[numv];  // emp: parent embedding
            int emp[MOSIZE];  // emp: parent embedding
            for (kk=jj->begin(), l=0; kk != jj->end(); kk++, l++)
                emp[l] = *kk;
            for (dd=(*ii)->edgeDiff.begin(), ff=(*ii)->mapFlag.begin(),
                 mm=(*ii)->map.begin(); dd != (*ii)->edgeDiff.end();
                 dd++, ff++, mm++) {
                a = (*dd).first;
                b = (*dd).second;
                if (adjM[emp[a]][emp[b]]) {
                    if (*ff == 0) {
                        (*ii)->emb.push_back(*jj);
                        (*ii)->frequency++;
                    } else {
                        // int map2[numv];
                        int map2[MOSIZE];
                        for (kk=mm->begin(), j=0; kk != mm->end(); kk++, j++)
                            map2[j] = *kk;
                        std::vector<int> em;
                        for (j=0; j < numv; j++)
                            em.push_back(emp[map2[j]]);
                        (*ii)->emb.push_back(em);
                        (*ii)->frequency++;
                    }
                }
            }
        }
        // cout << "orig_frequency= " << (*ii)->frequency << endl;
        (*ii)->mis((*ii)->numvertex, g, dsg);
        // cout << "disjoint_frequency= " << (*ii)->disjointFrequency << endl;
        if ((*ii)->disjointFrequency >= th)
            dsg->setString((*ii)->canstr, (*ii)->disjointFrequency);
        if ((*ii)->disjointFrequency >= th && (*ii)->numedge <= maxedge)
            (*ii)->graphCensus(size, g, dsg, isomorSG, th);
    }
}

void DET::inducedFreq(int size, Graph *g, SubgraphTree *sg, int th) {
    root->inducedFreq(size, g, sg, th);
}

void DETNode::inducedFreq(int size, Graph *g, SubgraphTree *dsg, int th) {
    std::list<DETNode *>::iterator ii;
    int i, j, k;
    for (ii=child.begin(); ii != child.end(); ii++) {
        if ((*ii)->frequency <= 0) continue;
        if ((*ii)->frequency > th && (*ii)->numvertex == size) {
            int freq = 0;
            std::list< std::vector<int> >::iterator jj;
            for (jj=(*ii)->disjointEmb.begin();
                 jj != (*ii)->disjointEmb.end(); jj++) {
                std::vector<int>::iterator kk;
                // int a[size];
                int a[MOSIZE];
                for (kk=jj->begin(), j=0; kk != jj->end(); kk++, j++)
                    a[j] = *kk;
                int flag = 0;
                for (j=0; j < size && flag == 0; j++) {
                    for (k=j+1; k < size; k++) {
                        if ((*ii)->canstr[j*size+k] == '0' &&
                            g->hasEdge(a[j], a[k])) {
                            flag = 1;
                            break;
                        }
                    }
                }
                if (flag == 0) freq++;
            }
            (*ii)->frequency = freq;
            if (freq > th ) dsg->setString((*ii)->canstr, freq);
        }
        int maxedge = ((*ii)->numvertex * ((*ii)->numvertex-1))/2;
        if ((*ii)->frequency > 0 && (*ii)->numedge <= maxedge)
            (*ii)->inducedFreq(size, g, dsg, th);
    }
}

void DETNode::mis(int size, Graph *g, SubgraphTree *dsg) {
    int i, j, k, l, nodeIndex, heapIndex;
    int embsize = emb.size();
    int **embArray;
    embArray = new int*[embsize];
    for (i=0; i < embsize; i++)
        embArray[i]= new int[size];
    // populate emb array
    std::list< std::vector<int> >::iterator ii;
    for (ii=emb.begin(), i=0; ii != emb.end(); ii++, i++) {
        std::vector<int>::iterator jj;
        for (jj=ii->begin(), j=0; jj != ii->end(); jj++, j++)
            embArray[i][j] =* jj;
    }
    // prepare overlap graph
    Graph *og;
    og = new Graph();
    og->createGraph(embsize);
    for (i=0; i < embsize; i++)
        for (j=i+1; j < embsize; j++)
            if (hasOverlap(size, embArray, i, j, canstr) == true)
                og->addEdge(i, j);
    // sort neighbours
    og->sortNeighbours();
    og->makeNeighboursArray();
    int **fastnei = og->getNeighboursArray();
    int *numnei = og->numberNeighbours();
    // Create Min Heap and index array
    int heapSize = embsize;
    int Heap[heapSize];
    // int heapIndexArray[heapSize];  // save heap index of each node
    // int nodeIndexArray[heapSize];  // save node index of each heap element
    int heapIndexArray[EMBSIZE];  // save heap index of each node
    int nodeIndexArray[EMBSIZE];  // save node index of each heap element
    // Construct degree array
    for (i=0; i < og->numberNodes(); i++) {
        Heap[i] = og->numberNeighbours(i);
        heapIndexArray[i] = i;
        nodeIndexArray[i] = i;
    }
    buildMinHeap(heapSize, Heap, heapIndexArray, nodeIndexArray);
    while (heapSize > 0) {
        // delete root node
        // i= node index of root element
        i = deleteHeap(&heapSize, Heap, 0, heapIndexArray, nodeIndexArray);
        std::vector<int> em;
        for (j=0; j < size; j++)
            em.push_back(embArray[i][j]);
        disjointEmb.push_back(em);
        disjointFrequency++;
        std::vector<int> neighb;
        std::list< std::vector<int> > neighbNeighb;
        for (j=0; j < numnei[i]; j++) {
            neighb.push_back(fastnei[i][j]);
            std::vector<int> neighbj;
            int ii = fastnei[i][j];
            for (k=0; k < numnei[ii]; k++) {
                if (fastnei[ii][k] == i) continue;
                neighbj.push_back(fastnei[ii][k]);
            }
            neighbNeighb.push_back(neighbj);
        }
        std::vector<int>::iterator ii;
        std::vector<int>::iterator ni;
        std::list< std::vector<int> >::iterator jj;
        // Delete Neighbors
        for (ii=neighb.begin(); ii != neighb.end(); ii++) {
            heapIndex = heapIndexArray[*ii];
            deleteHeap(&heapSize, Heap, heapIndex,
                       heapIndexArray, nodeIndexArray);
        }
        // Decrease the degree of neighbor's neighbor
        for (jj=neighbNeighb.begin(); jj != neighbNeighb.end(); jj++) {
            for (ii=(*jj).begin(); ii != (*jj).end(); ii++) {
                heapIndex = heapIndexArray[*ii];
                Heap[heapIndex] = Heap[heapIndex]-1;
                decreaseKey(Heap, heapIndex, heapIndexArray, nodeIndexArray);
            }
        }
        // Delete Neighbors edge
        for (ii=neighb.begin(); ii != neighb.end(); ii++)
            og->rmEdge(i, *ii);
        // Delete neighbor's neighbor edge
        for (jj=neighbNeighb.begin(), ni=neighb.begin();
             jj != neighbNeighb.end(); jj++, ni++)
            for (ii=(*jj).begin(); ii != (*jj).end(); ii++)
                og->rmEdge(*ni, *ii);
        og->sortNeighbours();
        og->makeNeighboursArray();
        fastnei = og->getNeighboursArray();
        numnei = og->numberNeighbours();
    }
    for (i=0; i < embsize; i++)
        delete embArray[i];
    delete embArray;

    delete og;
}

bool DETNode::hasOverlap(int size, int **embArray,
                         int a, int b, char *canstr) {
    int i, j, flag;
    // check vertex overlap
    flag = 0;
    for (i=0; i < size && flag == 0; i++) {
        for (j=0; j < size; j++) {
            if (embArray[a][i] == embArray[b][j]) {
                flag = 1;
                break;
            }
        }
    }
    if (flag == 0) return false;
    // check edge overlap
    bool M[size][size];
    int k = 0, k1, k2;
    for (i=0; i < size; i++) {
        for (j=0; j < size; j++) {
            if (canstr[k] == '0')
                M[i][j] = false;
            else
                M[i][j] = true;
            k++;
        }
    }
    flag = 0;
    for (i=0; i < size && flag == 0; i++) {
        for (j=0; j < size && flag == 0; j++) {
            if (embArray[a][i] != embArray[b][j]) continue;
            for (k1=0; k1 < size && flag == 0; k1++) {
                if (M[i][k1]) {
                    for (k2=0; k2 < size; k2++) {
                        if (M[j][k2] && embArray[a][k1] == embArray[b][k2]) {
                            flag = 1;
                            break;
                        }
                    }
                }
            }
        }
    }
    if (flag == 1) return true;
    return false;
}

void DETNode::buildMinHeap(int heapSize, int *Heap,
                           int *heapIndexArray, int *nodeIndexArray) {
    int i;
    for (i = heapSize/2; i >= 0; i--)
        minHeapify(heapSize, Heap, i, heapIndexArray, nodeIndexArray);
}

void DETNode::minHeapify(int heapSize, int *Heap, int i,
                         int *heapIndexArray, int *nodeIndexArray) {
    int temp;
    int min = i;
    int l = 2*i+1;
    int r = 2*i+2;
    if (l < heapSize && Heap[l] < Heap[min])
        min = l;
    if (r < heapSize && Heap[r] < Heap[min])
        min = r;
    if (min != i) {
        // exchange index array
        temp = nodeIndexArray[i];
        nodeIndexArray[i] = nodeIndexArray[min];
        nodeIndexArray[min] = temp;
        heapIndexArray[nodeIndexArray[i]] = i;
        heapIndexArray[nodeIndexArray[min]] = min;
        // exchange Heap[i] with Heap[min]
        temp = Heap[i];
        Heap[i] = Heap[min];
        Heap[min] = temp;
        minHeapify(heapSize, Heap, min, heapIndexArray, nodeIndexArray);
    }
}

void DETNode::decreaseKey(int *Heap, int i,
                          int *heapIndexArray, int *nodeIndexArray) {
    int temp;
    while (i > 0) {
        int p = (i-1)/2;
        if (Heap[i] < Heap[p]) {
            // exchange index array
            temp = nodeIndexArray[i];
            nodeIndexArray[i] = nodeIndexArray[p];
            nodeIndexArray[p] = temp;
            heapIndexArray[nodeIndexArray[i]] = i;
            heapIndexArray[nodeIndexArray[p]] = p;
            // exchange Heap[i] with Heap[min]
            temp = Heap[i];
            Heap[i] = Heap[p];
            Heap[p] = temp;
            i = p;
        } else {
              break;
        }
    }
}

int DETNode::deleteHeap(int *heapSize, int *Heap, int index,
                        int *heapIndexArray, int *nodeIndexArray) {
    int temp, i = nodeIndexArray[index];
    *heapSize = *heapSize-1;
    Heap[index] = Heap[*heapSize];
    // exchange index array
    nodeIndexArray[index] = nodeIndexArray[*heapSize];
    heapIndexArray[nodeIndexArray[index]] = index;
    heapIndexArray[i] = -1;
    // compare with left and right child
    minHeapify(*heapSize, Heap, index, heapIndexArray, nodeIndexArray);
    // compare with parent
    decreaseKey(Heap, index, heapIndexArray, nodeIndexArray);
    return i;
}

