#include "process.hpp"

#include <tlhelp32.h>
#include <psapi.h>

process::handle_t::~handle_t() {
    if (m_processHandle)
        CloseHandle(m_processHandle);
}

std::unique_ptr<process::handle_t> process::FindProcess(const std::string& processName) {
    auto handle = std::make_unique<handle_t>();
    handle->m_isValid = false;

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(snapshot, &entry) == FALSE) {
        CloseHandle(snapshot);
        return std::move(handle);
    }

    do {
        if (_stricmp(entry.szExeFile, processName.c_str()) != 0)
            continue;

        handle->m_processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, entry.th32ProcessID);
        handle->m_isValid = handle->m_processHandle != INVALID_HANDLE_VALUE;
        break;
    } while (Process32Next(snapshot, &entry) == TRUE);

    CloseHandle(snapshot);
    return std::move(handle);
}

std::filesystem::path process::GetInstallPathOf(std::unique_ptr<process::handle_t> handle) {
    if (!handle->m_isValid)
        return {};

    std::filesystem::path path{};
    char filePath[MAX_PATH];

    if (GetModuleFileNameEx(handle->m_processHandle, nullptr, filePath, MAX_PATH))
        path = std::filesystem::path(filePath).parent_path();

    return path;
}