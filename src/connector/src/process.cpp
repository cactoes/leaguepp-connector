#include "process.hpp"

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tlhelp32.h>
#include <psapi.h>

static std::optional<HANDLE> find_process(const std::string& process_name) {
    HANDLE handle = nullptr;

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(snapshot, &entry) == FALSE) {
        CloseHandle(snapshot);
        return handle;
    }

    do {
        if (_stricmp(entry.szExeFile, process_name.c_str()) != 0)
            continue;

        handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, entry.th32ProcessID);
        break;
    } while (Process32Next(snapshot, &entry) == TRUE);

    CloseHandle(snapshot);
    return handle == INVALID_HANDLE_VALUE ? std::nullopt : std::optional(handle);
}

std::optional<std::filesystem::path> connector::get_install_path_for(const std::string& process_name) {
    auto handle = find_process(process_name);
    if (!handle.has_value())
        return std::nullopt;

    char file_path[MAX_PATH];
    if (GetModuleFileNameEx(handle.value(), nullptr, file_path, MAX_PATH) == 0)
        return std::nullopt;

    return std::filesystem::path(file_path).parent_path();
}
