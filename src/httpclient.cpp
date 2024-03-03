#include "httpclient.hpp"

httpclient::httpclient_t g_client;

void httpclient::CreateClient(const riot::lock_file_t& lockFile) {
    g_client = std::make_unique<httplib::Client>(riot::LockFileAsUrl(lockFile));
    g_client->enable_server_certificate_verification(false);
    g_client->set_basic_auth(lockFile.m_username, lockFile.m_password);
}

void httpclient::Destroy() {
    g_client = nullptr;
}

int httpclient::GetRequest(const std::string &endpoint, nlohmann::json& out) {
    if (!g_client)
        return -1;

    httplib::Result result = g_client->Get(endpoint);

    if (!result)
        return 0;

    // false, disable exceptions
    out = nlohmann::json::parse(result->body, nullptr, false);

    return result->status;
}

int httpclient::PutRequest(const std::string &endpoint, nlohmann::json& out, const std::string& in) {
    if (!g_client)
        return -1;

    httplib::Result result = g_client->Put(endpoint, in, "application/json");

    if (!result)
        return 0;

    // false, disable exceptions
    out = nlohmann::json::parse(result->body, nullptr, false);

    return result->status;
}

int httpclient::PostRequest(const std::string &endpoint, nlohmann::json& out, const std::string& in) {
    if (!g_client)
        return -1;

    httplib::Result result = g_client->Post(endpoint, in, "application/json");

    if (!result)
        return 0;

    // false, disable exceptions
    out = nlohmann::json::parse(result->body, nullptr, false);

    return result->status;
}

int httpclient::PatchRequest(const std::string &endpoint, nlohmann::json& out, const std::string& in) {
    if (!g_client)
        return -1;

    httplib::Result result = g_client->Patch(endpoint, in, "application/json");

    if (!result)
        return 0;

    // false, disable exceptions
    out = nlohmann::json::parse(result->body, nullptr, false);

    return result->status;
}

int httpclient::DeleteRequest(const std::string &endpoint, nlohmann::json& out, const std::string& in) {
    if (!g_client)
        return -1;

    httplib::Result result = g_client->Patch(endpoint, in, "application/json");

    if (!result)
        return 0;

    // false, disable exceptions
    out = nlohmann::json::parse(result->body, nullptr, false);

    return result->status;
}