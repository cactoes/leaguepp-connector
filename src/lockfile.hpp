#ifndef __LOCKFILE_HPP__
#define __LOCKFILE_HPP__

#include <string>
#include <filesystem>

namespace riot {
    struct lock_file_t {
        std::string m_username;
        std::string m_processName;
        std::string m_processId;
        std::string m_port;
        std::string m_password;
        std::string m_protocol;
    };

    std::string EncodeLockFile(const lock_file_t& lockFile);
    std::string LockFileAsUrl(const lock_file_t& lockFile);
    lock_file_t GetLockFileData(const std::filesystem::path& leagueInstallPath);
    bool IsValidLockFile(const lock_file_t& lockFile);

    bool GetLockFile(lock_file_t& lockFile);
}; // namespace riot

#endif