// Copyright © 2019 Sabyasachi Patra, IIIT Bhubaneswar, All rights reserved.
// Main Function
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <vector>
#include <list>
#include <map>

#include <cmath>
#include <iostream>
// using namespace std;

#include "Graph.h"

#define MAX_BUF 256  // Maximum string buffer size
#define MAX_CLUS 15  // Maximum cluster size

void parse_cmdline(int argc, char **argv);
void check_inputs();
void initialize();
void prepare_graph();
void compute_cluster();
void compute_weight();
void compute_distance();
void print_cluster();
void log_normalize();

// Variable declarations
char graph_file[MAX_BUF];
char dictionary_file[MAX_BUF];
char feature_file[MAX_BUF];
char cluster_file[MAX_BUF];
Graph *g;
float **FMat;
int nf;
float tMapper;
float *Weight;
float **Dist;
std::list< std::vector<int> > clusters;

int main(int argc, char **argv) {
    std::cout << "This program is developed by Sabyasachi Patra, ";
    std::cout << "IIIT Bhubaneswar, India" << std::endl;
    initialize();
    parse_cmdline(argc, argv);
    check_inputs();
    prepare_graph();
    compute_cluster();
    print_cluster();
    return 0;
}

// Initialize everything
void initialize() {
    nf = -1;
    tMapper = 0.2;
    snprintf(graph_file, strlen("none")+1, "%s\n", "none");
    snprintf(dictionary_file, strlen("none")+1, "%s\n", "none");
    snprintf(feature_file, strlen("none")+1, "%s\n", "none");
    snprintf(cluster_file, strlen("none")+1, "%s\n", "none");
    FILE *fp;
    fp = fopen("Output.txt", "w");
    fprintf(fp, "OUTPUT");
    fprintf(fp, "\n");
    fclose(fp);
    fp = fopen("Error.txt", "w");
    fprintf(fp, "ERROR");
    fprintf(fp, "\n");
    fclose(fp);
}

// Parse all command line arguments
void parse_cmdline(int argc, char **argv) {
    for (int i=1; i < argc; i++) {
        // Graph file
        if (!strcmp("-g", argv[i]) || !strcmp("--graph", argv[i])) {
            snprintf(graph_file, strlen(argv[i+1])+1, "%s\n", argv[i+1]);
            // cout << "graph_file = " << graph_file << endl;
            i++;
        } else if (!strcmp("-d", argv[i]) || !strcmp("--dictionary", argv[i])) {
            snprintf(dictionary_file, strlen(argv[i+1])+1, "%s\n", argv[i+1]);
            // cout << "dictionary_file = " << dictionary_file << endl;
            i++;
	} else if (!strcmp("-f", argv[i]) || !strcmp("--feature", argv[i])) {
            snprintf(feature_file, strlen(argv[i+1])+1, "%s\n", argv[i+1]);
            // cout << "feature_file = " << feature_file << endl;
            i++;
	} else if (!strcmp("-c", argv[i]) || !strcmp("--cluster", argv[i])) {
            snprintf(cluster_file, strlen(argv[i+1])+1, "%s\n", argv[i+1]);
            // cout << "cluster_file = " << cluster_file << endl;
            i++;
        } else if (!strcmp("-t", argv[i]) || !strcmp("--tMapper", argv[i])) {
            tMapper = 0.01*atoi(argv[++i]);
            // cout << "tMapper = " << tMapper << endl;
            i++;
        }
    }
}

void check_inputs() {
    if (strcmp(graph_file, "none") == 0) {
        std::cout << "no input graph file" << std::endl;
        exit(1);
    }
    if (strcmp(feature_file, "none") == 0) {
        std::cout << "no input feature file" << std::endl;
        exit(1);
    }
}

// Prepare the real graph for computation
void prepare_graph() {
    g = new Graph();
    // Read the graph file
    Graph::readGraphFile(g, graph_file);
    // sort and create neighbours array
    g->sortNeighbours();
    g->makeNeighboursArray();
    // Read the feature file
    nf=Graph::computeNumFeature(g, feature_file);
    int i,j;
    FMat = new float*[g->numberNodes()];
    for (i=0; i < g->numberNodes(); i++) {
        FMat[i] = new float[nf];
    }
    Graph::readFeatureFile(g, FMat, nf, feature_file);
    // Print chosen parameters
    printf("graph file: %s\n", graph_file);
    printf("dictionary file: %s\n", dictionary_file);
    printf("feature file: %s\n", feature_file);
    printf("clusters file: %s\n", cluster_file);
    printf("%d nodes, %d edges\n", g->numberNodes(), g->numberEdges());
    std::cout << "nf=" << nf << "\n";
    for (i=0; i<g->numberNodes(); i++) {
        for (j=0; j<nf; j++) {
            //std::cout << FMat[i][j] << "  ";
        }
        //std::cout << "\n";
    }
}

// Compute cluster
void compute_cluster() {
    int i, j, j1, k, m, flag;
    float wtol=0.0;
    // Log Normalize Features
    //log_normalize();
    // Compute Weights
    compute_weight();
    for (i=0; i < g->numberNodes(); i++) {
        //std:: cout << Weight[i] << "  ";
    }
    //std::cout << "\n";
    // Compute Distance Matrix
    compute_distance();
    for (i=0; i < g->numberNodes(); i++) {
        for (j=0; j < g->numberNodes(); j++) {
            //std:: cout << Dist[i][j] << "  ";
        }
        //std::cout << "\n\n";
    }
    
    //int b=0;
    
    for (i=1; i<g->numberNodes(); i++) {
        //b++;
        //if (b>100) break;
        int clus[MAX_CLUS];
        int clusSize=0;
        clus[0]=i;
        clusSize++;
        while (1) {
            //std::cout << "clusSize=" << clusSize << "\n";
            float minDist=999999.0;
            int a=-1,b=-1;
            for (j=0; j<clusSize; j++) {
                int *neiA = g->getNeighboursArray(clus[j]);
                for (k=0; k< g->numberNeighbours(clus[j]); k++) {
                    int present=0;
                    for (m=0; m<clusSize; m++) {
                        if (neiA[k]==clus[m]) {
                            present=1;
                            break;
                        }
                    }
                    if (present==1) {
                        continue;
                    }
		    float sumDist=0.0;
		    //Compute distance between cluster and neiA[k]
                    for (j1=0; j1<clusSize; j1++) {
                        sumDist+=Dist[clus[j1]][neiA[k]]/clusSize;
                    }
                    //Test min distance between cluser and neiA[k]
                    if (sumDist < minDist) {
                        minDist=sumDist;
                        a=clus[j];
                        b=neiA[k];
                    }
                }
            }
            if (b==-1) break;
            float avgWeight=0.0, newWeight=0.0;
            for (j=0; j<clusSize; j++) {
                avgWeight += Weight[clus[j]]/clusSize;
            }
            //std::cout<< avgWeight << "  ";
            //std::cout << "a=" << a << "  " << Weight[a] << "  b=" << b << "  " << Weight[b] << "\n";
            for (j=0; j<clusSize; j++) {
                newWeight += Weight[clus[j]]/(clusSize+1);
            }
            newWeight += Weight[b]/(clusSize+1);
            //std::cout<< newWeight << "  ";
            //std::cout << "tMapper = " << tMapper << std::endl;
	    wtol=pow(clusSize,tMapper)/(2*exp(clusSize*tMapper));
            //std::cout<< wtol*avgWeight << "  ";
            float diff;
            if (newWeight>avgWeight)
                diff=newWeight-avgWeight;
            else
                diff=avgWeight-newWeight;
            //std::cout<< diff << "\n";
            if (diff < wtol*avgWeight) {
                clus[clusSize]=b;
                clusSize++;
                for (j=0; j<clusSize; j++) {
                    std::cout<< clus[j] << "  ";
                }
                std::cout << "\n";
            }
            else break;
        }
        for (j=0; j<clusSize; j++) {
            //std::cout<< clus[j] << "  ";
        }
        //std::cout << "\n";
        // Sort cluster
        for (j=0; j<clusSize; j++) {
            for (k=clusSize-1; k>j;k--) {
                if (clus[k] < clus[k-1]) {
                    int temp=clus[k];
                    clus[k]=clus[k-1];
                    clus[k-1]=temp;
                }
            }
        }

        if (clusSize >= 3) {
            // Check if already added
            int flag=0;
            std::list< std::vector<int> >::iterator ii;
            std::vector<int>::iterator jj;
            for (ii=clusters.begin(); ii != clusters.end(); ii++) {
                if ((*ii).size() != clusSize) continue;
                for (jj=ii->begin(), j=0; jj != ii->end(); jj++, j++) {
                    if (*jj != clus[j]) break;
                }
                if (j == clusSize) {
                    flag=1;
                    break;
                }
            }
            if (flag == 1) continue;
            // Add
            std::vector<int> clusv;
            for (j=0; j< clusSize; j++) {
                clusv.push_back(clus[j]);
            }
            clusters.push_back(clusv);
        }
    }
}

// Compute Weights
void compute_weight() {
    int i, j;
    Weight = new float[g->numberNodes()];
    for (i=0; i < g->numberNodes(); i++) {
        Weight[i] = 0;
        for (j=0; j<nf; j++) {
            Weight[i]=Weight[i]+FMat[i][j];
        }
    }
}


// Compute Distance Matrix
void compute_distance() {
    int i, j, k;
    Dist = new float*[g->numberNodes()];
    for (i=0; i < g->numberNodes(); i++) {
        Dist[i] = new float[g->numberNodes()];
    }
    for (i=0; i < g->numberNodes(); i++) {
        for (j=0; j < g->numberNodes(); j++) {
            if (i==j) {
                Dist[i][j]=0;
                continue;
            }
            if (Weight[i]==0 && Weight[j]==0) {
                Dist[i][j]=0;
                continue;
            }
            float d=0;
            for (k=0; k<nf; k++) {
                d = d+(FMat[i][k]-FMat[j][k])*(FMat[i][k]-FMat[j][k]);
            }
            Dist[i][j]=sqrt(d);
        }
    }    
}

// Compute Distance Matrix
void log_normalize() {
    int i, j;
    for (i=0; i < g->numberNodes(); i++) {
        for (j=0; j<nf; j++) {
            FMat[i][j] = sqrt(FMat[i][j]);
        }
    }    
}


void print_cluster() {
    int i, j;
    FILE *f1 = fopen(dictionary_file, "r");
    if (!f1) {
        std::cout << "invalid dictionary file" << std::endl;
        exit(1);
    }
    FILE *f2 = fopen(cluster_file, "w");
    if (!f2) {
        std::cout << "unable to open cluster file" << std::endl;
        exit(1);
    }
    std::vector<std::string> dict;
    char a[20];
    int b;
    dict.push_back("none");
    while (fscanf(f1, "%s%d", a, &b) == 2) {
        dict.push_back(a);
    }
    //std::cout << "Dictionary:\n";
    for (i=1; i<g->numberNodes(); i++) {
        //std::cout << i << " = " << dict[i] << "\n";
    }
    //std::cout << "\n";
    std::list< std::vector<int> >::iterator ii;
    std::vector<int>::iterator jj;
    for (ii=clusters.begin(), i=1; ii != clusters.end(); ii++, i++) {
        fprintf(f2, "cluster:%d\n", i);
        for (jj=ii->begin(); jj != ii->end(); jj++) {
                //std::cout << *jj << "  ";
                //std::cout << dict[*jj] << "  ";
                fprintf(f2,"%s  ", dict[*jj].c_str());
        }
        //std::cout << std::endl;
        fprintf(f2,"\n");
    }
    std:: cout << "Number of clusters = " << i-1 << "\n";
    fclose(f1);
    fclose(f2);
}

