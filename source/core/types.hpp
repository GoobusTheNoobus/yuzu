#pragma once
#include "core/core.hpp"

// This file does NOT contain type aliases like i32/usize, that is in core/core.hpp
// This file is for the built in types of the language

namespace yuzu {
    enum class BuiltinType { None, Auto, I8, I16, I32, I64, F32, F64, Bool, Char, String, Void };

    inline constexpr std::string_view builtinTypeToString[] = {
        "None", "Auto", "i8", "i16", "i32", "i64", "f32", "f64", "bool", "char", "string", "void"};

    inline bool isInteger(BuiltinType type) {
        return type == BuiltinType::I8 || type == BuiltinType::I16 || type == BuiltinType::I32 || type == BuiltinType::I64;
    }

    inline bool isFloat(BuiltinType type) {
        return type == BuiltinType::F32 || type == BuiltinType::F64;
    }

    inline BuiltinType smallestFittingInt(i64 i) {
        if (i <= INT8_MAX && i >= INT8_MIN)
            return BuiltinType::I8;
        if (i <= INT16_MAX && i >= INT16_MIN)
            return BuiltinType::I16;
        if (i <= INT32_MAX && i >= INT32_MIN)
            return BuiltinType::I32;
        if (i <= INT64_MAX && i >= INT64_MIN)
            return BuiltinType::I64;

        return BuiltinType::None;
    }

} // namespace yuzu