#include <windows.h>
#include <stdio.h>

int g_value = 0; // 全局变量

int main() {
    while (1) {
        // 输出全局变量的地址和当前值
        printf("Address: 0x%p, Value: %d\n", (void*)&g_value, g_value);

        // 更新全局变量的值
        g_value++;

        // 每隔一秒打印一次
        Sleep(1000);
    }
    return 0;
}