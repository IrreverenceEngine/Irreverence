#pragma once

#include <IR_Common.hpp>

#include <string>
#include <vector>

namespace IR {

    class KeyValue {
    public:
        enum class Type {
            NIL, // Just the key, nothing else.
            NUMBER,
            STRING,
            ARRAY,
            OBJECT
        };

        union Data {
            Float64 as_num;
            std::string* as_str;
            std::vector<KeyValue*>* as_child;
        };

        KeyValue(const char* key, Type type);
        KeyValue(const char* key, Float64 val);
        KeyValue(const char* key, const char* val);
        ~KeyValue();

        Float64 GetNumber() const;
        std::string GetString() const;

        Float64 GetChildNumber(UInt64 index, Float64 def = 0.0) const;
        std::string GetChildString(UInt64 index, const std::string& def = "") const;
        KeyValue* GetChildObject(UInt64 index) const;
        KeyValue* GetChildArray(UInt64 index) const;

        Float64 FindChildNumber(const char* key, Float64 def = 0.0) const;
        std::string FindChildString(const char* key, const std::string& def = "") const;
        KeyValue* FindChildObject(const char* key) const;
        KeyValue* FindChildArray(const char* key) const;

        void AddChildNumber(const char* key, Float64 value);
        void AddChildString(const char* key, const char* str);

    private:
        std::string m_Key;
        Type m_Type;
        Data m_Value;
    };

}