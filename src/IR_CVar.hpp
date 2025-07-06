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

        UInt32 GetFlags() const IR_RETURN(m_Flags)
        CVar::Type GetType() const IR_RETURN(m_Type)
        std::string GetName() const IR_RETURN(m_Name)

        void SetBool(const bool& v);
        void SetInt64(const Int64& v);
        void SetFloat64(const Float64& v);
        void SetString(const std::string& v);
        void SetString(const char* v);

        bool GetBool() const IR_RETURN(m_Value.as_bool)
        Int64 GetInt64() const IR_RETURN(m_Value.as_int64)
        Float64 GetFloat64() const IR_RETURN(m_Value.as_float64)
        std::string GetString() const IR_RETURN(std::string(m_Value.as_string))

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