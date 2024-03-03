#include "lockfile.hpp"

#include <fstream>
#include <regex>

#include "hash.hpp"
#include "process.hpp"

static std::vector<std::string> RegexSplitString(const std::string& str, const std::string& regexString) {
    const std::regex regexz(regexString, std::regex::optimize);

    std::vector<std::string> list;

    std::sregex_token_iterator iter(str.begin(), str.end(), regexz, { -1, 1 });
    std::sregex_token_iterator end;

    while (iter != end) {
        if (!iter->str().empty()) {
            list.push_back(iter->str());
        }
        ++iter;
    }

    return list;
};

std::string riot::EncodeLockFile(const riot::lock_file_t& lockfile) {
    return hash::ToBase64(lockfile.m_username + ":" + lockfile.m_password);
}

std::string riot::LockFileAsUrl(const lock_file_t& lockFile) {
    return lockFile.m_protocol + "://127.0.0.1:" + lockFile.m_port;
}

riot::lock_file_t riot::GetLockFileData(const std::filesystem::path& leagueInstallPath) {
    std::ifstream fileStream(leagueInstallPath / "lockfile", std::ios::binary);

    if (!fileStream.good())
        return lock_file_t{};

    std::string data;
    std::getline(fileStream, data);
    fileStream.close();

    const std::vector<std::string> tokens = RegexSplitString(data, ":");

    if (tokens.size() < 5)
        return lock_file_t{};

    return lock_file_t{
        .m_username = "riot",
        .m_processName = tokens[0],
        .m_processId = tokens[1],
        .m_port = tokens[2],
        .m_password = tokens[3],
        .m_protocol = tokens[4]
    };
}

bool riot::IsValidLockFile(const lock_file_t& lockFile) {
    return !lockFile.m_password.empty() && !lockFile.m_port.empty() && !lockFile.m_username.empty();
}

bool riot::GetLockFile(lock_file_t& lockFile) {
    auto handle = process::FindProcess("LeagueClientUx.exe");

    if (!handle->m_isValid) {
        lockFile = {};
        return false;
    }

    auto path = process::GetInstallPathOf(std::move(handle));
    if (path.empty()) {
        lockFile = {};
        return false;
    }

    lockFile = GetLockFileData(path);
    return true;
}
