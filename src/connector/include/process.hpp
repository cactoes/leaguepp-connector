#pragma once

#ifndef __CONNECTOR_PROCESS_HPP__
#define __CONNECTOR_PROCESS_HPP__

#include <string>
#include <optional>
#include <filesystem>

namespace connector {
    std::optional<std::filesystem::path> get_install_path_for(const std::string& process_name);
} // namespace connector

#endif // __CONNECTOR_PROCESS_HPP__