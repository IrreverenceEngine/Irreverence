#pragma once

#include <IR_Common.hpp>

#include <algorithm>
#include <vector>

namespace IR {

    template <typename T>
    class Gradient {
    public:
        struct Key {
            T val;
            Float32 t;
        };

        void Add(const T& val, Float32 t)
        {
            m_Keys.push_back({ val, t });
        }

        void Add(const std::vector<Key>& keys)
        {
            m_Keys.reserve(m_Keys.size() + keys.size());
            m_Keys.insert(keys.begin(), keys.end(), m_Keys.end());
        }

        void Sort() // Not necessary if the t are already in order.
        {
            std::sort(m_Keys.begin(), m_Keys.end(), [](Key& a, Key& b) { return a.t < b.t; });
        }

        T Evaluate(Float32 t)
        {
            if (m_Keys.empty()) {
                return {};
            }

            if (t <= m_Keys.front().t) {
                return m_Keys.front().val;
            }

            if (t >= m_Keys.back().t) {
                return m_Keys.back().val;
            }

            for (UInt32 i = 1; i < m_Keys.size(); i++) {
                const Key& r = m_Keys[i];
                if (t <= r.t) {
                    const Key& l = m_Keys[i - 1];
                    return glm::mix(l.val, r.val, (t - l.t) / (r.t - l.t));
                }

            }

            return m_Keys.back().val;
        }

        std::vector<Key> m_Keys;
    };

}
