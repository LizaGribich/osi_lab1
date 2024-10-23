#include <limits.h>
#include <stdio.h>
#include <windows.h>

#define BLOCK_SIZE (8 * 1024) // 8 KB
#define ITERATION_COUNT_IO 100
#define ITERATION_COUNT_DIJKSTRA 500000000
#define VERTEX_COUNT 6

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

void perform_disk_read(const char* filePath) {
    HANDLE file;
    char buffer[BLOCK_SIZE];
    DWORD bytesRead;

    for (int i = 0; i < ITERATION_COUNT_IO; i++) {
        file = CreateFile(
                filePath,                // lpFileName: путь для файла
                GENERIC_READ,            // dwDesiredAccess: доступ на чтение
                FILE_SHARE_READ,         // dwShareMode: общий доступ на чтение
                NULL,                    // lpSecurityAttributes: нет атрибутов безопасности
                OPEN_EXISTING,           // dwCreationDisposition: открыть существующий файл
                FILE_FLAG_NO_BUFFERING,  // dwFlagsAndAttributes: отключение кэширования
                NULL                     // hTemplateFile: шаблон не используется
        );

        if (file == INVALID_HANDLE_VALUE) {
            printf("Failed to open file: %s\n", filePath);
            return;
        }

        while (ReadFile(file, buffer, BLOCK_SIZE, &bytesRead, NULL) &&
               bytesRead > 0) {
        }

        CloseHandle(file);
    }
}

DWORD WINAPI io_thpt_read(LPVOID lpParam) {
    const char* filePath = getenv("TESTFILE_PATH");
    if (filePath == NULL) {
        printf("TESTFILE_PATH is not set.\n");
        return 1;
    }

    printf("Path: %s\n", filePath);

    LARGE_INTEGER start, end, frequency;

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);

    perform_disk_read(filePath);

    QueryPerformanceCounter(&end);

    double total_time_taken =
            (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
    printf("Total time taken for %d repetitions of disk read: %.4f seconds\n",
           ITERATION_COUNT_IO, total_time_taken);

    return 0;
}

DWORD WINAPI short_path(LPVOID lpParam) {
    int graph[VERTEX_COUNT][VERTEX_COUNT] = {
            {  0,  7,  9,  0,  0, 14 },
            {  7,  0, 10, 15,  0,  0 },
            {  9, 10,  0, 11,  0,  2 },
            {  0, 15, 11,  0,  6,  0 },
            {  0,  0,  0,  6,  0,  9 },
            { 14,  0,  2,  0,  9,  0 }
    };

    LARGE_INTEGER start, end, frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);

    for (int i = 0; i < ITERATION_COUNT_DIJKSTRA; i++) {
        dijkstra(graph, 0);
    }

    QueryPerformanceCounter(&end);

    double time_taken =
            (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
    printf(
            "Time taken for %d iterations of Dijkstra's algorithm: %.16f "
            "seconds\n",
            ITERATION_COUNT_DIJKSTRA,
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
