#pragma once

#include <IR_Macro.hpp>

namespace IR::Log {
    enum class MsgType {
        INFO,
        WARN,
        ERROR,
        FATAL
    };

    void Msg(MsgType type, const char* fmt, ...);
    void ScrMsg(MsgType type, const char* fmt, ...);

    void DrawScrMsgs();

    // TODO: Output Logs to file.
}

#define IR_MSG(_type, ...) IR::Log::Msg(IR::Log::MsgType::_type, __VA_ARGS__)
#define IR_SCRMSG(_type, ...) IR::Log::ScrMsg(IR::Log::MsgType::_type, __VA_ARGS__)