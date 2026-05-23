#pragma once

#include <IR_Common.hpp>

namespace IR {
    class BaseEntity {
    public:
        virtual ~BaseEntity() = default;

        virtual bool Spawn() = 0;
        virtual void Destroy() = 0;
        virtual void Precache() = 0;

        virtual void Update(Float64 dt) = 0;
        virtual void Render() = 0;
        virtual void OnKeyValue(const char* key, const char* value) = 0;
        virtual void OnKeyValue(const char* key, Int64 value) = 0;
        virtual void OnKeyValue(const char* key, Float64 value) = 0;

        virtual const char* GetClassName() const = 0;

        void Fire(const char* key, const char* value);
        void Fire(const char* key, Int64 value);
        void Fire(const char* key, Float64 value);

        BaseEntity* GetParent() const IR_RETURN(m_pParent);
        void SetParent(BaseEntity* parent) { m_pParent = parent; }

        void Remove();

        Int64 GetID() const IR_RETURN(m_iID);
        void SetID(Int64 id) { m_iID = id; }

    private:
        Int64 m_iID = 0;
        BaseEntity* m_pParent = nullptr;
    };
}
