// Copyright © 2019 Sabyasachi Patra, IIIT Bhubaneswar, All rights reserved.
#include "ExpansionTree.h"

#define MOSIZE 15  // Maximum motif size

ExpansionTreeNode::ExpansionTreeNode(int d, int v, int e) {
    depth = d;
    numvertex = v;
    numedge = e;
    frequency = 0;
    canstr = NULL;
    cond.clear();
    child.clear();
    emb.clear();
}

ExpansionTreeNode::~ExpansionTreeNode() {
    if (canstr != NULL)
        delete[] canstr;
}

ExpansionTree::ExpansionTree() {
    root = new ExpansionTreeNode(0, 0, 0);
}

ExpansionTree::~ExpansionTree() {
    delete root;
}

void ExpansionTree::create(int size, SubgraphTree *isomorSG) {
    root->insertRootNode(1, 3, 2, isomorSG);
    root->expandTree(size, isomorSG);
    int sl = 0;
    // root->printTree(&sl);
}

void ExpansionTreeNode::insertRootNode(int d, int v, int e,
                                       SubgraphTree *isomorSG) {
    char *str = "001001110";  // root node
    Graph *sg = new Graph();
    Graph::strToGraph(sg, str, v);
    sg->makeNeighboursArray();
    std::list< std::pair<int, int> > *conditions =
    new std::list< std::pair<int, int> >;
    GraphIsomor::symmetryConditions(sg, conditions);
    ExpansionTreeNode *et = new ExpansionTreeNode(d, v, e);
    et->canstr = new char[strlen(str)+1];
    snprintf(et->canstr, strlen(str)+1, "%s\n", str);
    et->insertConditions(conditions);
    child.push_back(et);
    isomorSG->setString(str, 0);
    delete conditions;
    delete sg;
}

void ExpansionTreeNode::expandTree(int size, SubgraphTree *isomorSG) {
    int n, n1, e, d, maxedge;
    int ret, flag;
    int i, j;
    char *origstr, *str, *newstr;
    std::list<ExpansionTreeNode *>::iterator ii;
    for (ii=child.begin(); ii != child.end(); ii++) {
        n = (*ii)->numvertex;
        e = (*ii)->numedge;
        d = (*ii)->depth;
        // Expand by adding a vertex
        if (n < size) {
            origstr = new char[n*n+1];
            snprintf(origstr, strlen((*ii)->canstr)+1, "%s\n", (*ii)->canstr);
            n1 = n + 1;
            str = new char[n1*n1+1];
            // add 0 at the end of each row
            for (i=0; i < n; i++) {
                for (j=0; j < n; j++)
                    str[i*n1+j] = origstr[i*n+j];
                str[i*n1+j] = '0';
            }
            // add a 0 row
            for (j=0; j < n1; j++)
                str[i*n1+j] = '0';
            str[i*n1+j] = '\0';
            delete[] origstr;

            newstr = new char[n1*n1+1];
            flag = 0;
            for (i=0; i < n; i++) {
                snprintf(newstr, strlen(str)+1, "%s\n", str);
                newstr[i*n1+n]='1';
                newstr[n1*(n1-1)+i]='1';
                ret = (*ii)->insertNode(d+1, n+1, e+1, newstr, i, n, isomorSG);
                if (ret)
                    flag = 1;
            }
            delete[] str;
            delete[] newstr;
            if (flag) (*ii)->expandTree(size, isomorSG);
        } else {  // Expand by adding an edge
            str = new char[n*n+1];
            newstr = new char[n*n+1];
            snprintf(str, strlen((*ii)->canstr)+1, "%s\n", (*ii)->canstr);
            flag = 0;
            for (i=1; i < n; i++) {
                for (j=0; j < i; j++) {
                    if (str[i*n+j] == '0') {
                        snprintf(newstr, strlen(str)+1, "%s\n", str);
                        newstr[i*n+j] = '1';
                        newstr[j*n+i] = '1';
                        ret = (*ii)->insertNode(d+1, n, e+1, newstr,
                                                i, j, isomorSG);
                        if (ret) flag=1;
                    }
                }
            }
            delete[] str;
            delete[] newstr;
            maxedge = (n*(n-1))/2;
            if (flag && e < maxedge) (*ii)->expandTree(size, isomorSG);
        }
    }
}

int ExpansionTreeNode::insertNode(int d, int n, int e, const char *s,
                                  int a, int b, SubgraphTree *isomorSG) {
    // char str[n*n+1];
    char str[MOSIZE*MOSIZE+1];
    int i;
    // int map2[n];
    int map2[MOSIZE];
    GraphIsomor::canonicalOrderWithMap(s, str, n, map2);
    int mflag = 0;
    for (i=0; i < n; i++)
        if (map2[i] != i) mflag = 1;

    std::list<ExpansionTreeNode *>::iterator ii;
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
    ExpansionTreeNode *et = new ExpansionTreeNode(d, n, e);
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
void ExpansionTreeNode::
insertConditions(std::list< std::pair<int, int> > *conditions) {
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
    for (jj=conditions->begin(); jj != conditions->end(); jj++) {
        cond.push_back(*jj);
    }
}

void ExpansionTreeNode::printTree(int *sl) {
    int maxedge, v;
    std::list<ExpansionTreeNode *>::iterator ii;
    for (ii=child.begin(); ii != child.end(); ii++) {
        *sl = *sl+1;
        (*ii)->printGraph(*sl);
        v = (*ii)->numvertex;
        maxedge = (v * (v-1))/2;
        if ((*ii)->numedge <= maxedge)
            (*ii)->printTree(sl);
    }
}

void ExpansionTreeNode::printGraph(int n) {
    printf("%d : ", n);
    printCanonical(numvertex, canstr);
}

void ExpansionTreeNode::printCanonical(int n, char* str) {
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

void ExpansionTree::printEmbeddings(int size) {
    root->printEmbeddings(size);
}

void ExpansionTreeNode::printEmbeddings(int size) {
    std::list<ExpansionTreeNode *>::iterator ii;
    for (ii=child.begin(); ii != child.end(); ii++) {
        if ((*ii)->frequency <= 0) continue;
        if ((*ii)->numvertex == size) {
            std::cout << std::endl << (*ii)->canstr << " : ";
            std::cout << (*ii)->frequency << std::endl;
        }
        std::list< std::vector<int> >::iterator jj;
        for (jj=(*ii)->emb.begin(); jj != (*ii)->emb.end(); jj++) {
            std::vector<int>::iterator kk;
            for (kk=jj->begin(); kk != jj->end(); kk++)
                std::cout << *kk << "  ";
            std::cout << std::endl;
        }
        int maxedge = ( (*ii)->numvertex * ((*ii)->numvertex-1) )/2;
        if ((*ii)->frequency > 0 && (*ii)->numedge <= maxedge)
            (*ii)->printEmbeddings(size);
    }
}

void ExpansionTree::census(int size, Graph *g, SubgraphTree *sg) {
    root->census(size, g, sg);
}

void ExpansionTreeNode::census(int size, Graph *g, SubgraphTree *sg) {
    int n = g->numberNodes();
    std::list<ExpansionTreeNode *>::iterator ii;
    for (ii=child.begin(); ii != child.end(); ii++) {
        (*ii)->basicTreeCensus(size, g, sg);
        if ((*ii)->numvertex < size)
            (*ii)->treeCensus(size, g, sg);
        else
            (*ii)->graphCensus(size, g, sg);
    }
}

void ExpansionTreeNode::basicTreeCensus(int size, Graph *g, SubgraphTree *sg) {
    int i, j, k;
    bool **adjMat = g->adjacencyMatrix();
    int **fastnei = g->getNeighboursArray();
    int *numnei = g->numberNeighbours();
    for (i=0; i < g->numberNodes(); i++) {
        for (j=0; j< numnei[i]; j++) {
            for (k=j+1; k< numnei[i]; k++) {
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
    if (size == 3) sg->setString(canstr, frequency);
}

void ExpansionTreeNode::treeCensus(int size, Graph *g, SubgraphTree *sg) {
    int i, j, k, l, a, b, flag;
    int numv;
    std::list<ExpansionTreeNode *>::iterator ii;
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
    if (frequency == 0) return;
    for (ii=child.begin(); ii != child.end(); ii++) {
        numv = (*ii)->numvertex;
        // for each child traverse embeddings of parent node
         for (jj=emb.begin(); jj != emb.end(); jj++) {
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
                        // cout << "no map : ";
                        flag = 0;
                        for (ll=(*ii)->cond.begin(); ll != (*ii)->cond.end();
                             ll++) {
                            if (emp[ll->first] > emp[ll->second]) {
                                flag = 1;
                                break;
                            }
                        }
                        if (flag == 1)  continue;
                        std::vector<int> em;
                        for (j=0; j < numv; j++)
                            em.push_back(emp[j]);
                        (*ii)->emb.push_back(em);
                        (*ii)->frequency++;
                    } else {
                        // int map2[numv];
                        int map2[MOSIZE];
                        for (kk=mm->begin(), j=0; kk != mm->end(); kk++, j++)
                            map2[j] = *kk;
                        flag = 0;
                        for (ll=(*ii)->cond.begin(); ll != (*ii)->cond.end();
                             ll++) {
                            if (emp[map2[ll->first]] > emp[map2[ll->second]]) {
                                flag = 1;
                                break;
                            }
                        }
                        if (flag == 1)  continue;
                        // cout<<"push embedding with map\n";
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
        if (numv < size) {
            // sg->setString((*ii)->canstr, (*ii)->frequency);
            (*ii)->treeCensus(size, g, sg);
        } else {
            sg->setString((*ii)->canstr, (*ii)->frequency);
            (*ii)->graphCensus(size, g, sg);
        }
    }
}

void ExpansionTreeNode::graphCensus(int size, Graph *g, SubgraphTree *sg) {
    int i, j, k, l, a, b, flag;
    int numv, maxedge;
    std::list<ExpansionTreeNode *>::iterator ii;
    std::list< std::vector<int> >::iterator jj;
    std::vector<int>::iterator kk;
    std::list< std::pair<int, int> >::iterator ll;
    std::list< std::pair<int, int> >::iterator dd;
    std::list< std::vector<int> >::iterator mm;
    std::list<int>::iterator ff;
    int n = g->numberNodes();
    bool **adjM = g->adjacencyMatrix();

    if (frequency == 0) return;
    for (ii=child.begin(); ii != child.end(); ii++) {
        numv = (*ii)->numvertex;
        maxedge = (numv * (numv-1))/2;
        // for each child traverse embeddings of parent node
         for (jj=emb.begin(); jj != emb.end(); jj++) {
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
                        flag = 0;
                        for (ll=(*ii)->cond.begin(); ll != (*ii)->cond.end();
                             ll++) {
                            if (emp[ll->first] > emp[ll->second]) {
                                flag = 1;
                                break;
                            }
                        }
                        if (flag == 1)  continue;
                        // cout<<"push embedding without map\n";
                        (*ii)->emb.push_back(*jj);
                        (*ii)->frequency++;
                    } else {
                        // int map2[numv];
                        int map2[MOSIZE];
                        for (kk=mm->begin(), j=0; kk != mm->end(); kk++, j++)
                            map2[j] = *kk;
                        flag = 0;
                        for (ll=(*ii)->cond.begin(); ll != (*ii)->cond.end();
                             ll++) {
                            if (emp[map2[ll->first]] > emp[map2[ll->second]]) {
                                flag = 1;
                                break;
                            }
                        }
                        if (flag == 1)  continue;
                        // cout<<"push embedding with map\n";
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
        if ((*ii)->numedge <= maxedge) {
            sg->setString((*ii)->canstr, (*ii)->frequency);
            (*ii)->graphCensus(size, g, sg);
        }
    }
}

void ExpansionTree::inducedFreq(int size, Graph *g, SubgraphTree *sg, int th) {
    root->inducedFreq(size, g, sg, th);
}

void ExpansionTree::computeFeatureMatrix(int size, Graph *g, int th, char *feature_file) {
    int i;
    std::cout << "g->numberNodes()=" << g->numberNodes() << "\n";
    for (i=0; i < g->numberNodes()-1; i++) {
        std::vector<int> fv;
        fv.clear();
        g->features.push_back(fv);
    }
    root->computeFeatureMatrix(size, g, th);
    // Display and save feature matrix
    std::list< std::vector<int> >::iterator jj;
    std::vector<int>::iterator kk;
    FILE *fp;
    fp = fopen(feature_file, "w");
    for (jj=g->features.begin(); jj != g->features.end(); jj++) {
        for (kk=jj->begin(); kk != jj->end(); kk++) {
            //std::cout << *kk << "  ";
            fprintf(fp, "%d  ", *kk);
        }
        //std::cout << "\n";
        fprintf(fp, "\n");
    }
    fclose(fp);
}

void ExpansionTreeNode::inducedFreq(int size, Graph *g,
                                    SubgraphTree *sg, int th) {
    std::list<ExpansionTreeNode *>::iterator ii;
    int i, j, k;
    for (ii=child.begin(); ii != child.end(); ii++) {
        if ((*ii)->frequency <= 0) continue;
        if ((*ii)->frequency >= th && (*ii)->numvertex == size) {
            int freq = 0;
            std::list< std::vector<int> >::iterator jj;
            for (jj=(*ii)->emb.begin(); jj != (*ii)->emb.end(); jj++) {
                std::vector<int>::iterator kk;
                // int a[size];
                int a[MOSIZE];
                for (kk=jj->begin(), j=0; kk != jj->end(); kk++, j++)
                    a[j] = *kk;
                int flag = 0;
                for (j=0; j < size && flag == 0; j++) {
                    for (k=j+1; k < size; k++) {
                        if ((*ii)->canstr[j*size+k] == '0'
                            && g->hasEdge(a[j], a[k])) {
                            flag = 1;
                            break;
                        }
                    }
                }
                if (flag == 0) freq++;
            }
            (*ii)->frequency = freq;
            if (freq >= th) sg->setString((*ii)->canstr, freq);
        }
        int maxedge = ((*ii)->numvertex * ((*ii)->numvertex-1))/2;
        if ((*ii)->frequency > 0 && (*ii)->numedge <= maxedge)
            (*ii)->inducedFreq(size, g, sg, th);
    }
}

void ExpansionTreeNode::computeFeatureMatrix(int size, Graph *g, int th) {
    std::list<ExpansionTreeNode *>::iterator ii;
    for (ii=child.begin(); ii != child.end(); ii++) {
        if ((*ii)->frequency <= 0) continue;
        if ((*ii)->frequency >= th && (*ii)->numvertex == size && strcmp((*ii)->canstr, "001001110") != 0 && strcmp((*ii)->canstr, "0001000100011110") != 0 && strcmp((*ii)->canstr, "0010000110010110") != 0) {
            // get degrees of pattern
            int i, j, k, d, m, l1;
            int l=strlen((*ii)->canstr);
            int degrees[MOSIZE];
            int origdegrees[MOSIZE];
            j=0;
            d=0;
            l1=0;
            m=0;
            std::cout << "canstr=" << (*ii)->canstr << "\n";
            for (i=0; i<=l; i++) {
                //std::cout << "i=" << i << "  j=" << j << "\n";
                if (j < size-1) {
                    if ((*ii)->canstr[i] == '1') d++;
                    j++;
                }
                else {
                    if ((*ii)->canstr[i] == '1') d++;
                    //std::cout<< "d=" << d << "\n";
                    origdegrees[m]=d;
                    m++;
                    if (l1==0) {
                         degrees[0]=d;
                         l1++;
                         d=0;
                         j=0;
                         continue;
                    }
                    for (k=0;k<l1;k++) {
                        if (d == degrees[k]) break;
                    }
                    if (k == l1) {
                        for (k=l1-1;k>=0;k--) {
                            if (d < degrees[k]) {
                                degrees[k+1] =degrees[k];
                            }
                            else {
                                degrees[k+1] = d;
                                break;
                            }
                        }
                        l1++;
                    }
                    d=0;
                    j=0;
                }
            }
            for (i=0; i<l1; i++) {
                std::cout << degrees[i] << "  ";
            }
            std::cout << "\n";

            for (i=0; i<size; i++) {
                std::cout << origdegrees[i] << "  ";
            }
            std::cout << "\n";

            int freq[g->numberNodes()][l1];
            // Pattern frequency of each protein initialized to 0
            for (i=0; i<g->numberNodes(); i++) {
                for (j=0; j<l1; j++) {
                    freq[i][j]=0;
                }
            }
            
            std::list< std::vector<int> >::iterator jj;
            std::vector<int>::iterator kk;
            // Update pattern frequency
            for (jj=(*ii)->emb.begin(); jj != (*ii)->emb.end(); jj++) {
                for (kk=jj->begin(), i=0; kk != jj->end(); kk++, i++) {
                    //std::cout << *kk << "  ";
                    for (j=0; j<l1; j++) {
                        if (origdegrees[i]==degrees[j]) break;
                    }
                    freq[*kk-1][j]++;
                }
                //std::cout << "\n";
            }
            //std::cout << "emb.size()=" << (*ii)->emb.size()*size << "\n";

            //for (i=0; i<g->numberNodes(); i++) {
            //    for (j=0; j<l1; j++) {
            //        std::cout << "freq[i][j]=" << freq[i][j] << "  ";
            //    }
            //    std::cout << "\n";
            //}

            // Set feature matrix
            for (jj=g->features.begin(), i=0; jj != g->features.end(); jj++, i++) {
                for (j=0; j<l1; j++) {
                    (*jj).push_back(freq[i][j]);
                }
            }

            // Display and save feature matrix
            FILE *fp;
            int count=0;
            for (jj=g->features.begin(); jj != g->features.end(); jj++) {
                for (kk=jj->begin(); kk != jj->end(); kk++) {
                    //std::cout << *kk << "  ";
                    count=count+(*kk);
                }
                //std::cout << "\n";
            }
            //std::cout << "count=" << count << "\n";
        }
        int maxedge = ((*ii)->numvertex * ((*ii)->numvertex-1))/2;
        if ((*ii)->frequency > 0 && (*ii)->numedge <= maxedge)
            (*ii)->computeFeatureMatrix(size, g, th);
    }
}
