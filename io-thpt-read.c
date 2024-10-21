#include <stdio.h>
#include <windows.h>

#define BLOCK_SIZE 8192
#define FILENAME \
    "C:\\Users\\liza6\\CLionProjects\\osi1\\cmake-build-debug\\testfile.bin"
#define REPETITIONS 100


int main() {
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
    printf("Total time taken for 100 repetitions: %.4f seconds\n",
           total_time_taken);

    return 0;
}
