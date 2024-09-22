#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <string.h>

int main() {
    // 设置控制台编码为 UTF-8
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    TCHAR szProcessName[MAX_PATH] = TEXT("print.exe"); // 目标进程的名称

    while (1) {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(pe); // 初始化结构体大小

        if (!Process32First(hSnapshot, &pe)) {
            printf("Process32First failed with error %lu\n", GetLastError());
            CloseHandle(hSnapshot);
            Sleep(1000); // 没有找到进程，等待1秒后重试
            continue;
        }

        do {
            if (_tcscmp(pe.szExeFile, szProcessName) == 0) { // 比较进程名称
                printf("找到进程 ID: %lu\n", pe.th32ProcessID);
                DWORD processId = pe.th32ProcessID;
                HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, processId);
                if (hProcess == NULL) {
                    printf("打开进程失败，错误代码：%lu\n", GetLastError());
                    CloseHandle(hSnapshot);
                    Sleep(1000); // 打开进程失败，等待1秒后重试
                    continue;
                }

                // 请求用户输入内存地址
                printf("请输入要修改的内存地址 (例如: 0x0000000000407030)，或输入 'exit' 退出: ");
                char addrInput[20];
                fgets(addrInput, sizeof(addrInput), stdin);
                addrInput[strcspn(addrInput, "\n")] = '\0'; // 去掉换行符

                if (_stricmp(addrInput, "exit") == 0) {
                    printf("退出程序。\n");
                    CloseHandle(hProcess);
                    CloseHandle(hSnapshot);
                    return 0;
                }

                // 检查输入格式是否正确
                if (strncmp(addrInput, "0x", 2) != 0) {
                    printf("地址必须以 '0x' 开头。\n");
                    continue;
                }

                // 解析十六进制地址
                void* address = (void*)strtoull(addrInput + 2, NULL, 16);

                // 检查地址是否有效
                MEMORY_BASIC_INFORMATION mbi;
                if (!VirtualQueryEx(hProcess, (LPCVOID)(DWORD_PTR)address, &mbi, sizeof(mbi))) {
                    printf("无法查询内存地址 0x%p 的信息: %lu\n", address, GetLastError());
                    continue;
                }

                if (!(mbi.Protect & PAGE_EXECUTE_READWRITE || mbi.Protect & PAGE_READWRITE)) {
                    printf("内存地址 0x%p 不可写入\n", address);
                    continue;
                }

                // 请求用户输入要写入的值
                printf("请输入要写入的值: ");
                char valueInput[20];
                fgets(valueInput, sizeof(valueInput), stdin);
                valueInput[strcspn(valueInput, "\n")] = '\0'; // 去掉换行符

                DWORD value = 0;
                if (sscanf(valueInput, "%lu", &value) != 1) {
                    printf("无效的输入。\n");
                    continue;
                }

                // 尝试写入内存
                if (WriteProcessMemory(hProcess, (LPVOID)(DWORD_PTR)address, &value, sizeof(DWORD), NULL)) {
                    printf("成功写入值 %lu 到地址 0x%p\n", value, address);
                } else {
                    printf("无法写入值 %lu 到地址 0x%p: %lu\n", value, address, GetLastError());
                }

                CloseHandle(hProcess);
                CloseHandle(hSnapshot);
                break;
            }
        } while (Process32Next(hSnapshot, &pe));

        if (GetLastError() == ERROR_NO_MORE_FILES) {
            printf("未找到进程 %s\n", szProcessName);
            CloseHandle(hSnapshot);
        }

        Sleep(1000); // 未找到进程，等待1秒后重试
    }

    return 0;
}