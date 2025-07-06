#include <IR_Input.hpp>

#include <unordered_map>

namespace IR::Input {

    static std::unordered_map<UInt32, UInt8> s_KeyStates;
    static UInt8 s_MButtonStates[(UInt8)MButton::_COUNT] = { 0 };

    static glm::ivec2 s_MOldPos = glm::vec2(0);
    static glm::ivec2 s_MPos = glm::vec2(0);

    void Update()
    {
        for (auto& kv : s_KeyStates) {
            kv.second &= 0x2;
        }

        for (UInt32 i = 0; i < IR_ARRLEN(s_MButtonStates); i++) {
            s_MButtonStates[i] &= 0x2;
        }

        s_MOldPos = s_MPos;
    }

    void KeyEvent(Key key, bool down)
    {
        UInt32 iKey = (UInt32)key;
        UInt8& state = s_KeyStates[iKey];

        if (down) {
            if ((state & 0x2) == 0) {
                state = 0x1;
            }

            state |= 0x2;
        } else {
            state = 0x0;
        }
    }

    bool IsKeyPressed(Key key) IR_RETURN(s_KeyStates[(UInt32)key] & 0x1)

    bool IsKeyDown(Key key) IR_RETURN(s_KeyStates[(UInt32)key] & 0x2)

    void MButtonEvent(MButton button, bool pressed)
    {
        UInt8 iMButton = (UInt8)button;
        if (iMButton >= (UInt8)MButton::_COUNT) {
            return;
        }

        UInt8& state = s_MButtonStates[iMButton];

        if (pressed) {
            state = 0x1 | 0x2;
        } else {
            state &= ~0x2;
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