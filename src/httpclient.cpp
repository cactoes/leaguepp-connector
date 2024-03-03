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

int httpclient::MakeRequest(const request_type type, const std::string &endpoint, nlohmann::json& out, const std::string& in) {
    if (!g_client)
        return -1;

    httplib::Result result;

    switch (type) {
        case request_type::PUT:     result = g_client->Put(endpoint, in, "application/json"); break;
        case request_type::POST:    result = g_client->Post(endpoint, in, "application/json"); break;
        case request_type::PATCH:   result = g_client->Patch(endpoint, in, "application/json"); break;
        case request_type::DEL:     result = g_client->Delete(endpoint, in, "application/json"); break;
        case request_type::GET:
        default:
            result = g_client->Get(endpoint);
            break;
    }

    if (!result)
        return 0;

    // false, disable exceptions
    out = nlohmann::json::parse(result->body, nullptr, false);

    return result->status;
}