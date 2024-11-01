#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <stdlib.h>

#define MAX_CMDS 10
#define BUFFER_SIZE 1024

int main() {
    char command[BUFFER_SIZE];
    LARGE_INTEGER frequency, start, end;
    double elapsedTime;

    QueryPerformanceFrequency(&frequency);

    while (1) {
        printf("shell> ");
        if (!fgets(command, sizeof(command), stdin)) {
            break;
        }

        command[strcspn(command, "\n")] = 0;

        if (strcmp(command, "exit") == 0) {
            break;
        }

        if (strncmp(command, "cd ", 3) == 0) {
            const char *path = command + 3;
            if (SetCurrentDirectory(path)) {
                char cwd[MAX_PATH];
                GetCurrentDirectory(sizeof(cwd), cwd);
                printf("Current directory: %s\n", cwd);
            } else {
                printf("Failed to change directory to %s\n", path);
            }
            continue;
        }

        char *commands[MAX_CMDS];
        int num_commands = 0;

        char *cmdline = _strdup(command);
        if (!cmdline) {
            fprintf(stderr, "Memory allocation error\n");
            continue;
        }

        char *next_token = NULL;
        char *token = strtok_s(cmdline, "|", &next_token);
        while (token != NULL && num_commands < MAX_CMDS) {
            while (*token == ' ') token++;
            char *token_end = token + strlen(token) - 1;
            while (token_end > token && (*token_end == ' ' || *token_end == '\n')) {
                *token_end = '\0';
                token_end--;
            }
            commands[num_commands++] = token;
            token = strtok_s(NULL, "|", &next_token);
        }

        HANDLE hPipeRead[MAX_CMDS], hPipeWrite[MAX_CMDS];
        for (int i = 0; i < MAX_CMDS; i++) {
            hPipeRead[i] = NULL;
            hPipeWrite[i] = NULL;
        }

        SECURITY_ATTRIBUTES saAttr;
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;

        for (int i = 0; i < num_commands - 1; i++) {
            if (!CreatePipe(&hPipeRead[i], &hPipeWrite[i], &saAttr, 0)) {
                fprintf(stderr, "Failed to create pipe\n");
                goto cleanup;
            }
        }

        PROCESS_INFORMATION pi[MAX_CMDS];
        ZeroMemory(&pi, sizeof(pi));

        QueryPerformanceCounter(&start);

        for (int i = 0; i < num_commands; i++) {
            STARTUPINFO si;
            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            si.dwFlags |= STARTF_USESTDHANDLES;

            if (i == 0) {
                si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
                si.hStdOutput = (num_commands > 1) ? hPipeWrite[i] : GetStdHandle(STD_OUTPUT_HANDLE);
            } else if (i == num_commands - 1) {
                si.hStdInput = hPipeRead[i - 1];
                si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
            } else {
                si.hStdInput = hPipeRead[i - 1];
                si.hStdOutput = hPipeWrite[i];
            }

            si.hStdError = si.hStdOutput;

            char cmdline_full[BUFFER_SIZE];
            snprintf(cmdline_full, sizeof(cmdline_full), "cmd.exe /c \"%s\"", commands[i]);

            if (!CreateProcess(
                    NULL,
                    cmdline_full,
                    NULL,
                    NULL,
                    TRUE,
                    0,
                    NULL,
                    NULL,
                    &si,
                    &pi[i])) {
                fprintf(stderr, "Failed to start process (%d)\n", GetLastError());
                goto cleanup;
            }

            if (i > 0) {
                CloseHandle(hPipeRead[i - 1]);
            }
            if (hPipeWrite[i] != NULL) {
                CloseHandle(hPipeWrite[i]);
            }
        }

        for (int i = 0; i < num_commands; i++) {
            WaitForSingleObject(pi[i].hProcess, INFINITE);
            CloseHandle(pi[i].hProcess);
            CloseHandle(pi[i].hThread);
        }

        QueryPerformanceCounter(&end);
        elapsedTime = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
        printf("Program executed in %.3f seconds\n", elapsedTime);

        cleanup:
        for (int i = 0; i < MAX_CMDS; i++) {
            if (hPipeRead[i]) CloseHandle(hPipeRead[i]);
            if (hPipeWrite[i]) CloseHandle(hPipeWrite[i]);
        }
        free(cmdline);
    }

    return 0;
}
