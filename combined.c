#include <limits.h>
#include <stdio.h>
#include <windows.h>

#define BLOCK_SIZE 8192
#define FILENAME "C:\\Users\\liza6\\CLionProjects\\osi1\\cmake-build-debug\\testfile.bin"
#define REPETITIONS 100
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

DWORD WINAPI io_thpt_read(LPVOID lpParam) {
HANDLE file;
char buffer[BLOCK_SIZE];
DWORD bytesRead;
LARGE_INTEGER start, end, frequency;

QueryPerformanceFrequency(&frequency);
QueryPerformanceCounter(&start);

for (int i = 0; i < REPETITIONS; i++) {
file = CreateFile(FILENAME, GENERIC_READ, FILE_SHARE_READ, NULL,
                  OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
if (file == INVALID_HANDLE_VALUE) {
printf("Failed to open file\n");
return 1;
}

while (ReadFile(file, buffer, BLOCK_SIZE, &bytesRead, NULL) &&
bytesRead > 0) {
}

CloseHandle(file);
}

QueryPerformanceCounter(&end);

double total_time_taken =
        (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
printf("Total time taken for 100 repetitions of disk read: %.4f seconds\n",
total_time_taken);

return 0;
}

DWORD WINAPI short_path(LPVOID lpParam) {
int graph[V][V] = {{0, 7, 9, 0, 0, 14},  {7, 0, 10, 15, 0, 0},
                   {9, 10, 0, 11, 0, 2}, {0, 15, 11, 0, 6, 0},
                   {0, 0, 0, 6, 0, 9},   {14, 0, 2, 0, 9, 0}};

LARGE_INTEGER start, end, frequency;
QueryPerformanceFrequency(&frequency);
QueryPerformanceCounter(&start);

for (int i = 0; i < 500000000; i++) {
dijkstra(graph, 0);
}

QueryPerformanceCounter(&end);

double time_taken =
        (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
printf(
"Time taken for 500000000 iterations of Dijkstra's algorithm: %.16f "
"seconds\n",
time_taken);

return 0;
}

int main() {
    HANDLE ioThread, dijkstraThread;

    ioThread = CreateThread(NULL, 0, io_thpt_read, NULL, 0, NULL);
    if (ioThread == NULL) {
        printf("Error creating IO thread\n");
        return 1;
    }

    dijkstraThread = CreateThread(NULL, 0, short_path, NULL, 0, NULL);
    if (dijkstraThread == NULL) {
        printf("Error creating Dijkstra thread\n");
        return 1;
    }

    WaitForSingleObject(ioThread, INFINITE);
    WaitForSingleObject(dijkstraThread, INFINITE);

    CloseHandle(ioThread);
    CloseHandle(dijkstraThread);

    return 0;
}
