#include "hash.hpp"

std::string hash::ToBase64(const std::string& input) {
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

uint64_t hash::Fnv1a64HashUnsafe(const char* str, uint64_t hash) {
    while (*str != '\0') {
        hash ^= static_cast<uint64_t>(*str);
        hash *= 1099511628211ULL;
        ++str;
    }
    return hash;
}

uint64_t hash::Fnv1a64Hash(const char* str, uint64_t hash) {
    int i = 0;
    while (str[i] != '\0') {
        hash ^= static_cast<uint64_t>(str[i]);
        hash *= 1099511628211ULL;
        i++;
    }
    return hash;
}