#pragma once

#include <IR_Common.hpp>

#include <string>

namespace IR {
    class CVar {
    public:
        enum class Type {
            Bool,
            Int64,
            Float64,
            String
        };

        CVar(const char* name, UInt32 flags, const bool& v);
        CVar(const char* name, UInt32 flags, const Int64& v);
        CVar(const char* name, UInt32 flags, const Float64& v);
        CVar(const char* name, UInt32 flags, const std::string& v);
        CVar(const char* name, UInt32 flags, const char* v);

        ~CVar();

        UInt32 GetFlags() const;
        CVar::Type GetType() const;
        std::string GetName() const;

        void SetBool(const bool& v);
        void SetInt64(const Int64& v);
        void SetFloat64(const Float64& v);
        void SetString(const std::string& v);
        void SetString(const char* v);

        bool GetBool() const;
        Int64 GetInt64() const;
        Float64 GetFloat64() const;
        std::string GetString() const;

        static CVar* Get(const char* name);

    private:
        UInt32 m_Flags;
        CVar::Type m_Type;

        std::string m_Name;
        union {
            bool as_bool;
            Int64 as_int64;
            Float64 as_float64;
            char* as_string;
        } m_Value;
    };
}