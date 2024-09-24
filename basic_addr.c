#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>

void PrintProcessModules(DWORD processId) {
    HANDLE hProcess;
    HMODULE *hMods;
    DWORD cbNeeded;
    DWORD i;

    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (NULL == hProcess) {
        _tprintf(TEXT("\nCould not open process. Error: %lu\n"), GetLastError());
        return;
    }

    if (!EnumProcessModules(hProcess, NULL, 0, &cbNeeded)) {
        _tprintf(TEXT("EnumProcessModules failed. Error: %lu\n"), GetLastError());
        CloseHandle(hProcess);
        return;
    }

    hMods = (HMODULE *)malloc(cbNeeded);
    if (hMods == NULL) {
        _tprintf(TEXT("Memory allocation failed\n"));
        CloseHandle(hProcess);
        return;
    }

    if (EnumProcessModules(hProcess, hMods, cbNeeded, &cbNeeded)) {
        for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            TCHAR szModName[MAX_PATH];
            if (GetModuleBaseName(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR))) {
                printf("Module %lu at base address: 0x%p - Name: %s\n", i, hMods[i], szModName);
            } else {
                printf("GetModuleBaseName failed for module %lu. Error: %lu\n", i, GetLastError());
            }
        }
    } else {
        _tprintf(TEXT("EnumProcessModules failed. Error: %lu\n"), GetLastError());
    }

    free(hMods);
    CloseHandle(hProcess);
}

int main() {
    DWORD myPid = GetCurrentProcessId();
    printf("Current Process ID: %lu\n", myPid);
    PrintProcessModules(myPid);
    getchar();
    return 0;
}