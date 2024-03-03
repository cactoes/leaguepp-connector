#ifndef __HASH_HPP__
#define __HASH_HPP__

#include <string>

namespace hash {
    std::string ToBase64(const std::string& input);

    uint64_t Fnv1a64HashUnsafe(const char* str, uint64_t hash = 14695981039346656037ULL);
    uint64_t Fnv1a64Hash(const char* str, uint64_t hash = 14695981039346656037ULL);

    constexpr uint64_t Fnv1a64HashConst(const char* str, uint64_t hash = 14695981039346656037ULL) {
        return (*str == '\0') ? hash :
            Fnv1a64HashConst(str + 1, (hash ^ static_cast<uint64_t>(*str)) * 1099511628211ULL);
    }
}; // namespace hash

#endif // __HASH_HPP__