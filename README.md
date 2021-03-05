# Minimum Spanning Tree - OpenMPI & OpenMP

This repository is an implementation of __Minimum Spanning Tree__ (MST) with parallelization using OpenMPI & OpenMP in C. There are two programs, one that uses OpenMP ([MST_OpenMP.c](./src/MST_OpenMP.c)) and one that uses OpenMPI ([MST_OpenMPI.c](./src/OpenMPI.c)), both uses parallelization, but have different approaches, OpenMPI is a particular API of MPI whereas OpenMP is shared memory standard available with compiler.

## Running the Program

To run our program, you basically need to compile and make an executable file of the program. So, you can move to the src folder first, then to the run the OpenMPI (locally) you can use:

```
mpicc -o <program_name> MST_OpenMPI.c && mpirun -np <num_of_process> <program_name>.exe
```

To run the OpenMP you can use:

```
gcc -o <program_name> -fopenmp MST_OpenMP.c && <program.name>.exe
```

## Approach on Getting Minimum Spanning Tree with Parallelization

In this section, we will explain how we use parallelization to get a Minimum Spanning Tree. We will divide it into two part, one that uses OpenMPI and one that uses OpenMP

### OpenMPI Approach

On our [MST_OpenMPI.c](./src/MST_OpenMPI.c) we use *Kruskal Algorithm* to find the MST. Basically, we use an edge list with weight included, and then sort the edges ascending by its weight. So, for sorting, we use Merge Sort with parallelization in OpenMPI by using scatter and gather. We first define the number of processes, and then we try to “scatter” the edges. So each process will get their own edges, and then each process will sort that edges independently. After the edges are sorted in each process, we then gather it and merge it again. After the edges are sorted we iterate from left to right, and we also use _Disjoint Set Union_ to take the edge needed in MST, ensuring that we didn’t get any loop. While we iterate, we also take the cost of chosen edges to get the cost of MST. And then, we use the same Merge Sort again but with different comparison to get minimum lexicographically output.

### OpenMP Approach

For the OpenMP program [MST_OpenMP.c](./src/MST_OpenMP.c), we also use *Kruskal Algorithm* to find the MST. The approach is just the same as the OpenMPI approach described above. But, here we use omp.h to parallelize the Merge Sort. Within pragma omp parallel and pragma omp single, we define the recursive call of the `merge_sort` as a task. Here is the snippet for the parallelization section.

```
#pragma omp parallel
{
    #pragma omp single
    {
        #pragma omp task shared(larr, m, comparison)
        merge_sort(larr, m, comparison);

        #pragma omp task shared(rarr, n, m, comparison)
        merge_sort(rarr, n - m, comparison);
    }
}
```

## Authors

- Muhammad Hasan (13518102) - [muhammadhasan01](https://github.com/muhammadhasan01)
- Naufal Dean Anugrah (13518123) - [naufal-dean](https://github.com/naufal-dean)
