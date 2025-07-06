#pragma once

#include <IR_Macro.hpp>

namespace IR {

    template <typename T>
    class Defer {
    public:
        Defer() = default;
        
        Defer(T func) : m_Func(func) {}

        ~Defer() { m_Func(); }

    private:
        T m_Func;
    };

}

#define IR_DEFER(...) IR::Defer IR_UNIQUE(__ir_defer)([&]() __VA_ARGS__ )