#pragma once

#include <IR_Common.hpp>

#include <string>
#include <vector>

namespace IR {
    class KeyValue {
	public:
		static KeyValue* Load(const char* path);
		static KeyValue* LoadMemory(const char* data, UInt64 size);

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

        KeyValue(const char* key, const void* data, Type type);
        KeyValue(const char* key, Float64 val);
        KeyValue(const char* key, const char* val);
        ~KeyValue();

		// NOTE: We disable copy and move operations to prevent accidental misuse.
		KeyValue(const KeyValue&) = delete;
		KeyValue& operator=(const KeyValue&) = delete;
		KeyValue(KeyValue&&) = delete;
		KeyValue& operator=(KeyValue&&) = delete;

		Type GetType() const;
		void SetType(Type type);

		std::string GetKey() const;

        Float64 GetNumber() const;
        std::string GetString() const;

		KeyValue* GetChild(UInt64 index) const;
        Float64 GetChildNumber(UInt64 index, Float64 def = 0.0) const;
        std::string GetChildString(UInt64 index, const std::string& def = "") const;
        KeyValue* GetChildObject(UInt64 index) const;
        KeyValue* GetChildArray(UInt64 index) const;

		KeyValue* FindChild(const char* key) const;
        Float64 FindChildNumber(const char* key, Float64 def = 0.0) const;
        std::string FindChildString(const char* key, const std::string& def = "") const;
        KeyValue* FindChildObject(const char* key) const;
        KeyValue* FindChildArray(const char* key) const;

		void AddChildNumber(Float64 value);
		void AddChildString(const char* str);
		KeyValue* AddChildObject();
		KeyValue* AddChildArray();

		void AddChildNumber(const char* key, Float64 value);
        void AddChildString(const char* key, const char* str);
		KeyValue* AddChildObject(const char* key);
		KeyValue* AddChildArray(const char* key);

		UInt64 ChildCount() const;

    private:
        std::string m_Key;
        Type m_Type;
        Data m_Value;
    };
}
