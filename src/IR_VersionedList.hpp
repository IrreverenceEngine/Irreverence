#pragma once

#include <IR_Common.hpp>

#include <vector>
#include <queue>

namespace IR {
    union Version {
        Version(UInt32 versionnum) : number(versionnum) {}

        struct {
            UInt32 number : 31;
            bool free : 1;
        };

        bool operator==(const Version& other) const IRX_RETURN(number == other.number);
        bool operator!=(const Version& other) const IRX_RETURN(!(*this == other));
    };

    struct Handle {
        Handle(UInt32 index, Version version) : index(index), version(version) {}

        UInt32 index = UINT32_MAX;
        bool operator==(const Handle& other) const IRX_RETURN((index == other.index) && (version == other.version));
        bool operator!=(const Handle& other) const IRX_RETURN(!(*this == other));

        Version version;
    };

    // For stuff like Renderer Objects in the future

    template <typename T>
    class ShiftlessList {
    private:
        struct Slot {
            T val = {};
            Version version = { 0 };
        };

    public:
        Handle Insert(const T& value)
        {
            if (!m_FreeSlots.empty()) {
                UInt32 loc = m_FreeSlots.front();
                m_FreeSlots.pop();

                Slot& slot = m_Slots[loc];
                slot.val = value;
                slot.version.free = false;

                return Handle(loc, slot.version);

            } else {
                UInt32 loc = m_Slots.size();

                Slot slot;
                slot.val = value;
                slot.version.number = 0;
                slot.version.free = false;
                m_Slots.emplace_back(slot);

                return Handle(loc, slot.version);
            }
        }

        void Erase(const Handle& handle)
        {
            if (handle.index >= m_Slots.size()) {
                return;
            }

            Slot& slot = m_Slots[handle.index];
            if (!slot.version.free && slot.version == handle.version) {
                slot.version.free = true;
                slot.version++;
                m_FreeSlots.push(handle.index);
            }
        }

        // This is unsafe
        T* Get(const Handle& handle) const
        {
            if (handle.index >= m_Slots.size()) {
                return nullptr;
            }
            const Slot& slot = m_Slots[handle.index];

            bool isValid = !slot.version.free && (slot.version == handle.version);
            return (isValid) ? &slot.val : nullptr;
        }

        template<typename F>
        void Access(const Handle& handle, F&& func) // I hate move semantics
        {
            if (T* val = Get(handle)) {
                func(*val);
            }
        }

        bool IsValid(const Handle& handle) const
        {
            if (handle.index >= m_Slots.size()) {
                return false;
            }
            const Slot& slot = m_Slots[handle.index];

            return !slot.version.free && (slot.version == handle.version);
        }

        void GetValues(std::vector<T>& out)
        {
            for (const Slot& slot : m_Slots) {
                out.emplace_back(slot);
            }
        }

    private:
        std::vector<Slot> m_Slots;
        std::queue<UInt32> m_FreeSlots;
    };
}