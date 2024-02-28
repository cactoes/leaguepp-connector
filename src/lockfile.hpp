#ifndef __LOCKFILE_HPP__
#define __LOCKFILE_HPP__

#include <string>

struct LockFileT {
    std::string m_username;
    std::string m_password;
    std::string m_port;

    std::string AsBase64() {
        std::string input = m_username + ":" + m_password;

        const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

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
};

#endif