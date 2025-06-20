#include <IR_Input.hpp>

namespace IR::Input {

    static UInt8 s_KeyStates[MAX_KEY];
    static UInt8 s_MButtonStates[MAX_MBUTTON];

    static glm::ivec2 s_MOldPos;
    static glm::ivec2 s_MPos;

    void Init()
    {
        memset(s_KeyStates, 0, sizeof(s_KeyStates));
        memset(s_MButtonStates, 0, sizeof(s_MButtonStates));

        s_MOldPos = glm::vec2(0);
        s_MPos = glm::vec2(0);
    }

    void Update()
    {
        for (UInt32 i = 0; i < IR_ARRLEN(s_KeyStates); i++) {
            s_KeyStates[i] &= 0x2; // Clear pressed state.
        }

        for (UInt32 i = 0; i < IR_ARRLEN(s_MButtonStates); i++) {
            s_MButtonStates[i] &= 0x2; // Clear pressed state.
        }

        s_MOldPos = s_MPos;
    }

    void KeyEvent(Key key, bool down)
    {
        UInt8 iKey = (UInt8)key;
        if (iKey >= MAX_KEY) {
            return;
        }

        UInt8& state = s_KeyStates[iKey];

        if (down) {
            state |= 0x2;
        } else {
            if (state & 0x2) {
                state |= 0x1;
            }

            state &= ~0x2;
        }
    }

    bool IsKeyPressed(Key key) IR_RETURN(s_KeyStates[(UInt8)key] & 0x1)
    bool IsKeyDown(Key key) IR_RETURN(s_KeyStates[(UInt8)key] & 0x2)

    void MButtonEvent(MButton button, bool pressed)
    {
        UInt8 iMButton = (UInt8)button;
        if (iMButton >= MAX_MBUTTON) {
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