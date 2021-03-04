#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
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

int merge_set(int u, int v) {
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

void merge(edge edges[], edge larr[], int nl, edge rarr[], int nr, int (*comparison)(edge*, edge*)) {
    int il = 0, ir = 0, j = 0;
    while (il < nl && ir < nr) {
        if ((*comparison)(&larr[il], &rarr[ir])) {
            edges[j] = larr[il];
            il++;
        } else {
            edges[j] = rarr[ir];
            ir++;
        }
        j++;
    }

    while (il < nl) {
        edges[j] = larr[il];
        il++; j++;
    }

    while (ir < nr) {
        edges[j] = rarr[ir];
        ir++; j++;
    }
}

void merge_sort(edge edges[], int n, int (*comparison)(edge*, edge*)) {
    if (n > 1) {
        int m = n / 2;
        edge larr[m], rarr[n - m];
        memcpy(larr, edges, m * sizeof(edge));
        memcpy(rarr, edges + m, (n - m) * sizeof(edge));

        merge_sort(larr, m, comparison);
        merge_sort(rarr, n - m, comparison);

        merge(edges, larr, m, rarr, n - m, comparison);
    }
}

int main(int argc, char** argv) {
    int world_size, world_rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Define mpi type
    MPI_Datatype MPI_EDGE;
    // Type create struct
    int block_count = 3;
    const int block_lengths[3] = { 1, 1, 1 };
    const MPI_Aint displacements[3] = { 0, sizeof(int), sizeof(int) * 2 };
    MPI_Datatype block_types[3] = { MPI_INT, MPI_INT, MPI_INT };
    MPI_Type_create_struct(block_count, block_lengths, displacements, block_types, &MPI_EDGE);
    // Commit type
    MPI_Type_commit(&MPI_EDGE);

    // Input
    clock_t t = clock();
    if (world_rank == 0) {
        scanf("%d", &n);
        edges = (edge*) malloc(n * n * sizeof(edge));
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
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (world_rank == 0) {
        for (int i = 1; i < world_size; i++)
            MPI_Send(&num_edge, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    } else {
        MPI_Recv(&num_edge, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Process
    {
        // Define array for gather and scatter
        edge gathered[num_edge];
        int scattered_size = num_edge / world_size;
        edge scattered[scattered_size + 1];

        int sendcounts[world_size];
        int displs[world_size];
        int rem = num_edge % world_size;
        int sum = 0;
        for (int i = 0; i < world_size; i++) {
            sendcounts[i] = num_edge / world_size + (i < rem);
            displs[i] = sum;
            sum += sendcounts[i];
        }

        // Scatter and gather
        MPI_Scatterv(edges, sendcounts, displs, MPI_EDGE, scattered, sendcounts[world_rank], MPI_EDGE, 0, MPI_COMM_WORLD);
        merge_sort(scattered, sendcounts[world_rank], comparison_weight);
        MPI_Gatherv(scattered, sendcounts[world_rank], MPI_EDGE, gathered, sendcounts, displs, MPI_EDGE, 0, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);
    }

    int num_chosen = 0;
    long long total_cost = 0;
    if (world_rank == 0) {
        par = (int*) malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            par[i] = i;
        }
        chosen_edges = (edge*) malloc(num_edge * sizeof(edge));
        for (int i = 0; i < num_edge; i++) {
            int u = edges[i].u;
            int v = edges[i].v;
            int w = edges[i].w;
            if (merge_set(u, v)) {
                total_cost += w;
                chosen_edges[num_chosen++] = edges[i];
                if (num_chosen == n - 1) break;
            }
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (world_rank == 0) {
        for (int i = 1; i < world_size; i++)
            MPI_Send(&num_chosen, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    } else {
        MPI_Recv(&num_chosen, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    {
        // Define array for gather and scatter
        edge gathered[num_chosen];
        int scattered_size = num_chosen / world_size;
        edge scattered[scattered_size + 1];

        int sendcounts[world_size];
        int displs[world_size];
        int rem = num_chosen % world_size;
        int sum = 0;
        for (int i = 0; i < world_size; i++) {
            sendcounts[i] = num_chosen / world_size + (i < rem);
            displs[i] = sum;
            sum += sendcounts[i];
        }

        // Scatter and gather
        MPI_Scatterv(chosen_edges, sendcounts, displs, MPI_EDGE, scattered, sendcounts[world_rank], MPI_EDGE, 0, MPI_COMM_WORLD);
        merge_sort(scattered, sendcounts[world_rank], comparison_node);
        MPI_Gatherv(scattered, sendcounts[world_rank], MPI_EDGE, gathered, sendcounts, displs, MPI_EDGE, 0, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);
    }

    // Output
    if (world_rank == 0) {
        printf("%lld\n", total_cost);

        for (int i = 0; i < num_chosen; i++) {
            printf("%d-%d\n", chosen_edges[i].u, chosen_edges[i].v);
        }
        double time_taken = ((double) (clock() - t)) / CLOCKS_PER_SEC;
        printf("Waktu Eksekusi: %f ms\n", time_taken);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    // End
    MPI_Type_free(&MPI_EDGE);
    MPI_Finalize();
    return 0;
}
