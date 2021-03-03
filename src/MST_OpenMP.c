#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <assert.h>

typedef struct edge {
    int u, v, w;
} edge;

int n;
edge* edges, *chosen_edges;
int* par;
int num_edge;

int find_set(int u) {
    if (par[u] == u) return u;
    return par[u] = find_set(par[u]);
}

int merge(int u, int v) {
    int pu = find_set(u), pv = find_set(v);
    if (pu == pv) return 0;
    par[pv] = pu;
    return 1;
}

int comparison_weight(edge* x, edge* y) {
    if (x->w == y->w) {
        if (x->u == y->u)
            return x->v < y->v;
        return x->u < y->u;
    }
    return x->w < y->w;
}

int comparison_node(edge* x, edge* y) {
    if (x->u == y->u)
        return x->v < y->v;
    return x->u < y->u;
}

int main(int argc, char** argv) {
    clock_t t = clock();
    scanf("%d", &n);
    edge* edges = (edge*) malloc(n * n * sizeof(edge));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int x;
            scanf("%d", &x);
            if (x == -1) continue;
            if (i >= j) continue;
            edges[num_edge].u = i;
            edges[num_edge].v = j;
            edges[num_edge].w = x;
            num_edge++;
        }
    }
    assert(num_edge >= n - 1);
    // TODO: Parallel Sorting
    for (int i = 0; i < num_edge - 1; i++) {
        for (int j = i + 1; j < num_edge; j++) {
            if (comparison_weight(&edges[j], &edges[i])) {
                edge temp = edges[i];
                edges[i] = edges[j];
                edges[j] = temp;
            }
        }
    }
    par = (int*) malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        par[i] = i;
    }
    long long total_cost = 0;
    int num_chosen = 0;
    chosen_edges = (edge*) malloc(num_edge * sizeof(edge));
    for (int i = 0; i < num_edge; i++) {
        int u = edges[i].u;
        int v = edges[i].v;
        int w = edges[i].w;
        if (merge(u, v)) {
            total_cost += w;
            chosen_edges[num_chosen++] = edges[i];
            if (num_chosen == n - 1) break;
        }
    }
    printf("%lld\n", total_cost);
    // TODO : Parallel Sorting
    for (int i = 0; i < num_chosen - 1; i++) {
        for (int j = i + 1; j < num_chosen; j++) {
            if (comparison_node(&chosen_edges[j], &chosen_edges[i])) {
                edge temp = chosen_edges[i];
                chosen_edges[i] = chosen_edges[j];
                chosen_edges[j] = temp;
            }
        }
    }
    for (int i = 0; i < n - 1; i++) {
        printf("%d-%d\n", chosen_edges[i].u, chosen_edges[i].v);
    }
    double time_taken = ((double) (clock() - t)) / CLOCKS_PER_SEC;
    printf("Waktu Eksekusi: %f\n", time_taken);

    return 0;
}
