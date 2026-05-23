#include <IR_Entity.hpp>

namespace IR {

    void BaseEntity::Fire(const char* key, const char* value)
    {
        OnKeyValue(key, value);
    }

    void BaseEntity::Fire(const char* key, Int64 value)
    {
        OnKeyValue(key, value);
    }

    void BaseEntity::Fire(const char* key, Float64 value)
    {
        OnKeyValue(key, value);
    }

    void BaseEntity::Destroy()
    {
        // Custom destruction logic can be added here if needed
    }

}
