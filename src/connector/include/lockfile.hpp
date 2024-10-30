#pragma once

#ifndef __CONNECTOR_LOCKFILE_HPP__
#define __CONNECTOR_LOCKFILE_HPP__

#include <string>
#include <optional>

namespace connector {
    struct lock_file_t {
        std::string username;
        std::string process_name;
        std::string process_id;
        std::string port;
        std::string password;
        std::string protocol;
    };

    std::string lock_file_to_login_b64(const lock_file_t& lock_file);
    std::string lock_file_to_url(const lock_file_t& lock_file);
    bool lock_file_validate(const lock_file_t& lock_file);
    std::optional<lock_file_t> lock_file_get();
} // namespace connector

#endif // __CONNECTOR_LOCKFILE_HPP__