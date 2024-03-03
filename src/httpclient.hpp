#ifndef __HTTPCLIENT_HPP__
#define __HTTPCLIENT_HPP__

#pragma warning(disable:4005)
#define CPPHTTPLIB_OPENSSL_SUPPORT
#pragma warning(default:4005)

#include <httplib.h>
#include <memory>
#include <string>
#include <nlohmann/json.hpp>

#include "lockfile.hpp"

namespace httpclient {
    typedef std::unique_ptr<httplib::Client> httpclient_t;

    enum class request_type {
        GET = 0,
        PUT,
        POST,
        PATCH,
        DEL
    };

    void CreateClient(const riot::lock_file_t& lockFile);
    void Destroy();

    int MakeRequest(const request_type type, const std::string& endpoint, nlohmann::json& out, const std::string& in = "");
}; // namespace httpclient

#endif // __HTTPCLIENT_HPP__