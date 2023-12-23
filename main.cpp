#include<Windows.h>
#include<TlHelp32.h>
#include<iostream>
#include<iomanip>
#include<tchar.h>
#include<vector>
#include<stdlib.h>
#include<thread>
#include<random>
#include<sstream>

HANDLE process = NULL;
DWORD id = 0;



uintptr_t GetModuleAddress(const char* moduleName) {
    MODULEENTRY32 entry;
    entry.dwSize = sizeof(MODULEENTRY32);
    const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, id);
    uintptr_t result = 0;
    while (Module32Next(snapShot, &entry)) {
        if (!strcmp(moduleName, entry.szModule)) {
            result = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
            break;
        }
    }
    if (snapShot) {
        CloseHandle(snapShot);
    }
    return result;
}
int getAddress(HANDLE process, int base, long int offset) {
    int offsets[5] = { 0x0003AA40, 0xC, 0x7B0, 0x118, 0x18 };
    int var = base;
    int currentAddress = base;
    for (int i = 0; i < 5; i++) {
        ReadProcessMemory(process, (LPCVOID)(currentAddress + offsets[i]), &var, sizeof(int), 0);
        currentAddress = var;
    }
    return var + offset;
}
int Read(int address) {
    int var = 0;
    ReadProcessMemory(process, (LPCVOID)(address), &var, sizeof(int), 0);
    return var;
}

bool Write(int address, int var) {
    return WriteProcessMemory(process, (LPVOID)(address), &var, sizeof(int), 0);
}
int main() {

    const char* processName = "SangFroid.exe";

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    while (Process32Next(snapShot, &entry)) {
        if (!strcmp(processName, entry.szExeFile)) {
            id = entry.th32ProcessID;
            process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);
            break;
        }
    }

    if (snapShot) {
        CloseHandle(snapShot);
    }
    if (!process) {
        char temp;
        std::cout << "No process found!";
        std::cin >> temp;
        return 0;
    }
    int offset = 0;
    const auto base = GetModuleAddress("VxMath.dll");
    offset = getAddress(process, base, 0);
    int moneyAddress = offset + 0x20;

    
    std::printf("Choose an option:\n0: Exit\n1: Read money\n2: Set money\n");
    int choice = 0;
    while (true) {
        std::cout << "Choice: ";
        std::cin >> choice;
        switch (choice) {
        case 0: {return 0;}
        case 1: {
            std::printf("Your current money is: %.2f$\n", Read(moneyAddress) / 100.0);
            system("pause");
            break;
        }
        case 2: {
            std::printf("Enter wanted money amount: ");
            float value;
            std::cin >> value;
            if (Write(moneyAddress, (int)(value * 100))) {
                std::printf("Money set to: %.2f$\n", value); system("pause");
            }
            
            break;
        }
        default: {
            break;
        }
        }
        system("cls");
        std::printf("Choose an option:\n0: Exit\n1: Read money\n2: Set money\n");

    }
    
    char temp;
    std::cin >> temp;

}