// Copyright © 2019 Sabyasachi Patra, IIIT Bhubaneswar, All rights reserved.
#include "GraphIsomor.h"

// Static variables
setword GraphIsomor::workspace[WORKSIZE];
int GraphIsomor::n;
int GraphIsomor::m;
set *GraphIsomor::gv;
graph GraphIsomor::g[MOSIZE*MOSIZE];
int GraphIsomor::lab[MOSIZE];
int GraphIsomor::ptn[MOSIZE];
int GraphIsomor::orbits[MOSIZE];

DEFAULTOPTIONS(options);
statsblk(stats);
graph mm[MOSIZE*MOSIZE];

int gsize;
int pos;
GMap *fastf;
Graph *fastg;
std::vector<char *> *fastvv;
bool *fasts;
bool **adj;
void GraphIsomor::initNauty(int size) {
    n = size;
    m = (n + WORDSIZE - 1) / WORDSIZE;
    nauty_check(WORDSIZE, m, n, NAUTYVERSIONID);
    options.getcanon = TRUE;
    options.writeautoms = FALSE;
    options.digraph = FALSE;
}

void GraphIsomor::finishNauty() {
    nauty_freedyn();
    naugraph_freedyn();
    nautil_freedyn();
}

void GraphIsomor::canonicalOrder(const char *in, char *can, int size) {
    int i, j, k;
    for (i=0; i < n; i++) {
        gv = GRAPHROW(g, i, m);
        EMPTYSET(gv, m);
        for (j=0; j < n; j++)
            if (in[i*size+j] == '1')
                ADDELEMENT(gv, j);
    }

    nauty(g, lab, ptn, NULL, orbits, &options, &stats,
          workspace, WORKSIZE, m, n, mm);

    k = 0;
    for (i=0; i < n; i++) {
        gv = GRAPHROW(mm, i, m);
        for (j=0; j < n; j++)
            can[k++] = ISELEMENT(gv, j)?'1':'0';
    }
    can[k] = '\0';
}

void GraphIsomor::canonicalOrderWithMap(const char *in, char *can,
                                        int size, int *map2) {
    int i, j, k;
    n = size;
    // cout << "in=" << in << endl;
    m = (n + WORDSIZE - 1) / WORDSIZE;
    for (i=0; i < n; i++) {
        gv = GRAPHROW(g, i, m);
        EMPTYSET(gv, m);
        for (j=0; j < n; j++)
            if (in[i*size+j] == '1')
                ADDELEMENT(gv, j);
    }

    nauty(g, lab, ptn, NULL, orbits, &options, &stats,
          workspace, WORKSIZE, m, n, mm);

    k = 0;
    for (i=0; i < n; i++) {
        gv = GRAPHROW(mm, i, m);
        for (j=0; j < n; j++)
            can[k++] = ISELEMENT(gv, j)?'1':'0';
    }
    can[k] = '\0';
    // cout << "can=" << can << endl;
    for (i=0; i < n; i++)
        map2[i] = lab[i];
}

GMap::GMap(int nf, int nr) {
    int i;
    numf = nf;
    numr = nr;
    f = new int[nf];
    r = new int[nr];

    for (i=0; i < nf; i++)
        f[i] = -1;
    for (i=0; i < nr; i++)
        r[i] = -1;
}

GMap::~GMap() {
    delete[] f;
    delete[] r;
}

void GraphIsomor::symmetryConditions(Graph *g,
                                     std::list< std::pair<int, int> > *cond) {
    int i, j, k, vvsize, size = g->numberNodes();
    std::pair<int, int> p;
    std::vector<char *> vv;
    cond->clear();
    findAutomorphisms(g, &vv);

    vvsize = vv.size();
    bool broken[vvsize];
    // std::cout << "vvsize= " << vvsize << "\n";
    for (i=0; i < vvsize; i++)
        broken[i] = false;
    for (i=0; i < size; i++) {
        vvsize = vv.size();
        for (j=0; j < vvsize; j++)
            if (!broken[j] && vv[j][i] != i)
                break;

        // There are still nodes not fixed
        if (j < vvsize) {
            for (k=i+1; k < size; k++) {
                for (j=0; j < static_cast<int>(vv.size()); j++) {
                    if (!broken[j] && vv[j][i] == k) {
                        p.first  = i;
                        p.second = k;
                        cond->push_back(p);
                        break;
                    }
                }
            }
        }

        // Reduce set of automorphisms to set that fix 'i'
        for (j=0; j < vvsize; j++)
            if (vv[j][i] != i)
                broken[j] = true;
    }
    for (j=0; j < vvsize; j++)
        delete [] vv[j];
}

void GraphIsomor::findAutomorphisms(Graph *G, std::vector<char *> *vv) {
    int i, j, k;
    int g;

    int n = G->numberNodes();
      GMap *f = new GMap(n, n);
    // int sequence[n][n];
    int sequence[MOSIZE][MOSIZE];

    for (i=0; i < n; i++)
        for (j=0; j < n; j++)
            if (G->isConnected(i, j))
                sequence[i][j] = G->numberNeighbours(j);
            else
                sequence[i][j] = 0;

    for (i=0; i < n; i++)
        qsort(sequence[i], n, sizeof(int), Graph::compareInt);

    // bool support[n*n];
    bool support[MOSIZE*MOSIZE];
    for (i=0; i < n; i++)
        for (j=0; j < n; j++) {
            for (k=0; k < n; k++)
                if (sequence[i][k] != sequence[j][k])
                    break;
            if (k < n)
                support[i*n+j] = false;
            else
                support[i*n+j] = true;
        }

    vv->clear();

    gsize = n;
    fastf  = f;
    fastg  = G;
    fastvv = vv;
    fasts  = support;
    adj = G->adjacencyMatrix();

    for (g=0; g < gsize; g++)
        if (support[g*gsize]) {
            f->add(0, g);
            pos = 1;
            isomorphicExtensions();
            f->remove(0);
        }
}

void GraphIsomor::isomorphicExtensions() {
    int i, j, ncand;
    // int cand[gsize];
    int cand[MOSIZE];
    int *v, num;

    if (pos == gsize) {
        char *v = new char[gsize];
        for (i=0; i < fastf->numf; i++)
            v[i] = fastf->f[i];
        fastvv->push_back(v);
    } else {
        std::list< std::pair<int, int> >::iterator ii;
        int n, m;
        int flag;
        // int count[gsize];
        int count[MOSIZE];
        ncand = 0;
        for (i=0; i < gsize; i++)
            count[i] = 0;
        for (i=0; i < gsize; i++)  // For all nodes of H already mapped
            if (fastf->f[i] != -1) {  // find their not mapped neighbours
                v = fastg->getNeighboursArray(i);
                num = fastg->numberNeighbours(i);
                for (j=0; j < num; j++)
                    if (fastf->f[v[j]] == -1) {
                        if (count[v[j]] == 0)
                            cand[ncand++] = v[j];
                        count[v[j]]++;
                    }
            }

        // Find most constrained neighbour 'm' (with more mapped neighbours)
        m = 0;
        for (i=1; i < ncand; i++)
            if (count[i] > count[m])  // Later: add more restraining conditions
                m = i;
        m = cand[m];

        ncand = 0;
        // bool already[gsize];
        bool already[MOSIZE];
        for (i=0; i < gsize; i++)
            already[i] = false;
        for (i=0; i < gsize; i++)  // For all nodes of G already mapped
            if (fastf->f[i] != -1) {  // find their not mapped neighbours
                v = fastg->getNeighboursArray(fastf->f[i]);
                num = fastg->numberNeighbours(fastf->f[i]);
                for (j=0; j < num; j++)
                    if (!already[v[j]] && fastf->r[v[j]]
                        == -1 && fasts[m*gsize+v[j]])  {
                        cand[ncand++] = v[j];
                        already[v[j]] = true;
                    }
            }

        for (i=0; i < ncand; i++) {
            n = cand[i];
            flag = false;
            for (j=0; j < gsize; j++)
                if (fastf->f[j] != -1) {
                    if (adj[m][j] != adj[n][fastf->f[j]]) {
                        flag = true;
                        break;
                    } else if (adj[j][m] != adj[fastf->f[j]][n]) {
                        flag = true;
                        break;
                    }
                }
            if (!flag) {
                fastf->add(m, n);
                pos++;
                isomorphicExtensions();
                pos--;
                fastf->remove(m);
            }
        }
    }
}
