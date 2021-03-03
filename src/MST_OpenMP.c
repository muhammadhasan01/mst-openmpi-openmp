#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void merge_sort(edge edges[], int n, int (*comparison)(edge*, edge*)) {
    if (n > 1) {
        int m = n / 2;
        edge larr[m], rarr[n - m];
        memcpy(larr, edges, m * sizeof(edge));
        memcpy(rarr, edges + m, (n - m) * sizeof(edge));

        merge_sort(larr, m, comparison);
        merge_sort(rarr, n - m, comparison);

        int l = 0, r = 0, j = 0;
        while (l < m && r < n - m) {
            if ((*comparison)(&larr[l], &rarr[r])) {
                edges[j++] = larr[l++];
            } else {
                edges[j++] = rarr[r++];
            }
        }

        while (l < m) {
            edges[j++] = larr[l++];
        }

        while (r < n - m) {
            edges[j++] = rarr[r++];
        }
    }
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
    merge_sort(edges, num_edge, comparison_weight);
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
    merge_sort(chosen_edges, num_chosen, comparison_node);
    for (int i = 0; i < n - 1; i++) {
        printf("%d-%d\n", chosen_edges[i].u, chosen_edges[i].v);
    }
    double time_taken = ((double) (clock() - t)) / CLOCKS_PER_SEC;
    printf("Waktu Eksekusi: %f ms\n", time_taken);

    return 0;
}
