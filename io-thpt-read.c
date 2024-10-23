#include <stdio.h>
#include <windows.h>

#define BLOCK_SIZE (8 * 1024)  // 8 KB
#define ITERATION_COUNT 10

int main() {
    char *filePath = getenv("TESTFILE_PATH");
    if (filePath == NULL) {
        printf("TESTFILE_PATH is not set.\n");
        return 1;
    }

    printf("Path: %s\n", filePath);

    HANDLE        file;
    char          buffer[BLOCK_SIZE];
    DWORD         bytesRead;
    LARGE_INTEGER start, end, frequency;

    QueryPerformanceFrequency(&frequency);

    QueryPerformanceCounter(&start);

    for (int i = 0; i < ITERATION_COUNT; i++) {
        file = CreateFile(
            filePath,                // lpFileName: путь для файла
            GENERIC_READ,            // dwDesiredAccess: доступ на чтение
            FILE_SHARE_READ,         // dwShareMode: общий доступ на чтение
            NULL,                    // lpSecurityAttributes: нет атрибутов безопасности
            OPEN_EXISTING,           // dwCreationDisposition: открыть существующий файл
            FILE_FLAG_NO_BUFFERING,  // dwFlagsAndAttributes: отключение
                                     // кэширования
            NULL                     // hTemplateFile: шаблон не используется
        );
        if (file == INVALID_HANDLE_VALUE) {
            printf("Failed to open file: %s\n", filePath);
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
    double total_data_MB =
        (double)(BLOCK_SIZE * ITERATION_COUNT) / (1024 * 1024);

    double throughput_MBps = total_data_MB / total_time_taken;

    printf("Total time taken for %d repetitions: %.4f seconds\n",
           ITERATION_COUNT,
           total_time_taken);
    printf("Total data read: %.2f MB\n", total_data_MB);
    printf("Throughput: %.2f MB/s\n", throughput_MBps);

    return 0;
}
