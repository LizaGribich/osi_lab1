#include <stdio.h>
#include <string.h>
#include <windows.h>

int main() {
    char                command[1024];
    STARTUPINFO         si;
    PROCESS_INFORMATION pi;
    LARGE_INTEGER       frequency, start, end;
    double              elapsedTime;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    QueryPerformanceFrequency(&frequency);

    while (1) {
        printf("shell> ");
        fgets(command, sizeof(command), stdin);

        command[strcspn(command, "\n")] = 0;

        if (strcmp(command, "exit") == 0) {
            break;
        }

        QueryPerformanceCounter(&start);

        if (!CreateProcess(
                NULL,     // lpApplicationName: путь к исполняемому файлу
                command,  // lpCommandLine: командная строка для выполнения
                NULL,     // lpProcessAttributes: NULL
                NULL,     // lpThreadAttributes: NULL
                FALSE,    // bInheritHandles: FALSE
                0,        // dwCreationFlags: 0
                NULL,     // lpEnvironment: NULL
                NULL,     // lpCurrentDirectory: NULL
                &si,      // lpStartupInfo: указатель на STARTUPINFO
                &pi       // lpProcessInformation: указатель на PROCESS_INFORMATION
                )) {
            printf("Failed to start process (%d)\n", GetLastError());
            continue;
        }

        WaitForSingleObject(pi.hProcess, INFINITE);

        QueryPerformanceCounter(&end);

        elapsedTime =
            (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
        printf("Program executed in %.3f seconds\n", elapsedTime);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    return 0;
}