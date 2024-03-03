#ifndef __PROCESS_HPP__
#define __PROCESS_HPP__

#define WIN32_MEAN_AND_LEAN

#include <Windows.h>
#include <memory>
#include <filesystem>

namespace process {
    struct handle_t {
        HANDLE m_processHandle;
        bool m_isValid;

        ~handle_t();
    };

    std::unique_ptr<handle_t> FindProcess(const std::string& processName);
    std::filesystem::path GetInstallPathOf(std::unique_ptr<handle_t> handle);
}; // namespace process

#endif // __PROCESS_HPP__