#pragma once

#include <IR_Common.hpp>
#include <glm.hpp>

namespace IR::Input {
    enum class Key;
    enum class MButton;

    void Update();

    void KeyEvent(Key key, bool down);

    bool IsKeyPressed(Key key);
    bool IsKeyDown(Key key);

    void MButtonEvent(MButton key, bool pressed);
    void MMotionEvent(glm::ivec2 pos, bool bRelative);

    bool IsMButtonPressed(MButton button);
    bool IsMButtonDown(MButton button);

    glm::ivec2 MousePos();
    glm::ivec2 MouseDelta();

    // Enums
    enum class Key {
        RETURN = 0x0000000du,
        ESCAPE = 0x0000001bu,
        BACKSPACE = 0x00000008u,
        TAB = 0x00000009u,
        SPACE = 0x00000020u,
        EXCLAIM = 0x00000021u,
        DBLAPOSTROPHE = 0x00000022u,
        HASH = 0x00000023u,
        DOLLAR = 0x00000024u,
        PERCENT = 0x00000025u,
        AMPERSAND = 0x00000026u,
        APOSTROPHE = 0x00000027u,
        LEFTPAREN = 0x00000028u,
        RIGHTPAREN = 0x00000029u,
        ASTERISK = 0x0000002au,
        PLUS = 0x0000002bu,
        COMMA = 0x0000002cu,
        MINUS = 0x0000002du,
        PERIOD = 0x0000002eu,
        SLASH = 0x0000002fu,
        ZERO = 0x00000030u,
        ONE = 0x00000031u,
        TWO = 0x00000032u,
        THREE = 0x00000033u,
        FOUR = 0x00000034u,
        FIVE = 0x00000035u,
        SIX = 0x00000036u,
        SEVEN = 0x00000037u,
        EIGHT = 0x00000038u,
        NINE = 0x00000039u,
        COLON = 0x0000003au,
        SEMICOLON = 0x0000003bu,
        LESS = 0x0000003cu,
        EQUALS = 0x0000003du,
        GREATER = 0x0000003eu,
        QUESTION = 0x0000003fu,
        AT = 0x00000040u,
        LEFTBRACKET = 0x0000005bu,
        BACKSLASH = 0x0000005cu,
        RIGHTBRACKET = 0x0000005du,
        CARET = 0x0000005eu,
        UNDERSCORE = 0x0000005fu,
        GRAVE = 0x00000060u,
        A = 0x00000061u,
        B = 0x00000062u,
        C = 0x00000063u,
        D = 0x00000064u,
        E = 0x00000065u,
        F = 0x00000066u,
        G = 0x00000067u,
        H = 0x00000068u,
        I = 0x00000069u,
        J = 0x0000006au,
        K = 0x0000006bu,
        L = 0x0000006cu,
        M = 0x0000006du,
        N = 0x0000006eu,
        O = 0x0000006fu,
        P = 0x00000070u,
        Q = 0x00000071u,
        R = 0x00000072u,
        S = 0x00000073u,
        T = 0x00000074u,
        U = 0x00000075u,
        V = 0x00000076u,
        W = 0x00000077u,
        X = 0x00000078u,
        Y = 0x00000079u,
        Z = 0x0000007au,
        LEFTBRACE = 0x0000007bu,
        PIPE = 0x0000007cu,
        RIGHTBRACE = 0x0000007du,
        TILDE = 0x0000007eu,
        DELETE = 0x0000007fu,
        F1 = 0x4000003au,
        F2 = 0x4000003bu,
        F3 = 0x4000003cu,
        F4 = 0x4000003du,
        F5 = 0x4000003eu,
        F6 = 0x4000003fu,
        F7 = 0x40000040u,
        F8 = 0x40000041u,
        F9 = 0x40000042u,
        F10 = 0x40000043u,
        F11 = 0x40000044u,
        F12 = 0x40000045u,
        PRINTSCREEN = 0x40000046u,
        SCROLLLOCK = 0x40000047u,
        PAUSE = 0x40000048u,
        INSERT = 0x40000049u,
        HOME = 0x4000004au,
        PAGEUP = 0x4000004bu,
        END = 0x4000004du,
        PAGEDOWN = 0x4000004eu,
        RIGHT = 0x4000004fu,
        LEFT = 0x40000050u,
        DOWN = 0x40000051u,
        UP = 0x40000052u,
        LCTRL = 0x400000e0u,
        LSHIFT = 0x400000e1u,
        LALT = 0x400000e2u,
        LGUI = 0x400000e3u,
        RCTRL = 0x400000e4u,
        RSHIFT = 0x400000e5u,
        RALT = 0x400000e6u,
        _COUNT
    };

    enum class MButton {
        LEFT = 1,
        MIDDLE = 2,
        RIGHT = 3,
        _COUNT
    };
}
