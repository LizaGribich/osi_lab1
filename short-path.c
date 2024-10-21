#include <limits.h>
#include <stdio.h>
#include <windows.h>

#define V 6

int minDistance(const int dist[], const int sptSet[]) {
    int min = INT_MAX, min_index = -1;

    for (int v = 0; v < V; v++) {
        if (sptSet[v] == 0 && dist[v] <= min) {
            min = dist[v];
            min_index = v;
        }
    }

    return min_index;
}

void dijkstra(int graph[V][V], int src) {
    int dist[V];
    int sptSet[V];

    for (int i = 0; i < V; i++) {
        dist[i] = INT_MAX;
        sptSet[i] = 0;
    }

    dist[src] = 0;

    for (int count = 0; count < V - 1; count++) {
        int u = minDistance(dist, sptSet);

        sptSet[u] = 1;

        for (int v = 0; v < V; v++) {
            if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX &&
                dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
            }
        }
    }
}

double get_time_in_seconds() {
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / frequency.QuadPart;
}

int main() {
    int graph[V][V] = {{0, 7, 9, 0, 0, 14},  {7, 0, 10, 15, 0, 0},
                       {9, 10, 0, 11, 0, 2}, {0, 15, 11, 0, 6, 0},
                       {0, 0, 0, 6, 0, 9},   {14, 0, 2, 0, 9, 0}};

    double start = get_time_in_seconds();

    for (int i = 0; i < 500000000; i++) {
        dijkstra(graph, 0);
    }

    double end = get_time_in_seconds();

    double time_taken = end - start;
    printf("Time taken for 500000000 iterations: %.16f seconds\n", time_taken);

    return 0;
}
