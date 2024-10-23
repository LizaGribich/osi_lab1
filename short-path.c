#include <limits.h>
#include <stdio.h>
#include <windows.h>

#define VERTEX_COUNT 6
#define ITERATION_COUNT 500000000

int minDistance(const int dist[], const int sptSet[]) {
    int min = INT_MAX, min_index = -1;

    for (int v = 0; v < VERTEX_COUNT; v++) {
        if (sptSet[v] == 0 && dist[v] <= min) {
            min = dist[v];
            min_index = v;
        }
    }

    return min_index;
}

void dijkstra(int graph[VERTEX_COUNT][VERTEX_COUNT], int src) {
    int dist[VERTEX_COUNT];
    int sptSet[VERTEX_COUNT];

    for (int i = 0; i < VERTEX_COUNT; i++) {
        dist[i] = INT_MAX;
        sptSet[i] = 0;
    }

    dist[src] = 0;

    for (int count = 0; count < VERTEX_COUNT - 1; count++) {
        int u = minDistance(dist, sptSet);

        sptSet[u] = 1;

        for (int v = 0; v < VERTEX_COUNT; v++) {
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
    int graph[VERTEX_COUNT][VERTEX_COUNT] = {
            {  0,  7,  9,  0,  0, 14 },
            {  7,  0, 10, 15,  0,  0 },
            {  9, 10,  0, 11,  0,  2 },
            {  0, 15, 11,  0,  6,  0 },
            {  0,  0,  0,  6,  0,  9 },
            { 14,  0,  2,  0,  9,  0 }
    };


    double start = get_time_in_seconds();

    for (int i = 0; i < ITERATION_COUNT; i++) {
        dijkstra(graph, 0);
    }

    double end = get_time_in_seconds();

    double time_taken = end - start;
    printf("Time taken for %d iterations: %.16f seconds\n", ITERATION_COUNT, time_taken);

    return 0;
}
