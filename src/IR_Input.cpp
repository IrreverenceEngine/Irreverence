#include <IR_Input.hpp>

#include <unordered_map>

namespace IR::Input {
    constexpr UInt8 PRESSED_BIT = 0x1;
    constexpr UInt8 HOLD_BIT = 0x2;

    static std::unordered_map<UInt32, UInt8> s_KeyStates;
    static UInt8 s_MButtonStates[(UInt8)MButton::_COUNT] = { 0 };

    static glm::ivec2 s_MOldPos = glm::vec2(0);
    static glm::ivec2 s_MPos = glm::vec2(0);

    void Update()
    {
        for (auto& kv : s_KeyStates) {
            kv.second &= HOLD_BIT;
        }

        for (UInt32 i = 0; i < IR_ARRLEN(s_MButtonStates); i++) {
            s_MButtonStates[i] &= HOLD_BIT;
        }

        s_MOldPos = s_MPos;
    }

    void KeyEvent(Key key, bool down)
    {
        UInt32 iKey = (UInt32)key;
        UInt8& state = s_KeyStates[iKey];

        if (down) {
            if ((state & HOLD_BIT) == 0) {
                state = PRESSED_BIT;
            }

            state |= HOLD_BIT;
        } else {
            state = 0;
        }
    }

    bool IsKeyPressed(Key key) IR_RETURN(s_KeyStates[(UInt32)key] & PRESSED_BIT)

    bool IsKeyDown(Key key) IR_RETURN(s_KeyStates[(UInt32)key] & HOLD_BIT)

    void MButtonEvent(MButton button, bool pressed)
    {
        UInt8 iMButton = (UInt8)button;
        if (iMButton >= (UInt8)MButton::_COUNT) {
            return;
        }

        UInt8& state = s_MButtonStates[iMButton];

        if (pressed) {
            state = PRESSED_BIT | HOLD_BIT;
        } else {
            state &= ~HOLD_BIT;
        }
    }

    void MMotionEvent(glm::ivec2 pos)
    {
        s_MOldPos = s_MPos;
        s_MPos = pos;
    }

    bool IsMButtonPressed(MButton button) IR_RETURN(s_MButtonStates[(UInt8)button] & 0x1)
    bool IsMButtonDown(MButton button) IR_RETURN(s_MButtonStates[(UInt8)button] & 0x2)

    glm::ivec2 MousePos() IR_RETURN(s_MPos)
    glm::ivec2 MouseDelta() IR_RETURN(s_MPos - s_MOldPos)

}