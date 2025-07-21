#include <IR_KeyValue.hpp>

namespace IR {

    KeyValue::KeyValue(const char* key, Type type)
    {
        m_Key = key;
        m_Type = type;

        m_Value.as_str = nullptr;
    }

    KeyValue::~KeyValue()
    {

    }

}