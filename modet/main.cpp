// Copyright © 2019 Sabyasachi Patra, IIIT Bhubaneswar, All rights reserved.
// Main Function
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <cmath>
#include <iostream>
// using namespace std;

#include "Graph.h"
#include "GraphIsomor.h"
#include "Timer.h"
#include "SubgraphTree.h"
#include "ExpansionTree.h"
#include "DET.h"

#define MAX_BUF 256  // Maximum string buffer size

void parse_cmdline(int argc, char **argv);
void check_inputs();
void initialize();
void prepare_graph();
void compute_real();
void compute_results();
void compute_features();
void run_modet();
void run_mdet();
int compare_results(const void *a, const void *b);

// Variable declarations
char graph_file[MAX_BUF];
char feature_file[MAX_BUF];
char method[MAX_BUF];
int motif_size;
int rand_number;
int num_exchanges;
int num_tries;
int threshold;
Graph *g;
SubgraphTree realSG;
SubgraphTree realDSG;
ExpansionTree *realET;
ExpansionTree *randET;
DET *realDET;
DET *randDET;

int main(int argc, char **argv) {
    std::cout << "This program is developed by Sabyasachi Patra, ";
    std::cout << "IIIT Bhubaneswar, India" << std::endl;
    initialize();
    parse_cmdline(argc, argv);
    check_inputs();
    prepare_graph();
    compute_real();
    compute_features();
    //compute_results();
    GraphIsomor::finishNauty();
    return 0;
}

// Initialize everything
void initialize() {
    motif_size = -1;
    rand_number = -1;
    num_exchanges = 2;
    num_tries = 7;
    threshold = 0;
    snprintf(method, strlen("none")+1, "%s\n", "none");
    snprintf(graph_file, strlen("none")+1, "%s\n", "none");
    FILE *fp;
    fp = fopen("Output.txt", "w");
    fprintf(fp, "OUTPUT");
    fprintf(fp, "\n");
    fclose(fp);
    fp = fopen("Error.txt", "w");
    fprintf(fp, "ERROR");
    fprintf(fp, "\n");
    fclose(fp);
    GraphIsomor::initNauty(motif_size);
}

// Parse all command line arguments
void parse_cmdline(int argc, char **argv) {
    for (int i=1; i < argc; i++) {
        // Graph file
        if (!strcmp("-g", argv[i]) || !strcmp("--graph", argv[i])) {
            snprintf(graph_file, strlen(argv[i+1])+1, "%s\n", argv[i+1]);
            // cout << "graph_file = " << graph_file << endl;
            i++;
        } else if (!strcmp("-f", argv[i]) || !strcmp("--feature", argv[i])) {
            snprintf(feature_file, strlen(argv[i+1])+1, "%s\n", argv[i+1]);
            // cout << "feature_file = " << feature_file << endl;
            i++;
        } else if (!strcmp("-s", argv[i]) || !strcmp("--size", argv[i])) {
            motif_size = atoi(argv[++i]);  // Size of motifs to consider
        } else if (!strcmp("-m", argv[i]) || !strcmp("--method", argv[i])) {
            snprintf(method, strlen(argv[i+1])+1, "%s\n", argv[i+1]);
            // cout << "method = " << method << endl;
            i++;
        } else if (!strcmp("-r", argv[i]) || !strcmp("--random", argv[i])) {
            rand_number = atoi(argv[++i]);  // Method for set random graphs
        } else if (!strcmp("-e", argv[i]) || !strcmp("--exchanges", argv[i])) {
            num_exchanges = atoi(argv[++i]);  // Number of exchanges per edge
        } else if (!strcmp("-t", argv[i]) || !strcmp("--tries", argv[i])) {
            num_tries = atoi(argv[++i]);  // Number of tries per node
        } else if (!strcmp("-th", argv[i]) || !strcmp("--threshold", argv[i])) {
            threshold = atoi(argv[++i]);  // Threshold value
        }
    }
}

void check_inputs() {
    if (strcmp(method, "modet") != 0 && strcmp(method, "mdet") != 0) {
        std::cout << "invalid method" << std::endl;
        exit(1);
    }
    if (strcmp(method, "modet") == 0) {
        if (motif_size < 3 || motif_size > 10) {
            std::cout << "invalid motf size" << std::endl;
            exit(1);
        }
    }
    if (strcmp(method, "mdet") == 0) {
        if (motif_size < 3 || motif_size > 15) {
            std::cout << "invalid motf size" << std::endl;
            exit(1);
        }
    }
    if (strcmp(graph_file, "none") == 0) {
        std::cout << "no input graph file" << std::endl;
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
    // Print chosen parameters
    printf("motif size: %d\n", motif_size);
    printf("graph file: %s\n", graph_file);
    printf("%d nodes, %d edges\n", g->numberNodes(), g->numberEdges());
    threshold = static_cast<int>(g->numberNodes()*threshold/100.0);
    std::cout << "threshold = " << threshold << std::endl;
}

// Count subgraphs on real network
void compute_real() {
    // Print method name
    if (strcmp(method, "modet") == 0)
        printf("Method: MODET on real network\n");
    else if (strcmp(method, "mdet") == 0)
        printf("Method: MDET on real network\n");
    else
        printf("Invalid method\n");

    // Compute frequency
    printf("\nCounting subgraph frequency on 'REAL NETWORK'\n");
    Timer::start(0);
    if (strcmp(method, "modet") == 0)
        run_modet();
    else if (strcmp(method, "mdet") == 0)
        run_mdet();
    Timer::stop(0);
    if (strcmp(method, "modet") == 0) {
        printf("%d subgraphs, ", realSG.countSubgraphs());
        printf("%f embeddings\n", realSG.countEmbeddings());
    } else if (strcmp(method, "mdet") == 0) {
        printf("%d subgraphs, ", realDSG.countSubgraphs());
        printf("%f embeddings\n", realDSG.countEmbeddings());
    }
    printf("Time elapsed: %.6fs\n", Timer::elapsed(0));
}

// Run MODET method on graph 'g' and store results on SubgraphTree 'realSG'
void run_modet() {
    Timer::start(0);
    SubgraphTree isomorSG;
    realET = new ExpansionTree();
    realET->create(motif_size, &isomorSG);
    Timer::stop(0);
    printf("Creation time: %.2f\n", Timer::elapsed(0));
    Timer::start(0);
    realET->census(motif_size, g, &realSG);
    // realET->printEmbeddings(motif_size);
    Timer::stop(0);
    printf("census time: %.2f\n", Timer::elapsed(0));
}

// Run MDET method on graph 'g' and store results on SubgraphTree 'realSG'
void run_mdet() {
    SubgraphTree isomorSG;
    realDET = new DET();
    Timer::start(0);
    realDET->census(motif_size, g, &realDSG, &isomorSG, threshold);
    // realDET->printEmbeddings(motif_size);
    Timer::stop(0);
    printf("census time: %.2f\n", Timer::elapsed(0));
}

// Compute feature matrix
void compute_features() {
    if (strcmp(method, "modet") == 0) {
        realET->computeFeatureMatrix(motif_size, g, threshold, feature_file);   
    }
}

// Compute random networks and result
void compute_results() {
    int i, j;
    std::map<std::string, int>::iterator ii;
    // Create map and init results
    std::map<std::string, int> realMap;
    int resultSize;
    if (strcmp(method, "modet") == 0) {
        realET->inducedFreq(motif_size, g, &realSG, threshold);
        realSG.populateMap(&realMap, motif_size);
    } else if (strcmp(method, "mdet") == 0) {
        realDET->inducedFreq(motif_size, g, &realDSG, threshold);
        realDSG.populateMap(&realMap, motif_size);
    }
    ResultType result[realMap.size()];
    for (ii=realMap.begin(), i=0; ii != realMap.end(); ii++) {
        if (ii->second > 0 && ii->second >= threshold) {
            result[i].s = strdup((ii->first).c_str());
            result[i].freq = ii->second;
            result[i].z_score = 0;
            result[i].avgf_rand = 0;
            result[i].devf_rand = 0;
            i++;
        }
    }
    resultSize = i;

    // Do we have random networks to compute?
    if (rand_number > 0) {
        std::map<std::string, int> randMap[rand_number];
        // Generate all random networks
        printf("Computing random networks: ");
        for (i=0; i < rand_number; i++) {
            std::cout << "random count = " << i+1 << std::endl;
            // Create new random network from previous one
            int nswap = 0;
            Graph::randomGraph(g, num_exchanges, num_tries, &nswap);
            g->sortNeighbours();
            g->makeNeighboursArray();
            // cout << "after:" << nswap << " number of edge swappings\n";
            SubgraphTree randSG;
            SubgraphTree randDSG;
            SubgraphTree randIsomorSG;
            if (strcmp(method, "modet") == 0) {
                randET = new ExpansionTree();
                randET->create(motif_size, &randIsomorSG);
                randET->census(motif_size, g, &randSG);
                randET->inducedFreq(motif_size, g, &randSG, threshold);
                randSG.populateMap(&randMap[i], motif_size);
                delete randET;
            } else if (strcmp(method, "mdet") == 0) {
                randDET = new DET();
                randDET->census(motif_size, g, &randDSG,
                                &randIsomorSG, threshold);
                randDET->inducedFreq(motif_size, g, &randDSG, threshold);
                randDSG.populateMap(&randMap[i], motif_size);
                delete randDET;
            }
        }
        // Compute significance
        for (i=0; i < resultSize; i++) {
            // Average frequency
            double avg = 0;
            for (j=0; j < rand_number; j++)
                avg += randMap[j][result[i].s];
            avg /= rand_number;
            // Standard deviation
            double dev = 0;
            for (j=0; j < rand_number; j++)
                dev += (randMap[j][result[i].s]-avg)*
                       (randMap[j][result[i].s]-avg)/
                       static_cast<double>(rand_number-1);
            dev = sqrt(dev);
            double zscore;
            if (dev != 0)
                zscore = (result[i].freq - avg)/dev;
            else
                zscore = 0;
            result[i].avgf_rand = avg;
            result[i].devf_rand = dev;
            result[i].z_score = fabs(zscore);
            // result[i].z_score = zscore;
        }
    }
    // Sort results
    qsort(result, resultSize, sizeof(ResultType), compare_results);
    FILE *fp;
    fp = fopen("result.txt", "w");
    for (i=0; i < resultSize; i++) {
        if (rand_number > 0) {
            fprintf(fp, "%s : %d : %f\n",
                    result[i].s, result[i].freq, result[i].z_score);
            std::cout << result[i].s << " : " << result[i].freq << " : ";
            std::cout << result[i].z_score << std::endl;
        } else {
            fprintf(fp, "%s : %d\n", result[i].s, result[i].freq);
            std::cout << result[i].s << " : " << result[i].freq << std::endl;
        }
    }
    fprintf(fp, "\nnumber of motifs = %d\n", resultSize);
    std::cout << "\nnumber of motifs = " << resultSize << std::endl;
    fclose(fp);
}

// Compare two different motif results (for sorting)
int compare_results(const void *a, const void *b) {
    ResultType *r1 = (ResultType *)a;
    ResultType *r2 = (ResultType *)b;

    if (r1->z_score > r2->z_score) return -1;
    if (r1->z_score < r2->z_score) return +1;
    if (r1->freq > r2->freq) return -1;
    if (r1->freq < r2->freq) return +1;
    return 0;
}

