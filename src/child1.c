#include <windows.h>
#include <ctype.h>
#define BUFFER_SIZE 4096

int main(void) {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    char buffer[BUFFER_SIZE];
    DWORD bytesRead, bytesWritten;

    while (ReadFile(hStdin, buffer, BUFFER_SIZE, &bytesRead, NULL) && bytesRead > 0) {
        for (DWORD i = 0; i < bytesRead; i++) {
            buffer[i] = (char)tolower((unsigned char)buffer[i]);
        }
        
        WriteFile(hStdout, buffer, bytesRead, &bytesWritten, NULL);
    }

    return 0;
}