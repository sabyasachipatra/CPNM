// Copyright © 2019 Sabyasachi Patra, IIIT Bhubaneswar, All rights reserved.
#include "SubgraphTree.h"

#define MOSIZE 15  // Maximum motif size

SubgraphTreeNode::SubgraphTreeNode() {
    frequency = 0;
    zero = NULL;
    one = NULL;
}

SubgraphTreeNode::~SubgraphTreeNode() {
    if (zero != NULL) delete zero;
    if ( one != NULL) delete  one;
}

void SubgraphTreeNode::zeroFrequency() {
    frequency = 0;
    if (zero != NULL) zero->zeroFrequency();
    if (one  != NULL)  one->zeroFrequency();
}

// add embedding to a subgraph tree
void SubgraphTreeNode::incrementString(int pos, char *s) {
    if (s[pos] == 0) {
        frequency++;
    } else {
        if (s[pos] == '1') {
            if (one == NULL)
                one = new SubgraphTreeNode();
            one->incrementString(pos+1, s);
        } else if (s[pos] == '0') {
            if (zero == NULL)
                zero = new SubgraphTreeNode();
            zero->incrementString(pos+1, s);
        }
    }
}

// set a frequency to a subgraph
void SubgraphTreeNode::setString(int pos, char *s, int f) {
    if (s[pos] == 0) {
        frequency = f;
    } else {
        if (s[pos] == '1') {
            if (one == NULL)
                one = new SubgraphTreeNode();
            one->setString(pos+1, s, f);
        } else if (s[pos] == '0') {
            if (zero == NULL)
                zero = new SubgraphTreeNode();
            zero->setString(pos+1, s, f);
        }
    }
}

// check the presence of a subgraph in a subgraph tree
// 1: present, 0: absent
int SubgraphTreeNode::checkString(int pos, char *s) {
    if (s[pos] == 0) {
        return 1;
    } else {
        if (s[pos] == '1') {
            if (one == NULL)
                return 0;
            one->checkString(pos+1, s);
        } else if (s[pos] == '0') {
            if (zero == NULL)
                return 0;
            zero->checkString(pos+1, s);
        }
    }
}

// add a subgraph with its frequency
void SubgraphTreeNode::addString(int pos, char *s, int f) {
    if (s[pos] == 0) {
        frequency += f;
    } else {
        if (s[pos] == '1') {
            if (one == NULL)
                one = new SubgraphTreeNode();
            one->addString(pos+1, s, f);
        } else if (s[pos] == '0') {
            if (zero == NULL)
                zero = new SubgraphTreeNode();
            zero->addString(pos+1, s, f);
        }
    }
}

// show frequency of all subgraph
void SubgraphTreeNode::showFrequency(int pos, char *s) {
    if (zero == NULL && one == NULL) {
        s[pos] = 0;
        printf("%s: %d\n", s, frequency);
    } else {
        if (zero != NULL) {
            s[pos] = '0';
            zero->showFrequency(pos+1, s);
        }
        if (one != NULL) {
            s[pos] = '1';
            one->showFrequency(pos+1, s);
        }
    }
}

// populate canstring vs freq
void SubgraphTreeNode::populateMap(std::map<std::string, int> *m, int size,
                                   int pos, char *s) {
    if (zero == NULL && one == NULL) {
        s[pos] = 0;
        // char cans[size*size+1];
        char cans[MOSIZE*MOSIZE+1];
        GraphIsomor::canonicalOrder(s, cans, size);
        (*m)[cans] = frequency;
    } else {
        if (zero != NULL) {
            s[pos] = '0';
            zero->populateMap(m, size, pos+1, s);
        }
        if (one != NULL) {
            s[pos] = '1';
            one->populateMap(m, size, pos+1, s);
        }
    }
}

// Count number of embeddings
double SubgraphTreeNode::countEmbeddings() {
    double ne = frequency;
    if (zero != NULL)
        ne += zero->countEmbeddings();
    if (one != NULL)
        ne +=  one->countEmbeddings();
    return ne;
}

// Count number of subgraphs
int SubgraphTreeNode::countSubgraphs() {
    int ns = 0;
    if (frequency > 0)
        ns++;
    if (zero != NULL)
        ns += zero->countSubgraphs();
    if (one != NULL)
        ns +=  one->countSubgraphs();
    return ns;
}

// -----------------------------------

SubgraphTree::SubgraphTree() {
    root = new SubgraphTreeNode();
}

SubgraphTree::~SubgraphTree() {
    if (root)
        delete root;
}

void SubgraphTree::zeroFrequency() {
    root->zeroFrequency();
}

void SubgraphTree::incrementString(char *s) {
    root->incrementString(0, s);
}

void SubgraphTree::setString(char *s, int f) {
    root->setString(0, s, f);
}
int SubgraphTree::checkString(char *s) {
    return root->checkString(0, s);
}

void SubgraphTree::addString(char *s, int f) {
    root->addString(0, s, f);
}

void SubgraphTree::showFrequency(int maxsize) {
    // char s[maxsize*maxsize+1];
    char s[MOSIZE*MOSIZE+1];
    root->showFrequency(0, s);
}


void SubgraphTree::populateMap(std::map<std::string, int> *m, int maxsize) {
    // char s[maxsize*maxsize+1];
    char s[MOSIZE*MOSIZE+1];
    root->populateMap(m, maxsize, 0, s);
}

double SubgraphTree::countEmbeddings() {
    return root->countEmbeddings();
}

int SubgraphTree::countSubgraphs() {
    return root->countSubgraphs();
}
