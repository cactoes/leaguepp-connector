#include "include/connector.hpp"
#include "client.hpp"

void connector::CreateConnection() {
    client::Init({ .m_username = "riot", .m_password = "", .m_port = "" });
}