#pragma once
#include <cstdint>
#include <string>
#include <string_view>

namespace yuzu {

    using u8 = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;

    using i8 = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;

    using usize = size_t;

    using String = std::string;
    using StringView = std::string_view;

    inline constexpr const char* Red = "\033[31m";
    inline constexpr const char* Reset = "\033[0m";
}