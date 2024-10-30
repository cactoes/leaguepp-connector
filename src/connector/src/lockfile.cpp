#include <fstream>
#include <regex>

#include "lockfile.hpp"
#include "process.hpp"

static std::string to_base64(const std::string& input) {
    static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string encoded;
    int val = 0, valb = -6;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            encoded.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) encoded.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    while (encoded.size() % 4) encoded.push_back('=');
    return encoded;
}

static std::vector<std::string> split_string(const std::string& str, const std::string& regex_str) {
    const std::regex regexz(regex_str, std::regex::optimize);

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

std::string connector::lock_file_to_login_b64(const lock_file_t& lock_file) {
    return to_base64(lock_file.username + ":" + lock_file.password);
}

std::string connector::lock_file_to_url(const lock_file_t& lock_file) {
    return lock_file.protocol + "://127.0.0.1:" + lock_file.port;
}

bool connector::lock_file_validate(const lock_file_t& lock_file) {
    return !lock_file.username.empty()
        && !lock_file.process_name.empty()
        && !lock_file.process_id.empty()
        && !lock_file.port.empty()
        && !lock_file.password.empty()
        && !lock_file.protocol.empty();
}

std::optional<connector::lock_file_t> connector::lock_file_get() {
    auto install_path = get_install_path_for("LeagueClientUx.exe");

    if (!install_path.has_value())
        return std::nullopt;

    std::ifstream stream(install_path.value() / "lockfile", std::ios::binary);

    if (!stream.is_open() || !stream.good())
        return std::nullopt;

    std::string data;
    std::getline(stream, data);
    stream.close();

    const std::vector<std::string> tokens = split_string(data, ":");

    if (tokens.size() < 5)
        return std::nullopt;

    lock_file_t lock_file = {
        .username = "riot",
        .process_name = tokens[0],
        .process_id = tokens[1],
        .port = tokens[2],
        .password = tokens[3],
        .protocol = tokens[4]
    };

    return lock_file_validate(lock_file) ? std::optional(lock_file) : std::nullopt;
}