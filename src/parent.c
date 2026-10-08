#include <windows.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 4096

int main(void) {
    HANDLE hPipe1Read, hPipe1Write;
    HANDLE hPipeMidRead, hPipeMidWrite;
    HANDLE hPipe2Read, hPipe2Write;

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&hPipe1Read, &hPipe1Write, &sa, 0) ||
        !CreatePipe(&hPipeMidRead, &hPipeMidWrite, &sa, 0) ||
        !CreatePipe(&hPipe2Read, &hPipe2Write, &sa, 0)) {
        return 1;
    }

    SetHandleInformation(hPipe1Write, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(hPipe2Read, HANDLE_FLAG_INHERIT, 0);

    // Запуск Child1
    STARTUPINFOA siChild1;
    PROCESS_INFORMATION piChild1;
    ZeroMemory(&siChild1, sizeof(siChild1));
    ZeroMemory(&piChild1, sizeof(piChild1));
    siChild1.cb = sizeof(siChild1);
    siChild1.hStdInput = hPipe1Read;
    siChild1.hStdOutput = hPipeMidWrite;
    siChild1.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    siChild1.dwFlags |= STARTF_USESTDHANDLES;

    char cmdChild1[] = ".\\child1.exe";
    if (!CreateProcessA(NULL, cmdChild1, NULL, NULL, TRUE, 0, NULL, NULL, &siChild1, &piChild1)) {
        return 1;
    }

    CloseHandle(hPipe1Read);
    CloseHandle(hPipeMidWrite);

    // Запуск Child2
    STARTUPINFOA siChild2;
    PROCESS_INFORMATION piChild2;
    ZeroMemory(&siChild2, sizeof(siChild2));
    ZeroMemory(&piChild2, sizeof(piChild2));
    siChild2.cb = sizeof(siChild2);
    siChild2.hStdInput = hPipeMidRead;
    siChild2.hStdOutput = hPipe2Write;
    siChild2.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    siChild2.dwFlags |= STARTF_USESTDHANDLES;

    char cmdChild2[] = ".\\child2.exe";
    if (!CreateProcessA(NULL, cmdChild2, NULL, NULL, TRUE, 0, NULL, NULL, &siChild2, &piChild2)) {
        return 1;
    }


    CloseHandle(hPipeMidRead);
    CloseHandle(hPipe2Write);

    char input[BUFFER_SIZE];
    DWORD bytesWritten, bytesRead;

    while (1) {
        fflush(stdout);

        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }

        DWORD len = (DWORD)strlen(input);
        if (len == 0 || (len == 1 && input[0] == '\n')) {
            continue;
        }

        if (!WriteFile(hPipe1Write, input, len, &bytesWritten, NULL)) {
            break;
        }

        char output[BUFFER_SIZE];
        
        if (ReadFile(hPipe2Read, output, sizeof(output) - 1, &bytesRead, NULL) && bytesRead > 0) {
            output[bytesRead] = '\0';
            printf("%s", output);
            fflush(stdout);
        } else {
            break;
        }
    }

    CloseHandle(hPipe1Write);
    CloseHandle(hPipe2Read);

    WaitForSingleObject(piChild1.hProcess, INFINITE);
    WaitForSingleObject(piChild2.hProcess, INFINITE);

    CloseHandle(piChild1.hProcess);
    CloseHandle(piChild1.hThread);
    CloseHandle(piChild2.hProcess);
    CloseHandle(piChild2.hThread);

    return 0;
}