#include <IR_KeyValue.hpp>

namespace IR {

    KeyValue::KeyValue(const char* key, const void* data, Type type)
    {
        m_Type = type;

        if (key) {
            m_Key = key;
        }

        switch (type) {
            case Type::NIL: {
                break;
            }

            case Type::NUMBER: {
                m_Value.as_num = *(Float64*)data;
                break;
            }

            case Type::STRING: {
                m_Value.as_str = new std::string((const char*)data);
                break;
            }

            case Type::OBJECT:
            case Type::ARRAY: {
                m_Value.as_child = new std::vector<KeyValue*>();
                break;
            }
        }
    }

    KeyValue::KeyValue(const char* key, Float64 val)
        : KeyValue(key, &val, Type::NUMBER)
    { }

    KeyValue::KeyValue(const char* key, const char* val)
        : KeyValue(key, (const void*)val, Type::STRING)
    { }

    KeyValue::~KeyValue()
    {
        if (m_Type == Type::STRING) {
            delete m_Value.as_str;

            return;
        }

        if (m_Type == Type::OBJECT || m_Type == Type::ARRAY) {
            for (KeyValue* child : *m_Value.as_child) {
                delete child;
            }

            delete m_Value.as_child;
            return;
        }
    }

    KeyValue::Type KeyValue::GetType() const
    {
        return m_Type;
    }

    void KeyValue::SetType(Type type)
    {
        m_Type = type;

        if (type == Type::OBJECT || type == Type::ARRAY) {
            if (!m_Value.as_child) {
                m_Value.as_child = new std::vector<KeyValue*>();
            }
        } else {
            delete m_Value.as_child;
            m_Value.as_child = nullptr;
        }
    }

    std::string KeyValue::GetKey() const
    {
        return m_Key;
    }

    Float64 KeyValue::GetNumber() const
    {
        if (m_Type != Type::NUMBER) {
            return 0.0;
        }

        return m_Value.as_num;
    }

    std::string KeyValue::GetString() const
    {
        if (m_Type != Type::STRING) {
            return "";
        }

        return *m_Value.as_str;
    }

    KeyValue* KeyValue::GetChild(UInt64 index) const
    {
        if (m_Type != Type::OBJECT && m_Type != Type::ARRAY) {
            return nullptr;
        }

        if (index >= m_Value.as_child->size()) {
            return nullptr;
        }

        return (*m_Value.as_child)[index];
    }

    Float64 KeyValue::GetChildNumber(UInt64 index, Float64 def) const
    {
        if (m_Type != Type::ARRAY || index >= m_Value.as_child->size()) {
            return def;
        }

        KeyValue* child = (*m_Value.as_child)[index];

        if (child->m_Type != Type::NUMBER) {
            return def;
        }

        return child->GetNumber();
    }

    std::string KeyValue::GetChildString(UInt64 index, const std::string& def) const
    {
        if (m_Type != Type::ARRAY || index >= m_Value.as_child->size()) {
            return def;
        }

        KeyValue* child = (*m_Value.as_child)[index];

        if (child->m_Type != Type::STRING) {
            return def;
        }

        return child->GetString();
    }

    KeyValue* KeyValue::GetChildObject(UInt64 index) const
    {
        if (m_Type != Type::ARRAY || index >= m_Value.as_child->size()) {
            return nullptr;
        }

        KeyValue* child = (*m_Value.as_child)[index];

        if (child->m_Type != Type::OBJECT) {
            return nullptr;
        }

        return child;
    }

    KeyValue* KeyValue::GetChildArray(UInt64 index) const
    {
        if (m_Type != Type::ARRAY || index >= m_Value.as_child->size()) {
            return nullptr;
        }

        KeyValue* child = (*m_Value.as_child)[index];

        if (child->m_Type != Type::ARRAY) {
            return nullptr;
        }

        return child;
    }

    KeyValue* KeyValue::FindChild(const char* key) const
    {
        if (m_Type != Type::OBJECT) {
            return nullptr;
        }

        for (KeyValue* child : *m_Value.as_child) {
            if (child->m_Key != key) {
                continue;
            }

            return child;
        }

        return nullptr;
    }

    Float64 KeyValue::FindChildNumber(const char* key, Float64 def) const
    {
        if (m_Type != Type::OBJECT) {
            return def;
        }

        for (KeyValue* child : *m_Value.as_child) {
            if (child->m_Key != key) {
                continue;
            }

            if (child->m_Type != Type::NUMBER) {
                continue;
            }

            return child->GetNumber();
        }

        return def;
    }

    std::string KeyValue::FindChildString(const char* key, const std::string& def) const
    {
        if (m_Type != Type::OBJECT) {
            return def;
        }

        for (KeyValue* child : *m_Value.as_child) {
            if (child->m_Key != key) {
                continue;
            }

            if (child->m_Type != Type::STRING) {
                continue;
            }

            return child->GetString();
        }

        return def;
    }

    KeyValue* KeyValue::FindChildObject(const char* key) const
    {
        if (m_Type != Type::OBJECT) {
            return nullptr;
        }

        for (KeyValue* child : *m_Value.as_child) {
            if (child->m_Key != key) {
                continue;
            }

            if (child->m_Type != Type::OBJECT) {
                continue;
            }

            return child;
        }

        return nullptr;
    }

    KeyValue* KeyValue::FindChildArray(const char* key) const
    {
        if (m_Type != Type::OBJECT) {
            return nullptr;
        }

        for (KeyValue* child : *m_Value.as_child) {
            if (child->m_Key != key) {
                continue;
            }

            if (child->m_Type != Type::ARRAY) {
                continue;
            }

            return child;
        }

        return nullptr;
    }

    void KeyValue::AddChildNumber(Float64 value)
    {
        if (m_Type != Type::ARRAY) {
            return;
        }

        KeyValue* child = new KeyValue(nullptr, value);
        m_Value.as_child->push_back(child);
    }

    void KeyValue::AddChildString(const char* str)
    {
        if (m_Type != Type::ARRAY) {
            return;
        }

        KeyValue* child = new KeyValue(nullptr, str);
        m_Value.as_child->push_back(child);
    }

    KeyValue* KeyValue::AddChildObject()
    {
        if (m_Type != Type::ARRAY) {
            return nullptr;
        }

        KeyValue* child = new KeyValue(nullptr, nullptr, Type::OBJECT);
        m_Value.as_child->push_back(child);
        return child;
    }

    KeyValue* KeyValue::AddChildArray()
    {
        if (m_Type != Type::ARRAY) {
            return nullptr;
        }

        KeyValue* child = new KeyValue(nullptr, nullptr, Type::ARRAY);
        m_Value.as_child->push_back(child);
        return child;
    }

    void KeyValue::AddChildNumber(const char* key, Float64 value)
    {
        if (m_Type != Type::OBJECT) {
            return;
        }

        KeyValue* child = new KeyValue(key, value);
        m_Value.as_child->push_back(child);
    }

    void KeyValue::AddChildString(const char* key, const char* str)
    {
        if (m_Type != Type::OBJECT) {
            return;
        }

        KeyValue* child = new KeyValue(key, str);
        m_Value.as_child->push_back(child);
    }

    KeyValue* KeyValue::AddChildObject(const char* key)
    {
        if (m_Type != Type::OBJECT) {
            return nullptr;
        }

        KeyValue* child = new KeyValue(key, nullptr, Type::OBJECT);
        m_Value.as_child->push_back(child);
        return child;
    }

    KeyValue* KeyValue::AddChildArray(const char* key)
    {
        if (m_Type != Type::OBJECT) {
            return nullptr;
        }

        KeyValue* child = new KeyValue(key, nullptr, Type::ARRAY);
        m_Value.as_child->push_back(child);
        return child;
    }

    UInt64 KeyValue::ChildCount() const
    {
        if (m_Type != Type::OBJECT && m_Type != Type::ARRAY) {
            return 0;
        }

        return m_Value.as_child->size();
    }

}
