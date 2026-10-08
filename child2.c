#include <windows.h>
#include <stdbool.h>

#define BUFFER_SIZE 4096

int main(void) {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    
    char inBuffer[BUFFER_SIZE];
    
    char outBuffer[BUFFER_SIZE];

    DWORD bytesRead, bytesWritten;
    bool prevWasSpace = false;


    while (ReadFile(hStdin, inBuffer, BUFFER_SIZE, &bytesRead, NULL) && bytesRead > 0) {
        DWORD outIdx = 0;

        for (DWORD i = 0; i < bytesRead; i++) {
            char c = inBuffer[i];
            if (c == ' ') {
                if (!prevWasSpace) {
                    outBuffer[outIdx++] = c;
                    prevWasSpace = true;
                }
            } else {
                outBuffer[outIdx++] = c;
                prevWasSpace = false;
            }
        }
        if (outIdx > 0) {
            WriteFile(hStdout, outBuffer, outIdx, &bytesWritten, NULL);
        }
    }

    return 0;
}